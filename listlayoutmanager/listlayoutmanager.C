/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/listitemhandle_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "listlayoutmanager/list_cell.H"
#include "listlayoutmanager/extra_list_row_info.H"
#include "listlayoutmanager/in_thread_new_cells_info.H"
#include "popup/popup_handler.H"
#include <type_traits>

LIBCXXW_NAMESPACE_START

new_items_ret::new_items_ret()=default;
new_items_ret::~new_items_ret()=default;

list_item_param::~list_item_param()=default;

list_item_param::list_item_param(const list_item_param &)=default;
list_item_param::list_item_param(list_item_param &&)=default;

list_item_param &list_item_param::operator=(const list_item_param &)=default;
list_item_param &list_item_param::operator=(list_item_param &&)=default;

listlayoutmanagerObj::listlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj{impl},
	  impl{impl}
{
}

listlayoutmanagerObj::~listlayoutmanagerObj()=default;

void listlayoutmanagerObj::remove_item(size_t item_number)
{
	remove_items(item_number, 1);
}

void listlayoutmanagerObj::remove_items(size_t item_number,
					size_t n_items)
{
	notmodified();

	impl->run_as([impl=this->impl, item_number, n_items]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();

			     me->impl->list_element_singleton->impl
				     ->remove_rows(IN_THREAD,
						   me,
						   item_number,
						   n_items);
		     });
}

void listlayoutmanagerObj::remove_item(ONLY IN_THREAD, size_t item_number)
{
	remove_items(IN_THREAD, item_number, 1);
}

void listlayoutmanagerObj::remove_items(ONLY IN_THREAD, size_t item_number,
					size_t n_items)
{
	impl->list_element_singleton->impl->remove_rows(IN_THREAD,
							ref{this},
							item_number,
							n_items);
}

new_items_ret
listlayoutmanagerObj::append_items(const std::vector<list_item_param> &items)
{
	notmodified();

	new_items_ret ret;

	auto cells=in_thread_new_cells_info::create(ref{this}, items, ret);

	impl->run_as([cells, impl=this->impl]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();
			     me->impl->list_element_singleton->impl
				     ->append_rows(IN_THREAD, me, cells->info);
		     });

	return ret;
}

new_items_ret listlayoutmanagerObj
::append_items(ONLY IN_THREAD, const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	auto list_impl=impl->list_element_singleton->impl;

	new_cells_info info{ret};

	list_impl->list_style.create_cells(items, impl, info);

	list_impl->append_rows(IN_THREAD, ref{this}, info);

	return ret;
}

new_items_ret listlayoutmanagerObj
::insert_items(size_t item_number, const std::vector<list_item_param> &items)
{
	notmodified();

	new_items_ret ret;

	auto cells=in_thread_new_cells_info::create(ref{this}, items, ret);

	impl->run_as([impl=this->impl, item_number, cells]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();
			     me->impl->list_element_singleton->impl
				     ->insert_rows(IN_THREAD, me, item_number,
						   cells->info);
		     });

	return ret;
}

new_items_ret listlayoutmanagerObj
::insert_items(ONLY IN_THREAD, size_t item_number,
	       const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	auto list_impl=impl->list_element_singleton->impl;

	new_cells_info info{ret};

	list_impl->list_style.create_cells(items, impl, info);

	list_impl->insert_rows(IN_THREAD, ref{this}, item_number, info);

	return ret;
}

new_items_ret
listlayoutmanagerObj::replace_items(size_t item_number,
				    const std::vector<list_item_param>  &items)
{
	notmodified();

	new_items_ret ret;

	auto cells=in_thread_new_cells_info::create(ref{this}, items, ret);

	impl->run_as([impl=this->impl, item_number, cells]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();
			     me->modified=true;
			     me->impl->list_element_singleton->impl
				     ->replace_rows(IN_THREAD, me,
						    item_number,
						    cells->info);
		     });

	return ret;
}

new_items_ret listlayoutmanagerObj
::replace_items(ONLY IN_THREAD,
		size_t item_number, const std::vector<list_item_param> &items)
{
	modified=true;

	new_items_ret ret;

	auto list_impl=impl->list_element_singleton->impl;

	new_cells_info info{ret};

	list_impl->list_style.create_cells(items, impl, info);

	list_impl->replace_rows(IN_THREAD, ref{this}, item_number, info);

	return ret;
}

new_items_ret
listlayoutmanagerObj::replace_all_items(const
					std::vector<list_item_param> &items)
{
	notmodified();

	new_items_ret ret;

	auto cells=in_thread_new_cells_info::create(ref{this}, items, ret);

	impl->run_as([cells, impl=this->impl]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();
			     me->modified=true;
			     me->impl->list_element_singleton->impl
				     ->replace_all_rows(IN_THREAD, me,
							cells->info);
		     });

	return ret;
}

new_items_ret listlayoutmanagerObj
::replace_all_items(ONLY IN_THREAD,
		    const std::vector<list_item_param> &items)
{
	modified=true;

	new_items_ret ret;

	auto list_impl=impl->list_element_singleton->impl;

	new_cells_info info{ret};

	list_impl->list_style.create_cells(items, impl, info);

	list_impl->replace_all_rows(IN_THREAD, ref{this}, info);

	return ret;
}

void listlayoutmanagerObj::resort_items(const std::vector<size_t> &indexes)
{
	notmodified();

	impl->run_as([indexes, impl=this->impl]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();

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
	notmodified();
	return impl->list_element_singleton->impl->size();
}

bool listlayoutmanagerObj::selected(size_t i) const
{
	notmodified();
	return impl->list_element_singleton->impl->selected(i);
}

size_t listlayoutmanagerObj::hierindent(size_t i) const
{
	notmodified();
	return impl->list_element_singleton->impl->hierindent(i);
}

std::optional<size_t> listlayoutmanagerObj::selected() const
{
	notmodified();
	return impl->list_element_singleton->impl->selected();
}

std::vector<size_t> listlayoutmanagerObj::all_selected() const
{
	notmodified();
	return impl->list_element_singleton->impl->all_selected();
}

std::optional<size_t> listlayoutmanagerObj::current_list_item() const
{
	notmodified();
	auto lei=impl->list_element_singleton->impl;

	listimpl_info_t::lock lock{lei->textlist_info};

	return lei->current_element(lock);
}

void listlayoutmanagerObj::selected(size_t i, bool selected_flag)
{
	notmodified();

	impl->run_as([impl=this->impl, i, selected_flag]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();

			     me->selected(IN_THREAD, i, selected_flag, {});
		     });
}

void listlayoutmanagerObj::selected(ONLY IN_THREAD,
				    size_t i, bool selected_flag,
				    const callback_trigger_t &trigger)
{
	modified=true;
	impl->list_element_singleton->impl
		->selected(IN_THREAD, ref(this), i, selected_flag,
			   trigger);
}

void listlayoutmanagerObj::autoselect(size_t i)
{
	notmodified();

	impl->run_as([impl=this->impl,i]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();
			     me->autoselect(IN_THREAD, i, {});
		     });
}

void listlayoutmanagerObj::autoselect(ONLY IN_THREAD,
				      size_t i,
				      const callback_trigger_t &trigger)
{
	notmodified();

	impl->list_element_singleton->impl->autoselect(IN_THREAD,
						       ref(this), i, trigger);
}

void listlayoutmanagerObj::unselect()
{
	notmodified();

	impl->run_as([impl=this->impl]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();
			     me->unselect(IN_THREAD);
		     });
}

void listlayoutmanagerObj::unselect(ONLY IN_THREAD)
{
	notmodified();

	impl->list_element_singleton->impl->unselect(IN_THREAD, ref(this));
}

bool listlayoutmanagerObj::enabled(size_t i) const
{
	notmodified();
	return impl->list_element_singleton->impl->enabled(i);
}

void listlayoutmanagerObj::enabled(size_t i, bool flag)
{
	notmodified();
	impl->run_as([impl=this->impl, i, flag]
		     (ONLY IN_THREAD)
		     {
			     listlayoutmanager me=impl->create_public_object();
			     me->enabled(IN_THREAD, i, flag);
		     });
}

void listlayoutmanagerObj::enabled(ONLY IN_THREAD, size_t i, bool flag)
{
	notmodified();
	impl->list_element_singleton->impl->enabled(IN_THREAD, i, flag);
}

void listlayoutmanagerObj::on_status_update(size_t i,
					    const list_item_status_change_callback &cb)
{
	notmodified();
	impl->run_as([impl=this->impl, i, cb]
		     (ONLY IN_THREAD)
		     {
			     auto me=listlayoutmanager::create(impl);
			     me->on_status_update(IN_THREAD, i, cb);
		     });

}

void listlayoutmanagerObj::on_status_update(ONLY IN_THREAD,
					    size_t i,
					    const list_item_status_change_callback &cb)
{
	notmodified();
	impl->list_element_singleton->impl->on_status_update(IN_THREAD,
							     ref{this},
							     i,
							     cb);
}


void listlayoutmanagerObj::selection_type(const list_selection_type_cb_t
					  &selection_type)
{
	notmodified();
	listimpl_info_t::lock lock{impl->list_element_singleton->impl
				   ->textlist_info};

	lock->selection_type=selection_type;
}

void listlayoutmanagerObj::selection_type(ONLY IN_THREAD,
					  const list_selection_type_cb_t &s)
{
	notmodified();

	selection_type(s); // In case this changed in the future.
}

void listlayoutmanagerObj::on_list_selection_changed(const
						     list_selection_changed_cb_t
						     &selection_changed)
{
	notmodified();
	listimpl_info_t::lock lock{impl->list_element_singleton->impl
				   ->textlist_info};

	lock->selection_changed=selection_changed;
}

void listlayoutmanagerObj
::on_list_selection_changed(ONLY IN_THREAD,
			    const list_selection_changed_cb_t &s)
{
	notmodified();
	on_list_selection_changed(s); // In case this changed in the future.
}

void listlayoutmanagerObj::on_selection_changed(const
						list_selection_changed_cb_t
						&selection_changed)
{
	notmodified();

	on_list_selection_changed(selection_changed);
}

void listlayoutmanagerObj::on_selection_changed(ONLY IN_THREAD,
						const
						list_selection_changed_cb_t
						&selection_changed)
{
	notmodified();

	on_list_selection_changed(IN_THREAD, selection_changed);
}

void listlayoutmanagerObj
::on_current_list_item_changed(const list_item_status_change_callbackptr
			       &current_list_item_changed)
{
	notmodified();

	impl->run_as([impl=this->impl, current_list_item_changed]
		     (ONLY IN_THREAD)
		     {
			     auto me=listlayoutmanager::create(impl);

			     me->on_current_list_item_changed
				     (IN_THREAD,
				      current_list_item_changed);
		     });
}

void listlayoutmanagerObj
::on_current_list_item_changed(ONLY IN_THREAD,
			       const list_item_status_change_callbackptr
			       &current_list_item_changed)
{
	notmodified();

	impl->list_element_singleton->impl
		->current_list_item_changed(IN_THREAD)=
		current_list_item_changed;
}

listlayoutmanagerptr
listlayoutmanagerObj::submenu_listlayout(size_t i)
{
	notmodified();
	return impl->list_element_singleton->impl->submenu_listlayout(i);
}

LIBCXXW_NAMESPACE_END
