/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/standard_comboboxlayoutmanager.H"
#include "x/w/focusable_label.H"
#include "x/w/label.H"
#include "messages.H"
#include <x/exception.H>
#include <x/sentry.H>
#include <x/visitor.H>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

[[noreturn]] static void nosuchitem(size_t i);

const_standard_combobox_lock
::const_standard_combobox_lock(const const_standard_comboboxlayoutmanager &ll)
	: list_lock(ll), locked_layoutmanager(ll)
{
}

const_standard_combobox_lock::~const_standard_combobox_lock()=default;

std::vector<text_param> &const_standard_combobox_lock::text_items() const
{
	return locked_layoutmanager->impl->text_items_protected;
}

size_t const_standard_combobox_lock::size() const
{
	return text_items().size();
}

text_param const_standard_combobox_lock::item(size_t i) const
{
	auto &ti=text_items();

	if (ti.size() <= i)
		nosuchitem(i);

	return ti.at(i);
}

standard_combobox_lock
::standard_combobox_lock(const standard_comboboxlayoutmanager &ll)
	: const_standard_combobox_lock(ll), locked_layoutmanager(ll)
{
}

standard_combobox_lock::~standard_combobox_lock()=default;

static std::vector<text_param> to_text_param(const std::vector<list_item_param>
					     &items)
{
	std::vector<text_param> ret;

	for (const list_item_param::variant_t &i:items)
		std::visit
			(visitor
			 {
				 [&](const text_param &t)
				 {
					 ret.push_back(t);
				 },
				 [&](const separator &s)
				 {
					 ret.push_back({});
				 },
				 [](const list_item_status_change_callback &)
				 {
				 },
				 [](const auto &s)
				 {
					 throw EXCEPTION(_("This combo-box cannot contain this item"));
				 }
			 }, i);
	return ret;
}

void standard_combobox_lock::append_item(const list_item_param &item)
{
	append_items({item});
}

void standard_combobox_lock::append_items(const std::vector<list_item_param>
					  &items)
{
	auto t=to_text_param(items);

	auto &ti=text_items();

	// Try to do the right thing when an exception gets thrown.

	auto s=make_sentry
		([&, this, s=ti.size()]
		 {
			 ti.erase(ti.begin()+s, ti.end());
		 });
	ti.insert(ti.end(), t.begin(), t.end());

	s.guard();
	locked_layoutmanager->superclass_t::append_item(items);
	s.unguard();
}

void standard_combobox_lock::insert_item(size_t i, const list_item_param &item)
{
	insert_items(i, {item});
}

void standard_combobox_lock::insert_items(size_t i,
					  const std::vector<list_item_param>
					  &items)
{
	auto t=to_text_param(items);
	auto &ti=text_items();

	if (ti.size() < i)
		nosuchitem(i);

	auto s=make_sentry
		([&, this]
		 {
			 ti.erase(ti.begin()+i,ti.begin()+t.size());
		 });

	ti.insert(ti.begin()+i, t.begin(), t.end());

	s.guard();
	locked_layoutmanager->superclass_t::insert_item(i, items);
	s.unguard();
}

void standard_combobox_lock::replace_item(size_t i,
					  const list_item_param &item)
{
	replace_items(i, {item});
}

void standard_combobox_lock::replace_items(size_t i,
					   const std::vector<list_item_param>
					   &items)
{
	auto t=to_text_param(items);

	auto &ti=text_items();

	if (ti.size() < i || ti.size()-i < t.size())
		nosuchitem(i);

	locked_layoutmanager->superclass_t::replace_item(i, items);

	std::copy(t.begin(), t.end(), ti.begin()+i);
}

void standard_combobox_lock::remove_item(size_t i)
{
	auto &ti=text_items();

	if (ti.size() <= i)
		nosuchitem(i);

	locked_layoutmanager->superclass_t::remove_item(i);
	ti.erase(ti.begin()+i);
}

void standard_combobox_lock::replace_all_items(const std::vector<list_item_param>
					       &items)
{
	auto t=to_text_param(items);

	// Try to do the right thing when an exception gets thrown.

	auto s=make_sentry
		([&, this]
		 {
			 this->locked_layoutmanager
				 ->superclass_t::replace_all_items(std::vector<list_item_param>{});
			 text_items().clear();
		 });

	s.guard();
	locked_layoutmanager->superclass_t::replace_all_items(items);

	text_items()=t;
	s.unguard();
}

bool standard_combobox_lock::search(size_t starting_index,
				    const std::u32string &text,
				    size_t &found,
				    bool shortest_match) const
{
	auto &ti=text_items();

	size_t n=ti.size();

	size_t search_size=text.size();

	bool was_found=false;
	size_t was_found_size=0;

	for (size_t i=0; i<n; ++i)
	{
		size_t j=(i+starting_index) % n;

		const auto &string=ti.at(j).string;

		if (string.size() < search_size)
			continue;

		size_t l;

		for (l=0; l<search_size; ++l)
			if (unicode_lc(string[l]) != unicode_lc(text[l]))
				break;

		if (l == search_size && locked_layoutmanager->enabled(i))
		{
			if (!was_found || was_found_size > string.size())
			{
				found=j;
				was_found=true;
				was_found_size=string.size();
				if (!shortest_match)
					return true;
				// else keep searching.
			}
		}
	}

	return was_found;
}

//////////////////////////////////////////////////////////////////////////


standard_comboboxlayoutmanagerObj
::standard_comboboxlayoutmanagerObj(const ref<implObj> &impl,
				    const ref<listlayoutmanagerObj::implObj>
				    &list_layout_impl)
	: custom_comboboxlayoutmanagerObj(impl, list_layout_impl),
	  impl(impl)
{
}

standard_comboboxlayoutmanagerObj::~standard_comboboxlayoutmanagerObj()=default;

static void nosuchitem(size_t i)
{
	throw EXCEPTION(gettextmsg(_("Item %1% does not exist."), i));
}

void standard_comboboxlayoutmanagerObj
::append_item(const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.append_items(items);
}

void standard_comboboxlayoutmanagerObj
::insert_item(size_t i,
	      const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.insert_items(i, items);
}

void standard_comboboxlayoutmanagerObj
::replace_item(size_t i,
	       const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.replace_items(i, items);
}

void standard_comboboxlayoutmanagerObj::remove_item(size_t i)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.remove_item(i);
}

text_param standard_comboboxlayoutmanagerObj::item(size_t i) const
{
	const_standard_combobox_lock
		lock{const_standard_comboboxlayoutmanager(this)};

	return lock.item(i);
}

void standard_comboboxlayoutmanagerObj
::replace_all_items(const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.replace_all_items(items);
}

//////////////////////////////////////////////////////////////////////////////

// The standard combo-box uses a focusable_label to represent the current
// selection.

new_standard_comboboxlayoutmanager::new_standard_comboboxlayoutmanager()
	: new_custom_comboboxlayoutmanager
	  ([]
	   (const auto &f)
	   {
		   return f->create_focusable_label("");
	   })
{
	// Install callback to search items using whatever was typed into the
	// current selection display element.

	selection_search=
		[]
		(const auto &search_info)
		{
			standard_comboboxlayoutmanager lm=search_info.lm;

			if (search_info.text.size() == 0)
			{
				lm->unselect();
				return;
			}

			size_t found;

			standard_combobox_lock lock{lm};

			if (lock.search(search_info.starting_index,
					search_info.text,
					found, false))
			{
				if (!lm->selected(found))
					lm->autoselect(found);
			}
		};
}

new_standard_comboboxlayoutmanager
::new_standard_comboboxlayoutmanager(const standard_combobox_selection_changed_t
				     &selection_changed)
	: new_standard_comboboxlayoutmanager()
{
	this->selection_changed=selection_changed;
}

new_standard_comboboxlayoutmanager
::~new_standard_comboboxlayoutmanager()=default;

custom_combobox_selection_changed_t new_standard_comboboxlayoutmanager
::get_selection_changed() const
{
	return [cb=this->selection_changed]
		(const auto &info)
	{
		standard_comboboxlayoutmanager lm=info.lm;
		x::w::focusable_label current_selection=info.current_selection;

		standard_combobox_lock lock{lm};

		info.popup_element->hide();
		if (info.selected_flag)
		{
			current_selection->update
				(lock.item(info.item_index));
		}
		else // Unselected.
		{
			current_selection->update("");
		}
		cb({lock, info.item_index, info.selected_flag,
					info.mcguffin});
	};
}

ref<custom_comboboxlayoutmanagerObj::implObj>
new_standard_comboboxlayoutmanager
::create_impl(const create_impl_info &i) const
{
	return ref<standard_comboboxlayoutmanagerObj::implObj>
		::create(i.container_impl, i.style);
}

LIBCXXW_NAMESPACE_END
