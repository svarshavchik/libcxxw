/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listitemfactoryobj.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "x/w/label.H"

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


//! The factory returned by append_item()

class LIBCXX_HIDDEN list_append_item_factoryObj : public listitemfactoryObj {
 public:

	using listitemfactoryObj::listitemfactoryObj;

	//! Forward create_item() to the append_item() implementation.

	void create_item(const listlayoutstyle::new_list_items_t &new_item)
		override
	{
		me->impl->append_item(me, new_item);
	}
};

factory listlayoutmanagerObj::append_item()
{
	return ref<list_append_item_factoryObj>
		::create(listlayoutmanager(this));
}

void listlayoutmanagerObj::append_item(const std::vector<text_param> &items)
{
	auto f=append_item();

	for (const auto &s:items)
		f->create_label(s);
}

//! The factory returned by insert_item()

class LIBCXX_HIDDEN list_insert_item_factoryObj : public listitemfactoryObj {
 public:

	size_t row_number;
	grid_map_t::lock lock;

	//! Constructor

	list_insert_item_factoryObj(const listlayoutmanager &l,
				    size_t row_number)
		: listitemfactoryObj(l),
		row_number(row_number),
		lock(l->impl->grid_map)
		{
		}

	//! Forward create_item() to the insert_item() implementation.

	void create_item(const listlayoutstyle::new_list_items_t &new_item)
		override
	{
		me->impl->insert_item(me, lock, new_item, row_number++);
	}
};

factory listlayoutmanagerObj::insert_item(size_t item_number)
{
	return ref<list_insert_item_factoryObj>
		::create(listlayoutmanager(this), item_number);
}

void listlayoutmanagerObj::insert_item(size_t item_number,
				       const std::vector<text_param> &items)
{
	auto f=insert_item(item_number);

	for (const auto &s:items)
		f->create_label(s);
}

//! The factory returned by replace_item()

class LIBCXX_HIDDEN list_replace_item_factoryObj : public listitemfactoryObj {
 public:

	size_t row_number;
	grid_map_t::lock lock;

	//! Constructor

	list_replace_item_factoryObj(const listlayoutmanager &l,
				     size_t row_number)
		: listitemfactoryObj(l),
		row_number(row_number),
		lock(l->impl->grid_map)
		{
		}

	//! Forward create_item() to the replace_item() implementation.

	void create_item(const listlayoutstyle::new_list_items_t &new_item)
		override
	{
		me->impl->replace_item(me, lock, new_item, row_number++);
	}
};

factory listlayoutmanagerObj::replace_item(size_t item_number)
{
	return ref<list_replace_item_factoryObj>
		::create(listlayoutmanager(this), item_number);
}

void listlayoutmanagerObj::replace_item(size_t item_number,
					const std::vector<text_param> &items)
{
	auto f=replace_item(item_number);

	for (const auto &s:items)
		f->create_label(s);
}
void listlayoutmanagerObj::remove_item(size_t item_number)
{
	grid_map_t::lock lock{impl->grid_map};

	impl->remove_item(listlayoutmanager(this), lock, item_number);
}

//! The factory returned by replace_all()

class LIBCXX_HIDDEN list_replace_all_factoryObj
	: public list_append_item_factoryObj {

	grid_map_t::lock lock;

 public:

	list_replace_all_factoryObj(const listlayoutmanager &l)
		: list_append_item_factoryObj(l),
		lock{l->impl->grid_map}
	{
		l->impl->remove_all_items(me);
	}

	~list_replace_all_factoryObj() = default;
};

factory listlayoutmanagerObj::replace_all()
{
	return ref<list_replace_all_factoryObj>
		::create(listlayoutmanager(this));
}

void listlayoutmanagerObj::replace_all(const std::vector<text_param> &items)
{
	auto f=replace_all();

	for (const auto &item:items)
		f->create_label(item);
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

LIBCXXW_NAMESPACE_END
