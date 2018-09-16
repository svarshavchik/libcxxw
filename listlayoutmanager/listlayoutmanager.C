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
	impl->run_as([me=ref(this), item_number]
		     (ONLY IN_THREAD)
		     {
			     me->remove_item(IN_THREAD, item_number);
		     });
}

void listlayoutmanagerObj::remove_item(ONLY IN_THREAD, size_t item_number)
{
	impl->list_element_singleton->impl->remove_row(IN_THREAD,
						       ref(this), item_number);
}

void listlayoutmanagerObj::append_items(const std::vector<list_item_param>
					&items)
{
	impl->run_as([items, me=ref(this)]
		     (ONLY IN_THREAD)
		     {
			     me->append_items(IN_THREAD, items);
		     });
}

void listlayoutmanagerObj::append_items(ONLY IN_THREAD,
					const std::vector<list_item_param>
					&items)
{
	impl->list_element_singleton->impl->append_rows(IN_THREAD,
							ref(this), items);
}

void listlayoutmanagerObj::insert_items(size_t item_number,
				       const std::vector<list_item_param>
				       &items)
{
	impl->run_as([me=ref(this), item_number, items]
		     (ONLY IN_THREAD)
		     {
			     me->insert_items(IN_THREAD, item_number, items);
		     });
}

void listlayoutmanagerObj::insert_items(ONLY IN_THREAD,
					size_t item_number,
					const std::vector<list_item_param>
					&items)
{
	impl->list_element_singleton->impl->insert_rows(IN_THREAD, ref(this),
							item_number, items);
}

void listlayoutmanagerObj::replace_items(size_t item_number,
					 const std::vector<list_item_param>
					 &items)
{
	impl->run_as([me=ref(this), item_number, items]
		     (ONLY IN_THREAD)
		     {
			     me->replace_items(IN_THREAD, item_number, items);
		     });
}

void listlayoutmanagerObj::replace_items(ONLY IN_THREAD,
					 size_t item_number,
					 const std::vector<list_item_param>
					 &items)
{
	impl->list_element_singleton->impl->replace_rows(IN_THREAD, ref(this),
							 item_number, items);
}

void listlayoutmanagerObj::replace_all_items(const
					     std::vector<list_item_param>
					     &items)
{
	impl->run_as([items, me=ref(this)]
		     (ONLY IN_THREAD)
		     {
			     me->replace_all_items(IN_THREAD, items);
		     });
}

void listlayoutmanagerObj::replace_all_items(ONLY IN_THREAD,
					     const
					     std::vector<list_item_param>
					     &items)
{
	impl->list_element_singleton->impl->replace_all_rows(IN_THREAD,
							     ref(this),
							     items);
}

void listlayoutmanagerObj::resort_items(const std::vector<size_t> &indexes)
{
	impl->run_as([indexes, me=ref(this)]
		     (ONLY IN_THREAD)
		     {
			     me->resort_items(IN_THREAD, indexes);
		     });
}

void listlayoutmanagerObj::resort_items(ONLY IN_THREAD,
					const std::vector<size_t> &indexes)
{
	auto copy=indexes;

	impl->list_element_singleton->impl->resort_rows(IN_THREAD,
							ref(this),
							copy);
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

void listlayoutmanagerObj::selected(size_t i, bool selected_flag)
{
	impl->run_as([me=ref(this), i, selected_flag]
		     (ONLY IN_THREAD)
		     {
			     me->selected(IN_THREAD, i, selected_flag, {});
		     });
}

void listlayoutmanagerObj::selected(ONLY IN_THREAD,
				    size_t i, bool selected_flag,
				    const callback_trigger_t &trigger)
{
	impl->list_element_singleton->impl
		->selected(IN_THREAD, ref(this), i, selected_flag,
			   trigger);
}

void listlayoutmanagerObj::autoselect(size_t i)
{
	impl->run_as([me=ref(this),i]
		     (ONLY IN_THREAD)
		     {
			     me->autoselect(IN_THREAD, i, {});
		     });
}

void listlayoutmanagerObj::autoselect(ONLY IN_THREAD,
				      size_t i,
				      const callback_trigger_t &trigger)
{
	impl->list_element_singleton->impl->autoselect(IN_THREAD,
						       ref(this), i, trigger);
}

void listlayoutmanagerObj::unselect()
{
	impl->run_as([me=ref(this)]
		     (ONLY IN_THREAD)
		     {
			     me->unselect(IN_THREAD);
		     });
}

void listlayoutmanagerObj::unselect(ONLY IN_THREAD)
{
	impl->list_element_singleton->impl->unselect(IN_THREAD, ref(this));
}

bool listlayoutmanagerObj::enabled(size_t i) const
{
	return impl->list_element_singleton->impl->enabled(i);
}

void listlayoutmanagerObj::enabled(size_t i, bool flag)
{
	impl->run_as([me=ref(this), i, flag]
		     (ONLY IN_THREAD)
		     {
			     me->enabled(IN_THREAD, i, flag);
		     });
}

void listlayoutmanagerObj::enabled(ONLY IN_THREAD, size_t i, bool flag)
{
	impl->list_element_singleton->impl->enabled(IN_THREAD, i, flag);
}

listlayoutmanagerptr
listlayoutmanagerObj::get_item_layoutmanager(size_t i)
{
	return impl->list_element_singleton->impl->get_item_layoutmanager(i);
}

LIBCXXW_NAMESPACE_END
