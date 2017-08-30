/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listitemfactoryobj.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "x/w/label.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

list_lock::list_lock(const const_listlayoutmanager &manager)
	: list_lock(manager->impl->grid_map)
{
}

list_lock::list_lock(grid_map_t &map) : grid_map_t::lock{map}
{
}

list_lock::~list_lock()=default;

listlayoutmanagerObj::listlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl),
	  impl(impl)
{
}

listlayoutmanagerObj::~listlayoutmanagerObj()=default;

void listlayoutmanagerObj::remove_callback_factory()
{
	callback_factory_container_t::lock lock{callback_factory_container};

	*lock=nullptr;
}

factory listlayoutmanagerObj::append_item()
{
	return ref<implObj::append_factoryObj<factoryObj, listitemfactoryObj>>
		::create(listlayoutmanager(this),
			 impl->container_impl);
}

void listlayoutmanagerObj::append_item(const std::vector<text_param> &items)
{
	auto f=append_item();

	for (const auto &s:items)
		append_text_or_separator(f, s);
}

void listlayoutmanagerObj::append_text_or_separator(const factory &f,
						    const text_param &t)
{
	if (t.string.empty())
	{
		impl->style.create_separator(impl,
					     impl->append_row(this),
					     queue);
		return;
	}
	f->create_label(t);
}

factory listlayoutmanagerObj::insert_item(size_t item_number)
{
	return ref<implObj::insert_factoryObj<factoryObj, listitemfactoryObj>>
		::create(listlayoutmanager(this), item_number,
			 impl->container_impl);
}

void listlayoutmanagerObj::insert_item(size_t item_number,
				       const std::vector<text_param> &items)
{
	auto f=insert_item(item_number);

	for (const auto &s:items)
		insert_text_or_separator(item_number++, f, s);
}

void listlayoutmanagerObj::insert_text_or_separator(size_t item_number,
						    const factory &f,
						    const text_param &t)
{
	if (t.string.empty())
	{
		impl->style.create_separator(impl,
					     impl->insert_row(this,
							      item_number),
					     queue);
		return;
	}
	f->create_label(t);
}

factory listlayoutmanagerObj::replace_item(size_t item_number)
{
	return ref<implObj::replace_factoryObj<factoryObj, listitemfactoryObj>>
		::create(listlayoutmanager(this), item_number,
			 impl->container_impl);
}

void listlayoutmanagerObj::replace_item(size_t item_number,
					const std::vector<text_param> &items)
{
	auto f=replace_item(item_number);

	for (const auto &s:items)
		replace_text_or_separator(item_number, f, s);
}

void listlayoutmanagerObj::replace_text_or_separator(size_t item_number,
						     const factory &f,
						     const text_param &t)
{
	if (t.string.empty())
	{
		impl->style.create_separator(impl,
					     impl->replace_row(this,
							       item_number),
					     queue);
		return;
	}
	f->create_label(t);
}

void listlayoutmanagerObj::remove_item(size_t item_number)
{
	grid_map_t::lock lock{impl->grid_map};

	selected(lock, item_number, false);

	impl->remove_item(listlayoutmanager(this), lock, item_number);
}

factory listlayoutmanagerObj::replace_all_items()
{
	return ref<implObj::replace_all_factoryObj<factoryObj,
						   listitemfactoryObj>>
		::create(listlayoutmanager(this), impl->container_impl);
}

void listlayoutmanagerObj::replace_all_items(const std::vector<text_param>
					     &items)
{
	auto f=replace_all_items();

	for (const auto &item:items)
		append_text_or_separator(f, item);
}
////////////////////////////////////////////////////////////////////////////

size_t listlayoutmanagerObj::size() const
{
	grid_map_t::lock lock{impl->grid_map};

	return size(lock);
}

size_t listlayoutmanagerObj::size(grid_map_t::lock &lock) const
{
	return impl->size(lock);
}

bool listlayoutmanagerObj::selected(size_t i) const
{
	grid_map_t::lock lock{impl->grid_map};

	return selected(lock, i);
}

bool listlayoutmanagerObj::selected(grid_map_t::lock &lock, size_t i) const
{
	return impl->selected(lock, i);
}

std::optional<size_t> listlayoutmanagerObj::selected() const
{
	auto v=all_selected();

	if (v.empty())
		return {};

	return { v.at(0) };
}

std::vector<size_t> listlayoutmanagerObj::all_selected() const
{
	std::vector<size_t> v;

	grid_map_t::lock lock{impl->grid_map};

	size_t n=size(lock);

	for (size_t i=0; i<n; ++i)
		if (selected(lock, i))
			v.push_back(i);

	return v;
}

void listlayoutmanagerObj::unselect()
{
	grid_map_t::lock lock{impl->grid_map};

	size_t n=size(lock);

	for (size_t i=0; i<n; ++i)
		if (selected(lock, i))
			selected(lock, i, false);
}

void listlayoutmanagerObj::selected(size_t i, bool selected_flag)
{
	grid_map_t::lock lock{impl->grid_map};

	selected(lock, i, selected_flag);
}

void listlayoutmanagerObj::selected(grid_map_t::lock &lock, size_t i,
				    bool selected_flag)
{
	return impl->selected(listlayoutmanager(this),
			      lock, i, selected_flag);
}

void listlayoutmanagerObj::autoselect(size_t i)
{
	grid_map_t::lock lock{impl->grid_map};

	autoselect(lock, i);
}

void listlayoutmanagerObj::autoselect(grid_map_t::lock &lock, size_t i)
{
	return impl->autoselect(listlayoutmanager(this), lock, i);
}

bool listlayoutmanagerObj::enabled(size_t i) const
{
	grid_map_t::lock lock{impl->grid_map};

	return impl->enabled(lock, i);
}

void listlayoutmanagerObj::enabled(size_t i, bool flag)
{
	grid_map_t::lock lock{impl->grid_map};

	auto e=impl->get(i, 0);

	if (!e)
		return;

	impl->container_impl->get_element_impl().THREAD->run_as
		([e, me=listlayoutmanager(this), flag]
		 (IN_THREAD_ONLY)
		 {
			 grid_map_t::lock lock{me->impl->grid_map};

			 auto looked_up=me->impl->lookup_row_col(e->impl);

			 if (!looked_up)
				 return;

			 size_t row;

			 std::tie(row, std::ignore)=looked_up.value();

			 me->impl->enabled(IN_THREAD, lock, row, flag);
		 });
}

element listlayoutmanagerObj::item(size_t item_number, size_t column)
{
	return impl->item(item_number, column);
}

LIBCXXW_NAMESPACE_END
