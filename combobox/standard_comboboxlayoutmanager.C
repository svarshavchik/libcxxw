/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/standard_comboboxlayoutmanager.H"
#include "x/w/focusable_label.H"
#include "messages.H"
#include <x/exception.H>
#include <x/sentry.H>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

standard_comboboxlayoutmanagerObj
::standard_comboboxlayoutmanagerObj(const ref<implObj> &impl,
				    const ref<listlayoutmanagerObj::implObj>
				    &list_layout_impl)
	: custom_comboboxlayoutmanagerObj(impl, list_layout_impl),
	  impl(impl)
{
}

standard_comboboxlayoutmanagerObj::~standard_comboboxlayoutmanagerObj()=default;

[[noreturn]] static void nosuchitem(size_t i);

static void nosuchitem(size_t i)
{
	throw EXCEPTION(gettextmsg(_("Item %1% does not exist."), i));
}

void standard_comboboxlayoutmanagerObj::append_item(const text_param &item)
{
	grid_map_t::lock lock{impl->grid_map};

	listlayoutmanagerObj::append_item(item);
	impl->text_items(lock).push_back(item);
}

void standard_comboboxlayoutmanagerObj::insert_item(size_t i,
						    const text_param &item)
{
	grid_map_t::lock lock{impl->grid_map};

	if (impl->text_items(lock).size() < i)
		nosuchitem(i);

	listlayoutmanagerObj::insert_item(i, item);
	impl->text_items(lock).insert(impl->text_items(lock).begin()+i,
				      item);
}

void standard_comboboxlayoutmanagerObj::remove_item(size_t i)
{
	grid_map_t::lock lock{impl->grid_map};

	if (impl->text_items(lock).size() <= i)
		nosuchitem(i);
	custom_comboboxlayoutmanagerObj::remove_item(i);
	impl->text_items(lock).erase(impl->text_items(lock).begin()+i);
}

size_t standard_comboboxlayoutmanagerObj::size() const
{
	grid_map_t::lock lock{impl->grid_map};

	return impl->text_items(lock).size();
}

text_param standard_comboboxlayoutmanagerObj::item(size_t i) const
{
	grid_map_t::lock lock{impl->grid_map};

	if (impl->text_items(lock).size() <= i)
		nosuchitem(i);

	return impl->text_items(lock).at(i);
}

void standard_comboboxlayoutmanagerObj
::replace_all(const std::vector<text_param> &items)
{
	// Try to do the right thing when an exception gets thrown.

	auto s=make_sentry
		([&, this]
		 {
			 this->replace_all();
		 });

	// However, manually call unselect() ourselves, and don't arm the
	// sentry until everything is unselect, so the unselect() in
	// the list layout manager does nothing. We'll invoke all unselection
	// triggered callbacks before arming the sentry.

	grid_map_t::lock lock{impl->grid_map};

	unselect();

	s.guard();
	custom_comboboxlayoutmanagerObj::replace_all(items);

	impl->text_items(lock)=items;
	s.unguard();
}

factory standard_comboboxlayoutmanagerObj::replace_all()
{
	grid_map_t::lock lock{impl->grid_map};

	auto f=custom_comboboxlayoutmanagerObj::replace_all();

	impl->text_items(lock).clear();

	return f;
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

			size_t n=lm->impl->text_items(search_info.lock).size();

			if (search_info.text.size() == 0 || n == 0)
			{
				lm->unselect();
				return;
			}

			size_t search_size=search_info.text.size();

			for (size_t i=0; i<n; ++i)
			{
				size_t j=(i+search_info.starting_index) % n;

				const auto &string=lm->impl->text_items
					(search_info.lock).at(j).string;

				if (string.size() < search_size)
					continue;

				size_t l;

				for (l=0; l<search_size; ++l)
					if (unicode_lc(string[l]) !=
					    unicode_lc(search_info.text[l]))
						break;

				if (l == search_size)
				{
					lm->autoselect(search_info.lock, j);
					return;
				}
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

		info.popup_element->hide();
		if (info.selected_flag)
		{
			current_selection->update
				(lm->impl->text_items(info.lock)
				 .at(info.item_index));
		}
		else // Unselected.
		{
			current_selection->update("");
		}
		cb({info.lock, lm, info.item_index,
					info.selected_flag,
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
