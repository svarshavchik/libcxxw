/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlistlayoutmanager/textlistlayoutmanager_impl.H"
#include "textlistlayoutmanager/textlist_impl.H"
#include <type_traits>

LIBCXXW_NAMESPACE_START

list_item_param::~list_item_param()=default;

list_item_param::list_item_param(const list_item_param &)=default;
list_item_param::list_item_param(list_item_param &&)=default;

list_item_param &list_item_param::operator=(const list_item_param &)=default;
list_item_param &list_item_param::operator=(list_item_param &&)=default;

textlistlayoutmanagerObj::textlistlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl),
	  impl(impl)
{
}

textlistlayoutmanagerObj::~textlistlayoutmanagerObj()=default;

void textlistlayoutmanagerObj::remove_item(size_t item_number)
{
	impl->textlist_element->impl->remove_row(ref(this), item_number);
}

void textlistlayoutmanagerObj::append_item(const std::vector<text_param> &items)
{
	append_item(std::vector<list_item_param>(items.begin(), items.end()));
}

void textlistlayoutmanagerObj::append_item(const std::vector<list_item_param>
					   &items)
{
	impl->textlist_element->impl->append_rows(ref(this), items);
}

void textlistlayoutmanagerObj::insert_item(size_t item_number,
					   const std::vector<text_param> &items)
{
	insert_item(item_number,
		    std::vector<list_item_param>(items.begin(), items.end()));
}

void textlistlayoutmanagerObj::insert_item(size_t item_number,
					   const std::vector<list_item_param>
					   &items)
{
	impl->textlist_element->impl->insert_rows(ref(this),
						  item_number, items);
}

void textlistlayoutmanagerObj::replace_item(size_t item_number,
					    const std::vector<text_param> &items)
{
	replace_item(item_number,
		     std::vector<list_item_param>(items.begin(), items.end()));
}

void textlistlayoutmanagerObj::replace_item(size_t item_number,
					    const std::vector<list_item_param>
					    &items)
{
	impl->textlist_element->impl->replace_rows(ref(this),
						   item_number, items);
}

void textlistlayoutmanagerObj::replace_all_items(const std::vector<text_param>
						 &items)
{
	replace_all_items(std::vector<list_item_param>(items.begin(),
						       items.end()));
}

void textlistlayoutmanagerObj::replace_all_items(const
						 std::vector<list_item_param>
						 &items)
{
	impl->textlist_element->impl->replace_all_rows(ref(this),
						       items);
}

size_t textlistlayoutmanagerObj::size() const
{
	return impl->textlist_element->impl->size();
}

bool textlistlayoutmanagerObj::selected(size_t i) const
{
	return impl->textlist_element->impl->selected(i);
}

std::optional<size_t> textlistlayoutmanagerObj::selected() const
{
	return impl->textlist_element->impl->selected();
}

std::vector<size_t> textlistlayoutmanagerObj::all_selected() const
{
	return impl->textlist_element->impl->all_selected();
}

void textlistlayoutmanagerObj::selected(size_t i, bool selected_flag,
					const callback_trigger_t &trigger)
{
	impl->textlist_element->impl->selected(ref(this), i, selected_flag,
					       trigger);
}

void textlistlayoutmanagerObj::autoselect(size_t i,
					  const callback_trigger_t &trigger)
{
	impl->textlist_element->impl->autoselect(ref(this), i, trigger);
}

void textlistlayoutmanagerObj::unselect()
{
	impl->textlist_element->impl->unselect(ref(this));
}

bool textlistlayoutmanagerObj::enabled(size_t i) const
{
	return impl->textlist_element->impl->enabled(i);
}

void textlistlayoutmanagerObj::enabled(size_t i, bool flag)
{
	impl->textlist_element->impl->enabled(i, flag);
}

textlistlayoutmanagerptr
textlistlayoutmanagerObj::get_item_layoutmanager(size_t i)
{
	return impl->textlist_element->impl->get_item_layoutmanager(i);
}

LIBCXXW_NAMESPACE_END
