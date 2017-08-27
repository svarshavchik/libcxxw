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
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

[[noreturn]] static void nosuchitem(size_t i);
[[noreturn]] static void notimplemented();

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

void standard_combobox_lock::append_item(const text_param &item)
{
	auto f=locked_layoutmanager->listlayoutmanagerObj::append_item();
	f->create_label(item);
	text_items().push_back(item);
}

void standard_combobox_lock::append_separator()
{
	locked_layoutmanager->listlayoutmanagerObj::append_separator();
	text_items().push_back({});
}

void standard_combobox_lock::insert_item(size_t i, const text_param &item)
{
	auto &ti=text_items();

	if (ti.size() < i)
		nosuchitem(i);

	auto f=locked_layoutmanager->listlayoutmanagerObj::insert_item(i);
	f->create_label(item);
	ti.insert(ti.begin()+i, item);
}

void standard_combobox_lock::insert_separator(size_t item_number)
{
	auto &ti=text_items();

	if (ti.size() < item_number)
		nosuchitem(item_number);

	locked_layoutmanager->listlayoutmanagerObj
		::insert_separator(item_number);

	ti.insert(ti.begin()+item_number, {});
}

void standard_combobox_lock::replace_item(size_t i,
					  const text_param &item)
{
	auto &ti=text_items();

	if (ti.size() < i)
		nosuchitem(i);

	auto f=locked_layoutmanager->listlayoutmanagerObj::replace_item(i);
	f->create_label(item);
	ti.at(i)=item;
}

void standard_combobox_lock::replace_separator(size_t item_number)
{
	auto &ti=text_items();

	if (ti.size() < item_number)
		nosuchitem(item_number);

	locked_layoutmanager->listlayoutmanagerObj
		::replace_separator(item_number);
	ti.at(item_number)={};
}


void standard_combobox_lock::remove_item(size_t i)
{
	auto &ti=text_items();

	if (ti.size() <= i)
		nosuchitem(i);

	locked_layoutmanager->listlayoutmanagerObj::remove_item(i);
	ti.erase(ti.begin()+i);
}

void standard_combobox_lock::replace_all_items(const std::vector<text_param>
					       &items)
{
	// Try to do the right thing when an exception gets thrown.

	auto s=make_sentry
		([&, this]
		 {
			 this->locked_layoutmanager
				 ->listlayoutmanagerObj::replace_all_items();
		 });

	// However, manually call unselect() ourselves, and don't arm the
	// sentry until everything is unselected, so the unselect() in
	// the list layout manager does nothing. We'll invoke all unselection
	// triggered callbacks before arming the sentry.

	locked_layoutmanager->unselect();

	s.guard();
	auto f=locked_layoutmanager
		->listlayoutmanagerObj::replace_all_items();

	for (const auto &item:items)
		f->create_label(item);

	text_items()=items;
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

		if (l == search_size)
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

static void notimplemented()
{
	throw EXCEPTION(_("Operation not supported for a standard combo-box"));
}

void standard_comboboxlayoutmanagerObj
::append_item(const std::vector<text_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	for (const auto &item:items)
		lock.append_item(item);
}

void standard_comboboxlayoutmanagerObj::append_separator()
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.append_separator();
}

void standard_comboboxlayoutmanagerObj::insert_separator(size_t item_number)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.insert_separator(item_number);
}

void standard_comboboxlayoutmanagerObj::replace_separator(size_t item_number)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.replace_separator(item_number);
}

void standard_comboboxlayoutmanagerObj
::insert_item(size_t i,
	      const std::vector<text_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	for (const auto &item:items)
		lock.insert_item(i++, item);
}

void standard_comboboxlayoutmanagerObj
::replace_item(size_t i,
	       const std::vector<text_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	if (i+items.size() > lock.text_items().size())
		nosuchitem(items.size()-i);

	for (const auto &item:items)
		lock.replace_item(i++, item);
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
::replace_all_items(const std::vector<text_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.replace_all_items(items);
}

factory standard_comboboxlayoutmanagerObj::append_item()
{
	notimplemented();
}

factory standard_comboboxlayoutmanagerObj::insert_item(size_t)
{
	notimplemented();
}

factory standard_comboboxlayoutmanagerObj::replace_all_items()
{
	notimplemented();
}

factory standard_comboboxlayoutmanagerObj::replace_item(size_t)
{
	notimplemented();
}

//////////////////////////////////////////////////////////////////////////////

// The standard combo-box uses a focusable_label to represent the current
// selection.

new_standard_comboboxlayoutmanager::new_standard_comboboxlayoutmanager()
	: new_custom_comboboxlayoutmanager
	  ([]
	   (const auto &f,
	    const auto &ignore)
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
				if (!lm->selected(lock, found))
					lm->autoselect(lock, found);
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
