/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include <type_traits>

LIBCXXW_NAMESPACE_START

list_item_param::~list_item_param()=default;

list_item_param::list_item_param(const list_item_param &)=default;
list_item_param::list_item_param(list_item_param &&)=default;

list_item_param &list_item_param::operator=(const list_item_param &)=default;
list_item_param &list_item_param::operator=(list_item_param &&)=default;

listlayoutmanagerObj::listlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl),
	  impl(impl)
{
}

listlayoutmanagerObj::~listlayoutmanagerObj()=default;

void listlayoutmanagerObj::remove_item(size_t item_number)
{
	impl->list_element_singleton->impl->remove_row(ref(this), item_number);
}

void listlayoutmanagerObj::append_items(const std::vector<list_item_param>
				       &items)
{
	impl->list_element_singleton->impl->append_rows(ref(this), items);
}

void listlayoutmanagerObj::insert_items(size_t item_number,
				       const std::vector<list_item_param>
				       &items)
{
	impl->list_element_singleton->impl->insert_rows(ref(this),
						  item_number, items);
}

void listlayoutmanagerObj::replace_items(size_t item_number,
					const std::vector<list_item_param>
					&items)
{
	impl->list_element_singleton->impl->replace_rows(ref(this),
						   item_number, items);
}

void listlayoutmanagerObj::replace_all_items(const
					     std::vector<list_item_param>
					     &items)
{
	impl->list_element_singleton->impl->replace_all_rows(ref(this),
						       items);
}

size_t listlayoutmanagerObj::size() const
{
	return impl->list_element_singleton->impl->size();
}

bool listlayoutmanagerObj::selected(size_t i) const
{
	return impl->list_element_singleton->impl->selected(i);
}

std::optional<size_t> listlayoutmanagerObj::selected() const
{
	return impl->list_element_singleton->impl->selected();
}

std::vector<size_t> listlayoutmanagerObj::all_selected() const
{
	return impl->list_element_singleton->impl->all_selected();
}

void listlayoutmanagerObj::selected(size_t i, bool selected_flag,
				    const callback_trigger_t &trigger)
{
	impl->list_element_singleton->impl->selected(ref(this), i, selected_flag,
					       trigger);
}

void listlayoutmanagerObj::autoselect(size_t i)
{
	autoselect(i, {});
}

void listlayoutmanagerObj::autoselect(size_t i,
				      const callback_trigger_t &trigger)
{
	impl->list_element_singleton->impl->autoselect(ref(this), i, trigger);
}

void listlayoutmanagerObj::unselect()
{
	impl->list_element_singleton->impl->unselect(ref(this));
}

bool listlayoutmanagerObj::enabled(size_t i) const
{
	return impl->list_element_singleton->impl->enabled(i);
}

void listlayoutmanagerObj::enabled(size_t i, bool flag)
{
	impl->list_element_singleton->impl->enabled(i, flag);
}

listlayoutmanagerptr
listlayoutmanagerObj::get_item_layoutmanager(size_t i)
{
	return impl->list_element_singleton->impl->get_item_layoutmanager(i);
}

LIBCXXW_NAMESPACE_END
