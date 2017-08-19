/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "menu/menulayoutmanager_impl.H"
#include "menu/menulistitemfactoryobj.H"
#include "x/w/menufactory.H"
#include "x/w/label.H"

LIBCXXW_NAMESPACE_START

menulayoutmanagerObj::menulayoutmanagerObj(const ref<implObj> &impl)
	: listlayoutmanagerObj(impl),
	  impl(impl)
{
}

menulayoutmanagerObj::~menulayoutmanagerObj()=default;

// Implement set_type(), inherited from menufactoryObj, and forward it to
// menulistitemfactoryObj.

template<typename factory_t>
class LIBCXX_HIDDEN menulayoutfactoryObj : public factory_t {

 public:

	using factory_t::factory_t;

	void set_type(const menuitem_type_t &t) override
	{
		menulistitemfactoryObj::set_type(t);
	}
};

// Creating plain label menu items eventually boils down to a
// menufactory and a vector of new menu items and their types.

static void create_menu_items(const menufactory &f,
			      const menulayoutmanagerObj::text_items_t &v)
{
	for (const auto &s:v)
	{
		f->set_type(std::get<menuitem_type_t>(s));
		f->create_label(std::get<text_param>(s));
	}
}

factory menulayoutmanagerObj::append_item()
{
	return append_menu_item();
}

void menulayoutmanagerObj::append_item(const std::vector<text_param> &items)
{
	append_menu_item(items);
}

menufactory menulayoutmanagerObj::append_menu_item()
{
	return ref<menulayoutfactoryObj<
		implObj::append_factoryObj<menufactoryObj,
					   menulistitemfactoryObj>>>
		::create(listlayoutmanager(this),
			 impl->container_impl);
}

void menulayoutmanagerObj::do_append_menu_item(const text_items_t &v)
{
	create_menu_items(append_menu_item(), v);
}

factory menulayoutmanagerObj::insert_item(size_t item_number)
{
	return insert_menu_item(item_number);
}

void menulayoutmanagerObj::insert_item(size_t item_number,
				       const std::vector<text_param> &items)
{
	insert_menu_item(item_number, items);
}

menufactory menulayoutmanagerObj::insert_menu_item(size_t item_number)
{
	return ref<menulayoutfactoryObj<
		implObj::insert_factoryObj<menufactoryObj,
					   menulistitemfactoryObj>>>
		::create(listlayoutmanager(this), item_number,
			 impl->container_impl);
}

void menulayoutmanagerObj::do_insert_menu_item(size_t item_number,
					       const text_items_t &v)
{
	create_menu_items(insert_menu_item(item_number), v);
}

factory menulayoutmanagerObj::replace_item(size_t item_number)
{
	return replace_menu_item(item_number);
}

void menulayoutmanagerObj::replace_item(size_t item_number,
					const std::vector<text_param> &items)
{
	replace_menu_item(item_number, items);
}

menufactory menulayoutmanagerObj::replace_menu_item(size_t item_number)
{
	return ref<menulayoutfactoryObj<
		implObj::replace_factoryObj<menufactoryObj,
					    menulistitemfactoryObj>>>
		::create(listlayoutmanager(this), item_number,
			 impl->container_impl);
}

void menulayoutmanagerObj::do_replace_menu_item(size_t item_number,
						const text_items_t &v)
{
	create_menu_items(replace_menu_item(item_number), v);
}


factory menulayoutmanagerObj::replace_all_items()
{
	return replace_all_menu_items();
}

void menulayoutmanagerObj::replace_all_items(const std::vector<text_param>
					     &items)
{
	replace_all_menu_items(items);
}

menufactory menulayoutmanagerObj::replace_all_menu_items()
{
	return ref<menulayoutfactoryObj
		   <implObj::replace_all_factoryObj<menufactoryObj,
						    menulistitemfactoryObj>>>
		::create(listlayoutmanager(this), impl->container_impl);
}

void menulayoutmanagerObj::do_replace_all_menu_items(const text_items_t &v)
{
	create_menu_items(replace_all_menu_items(), v);
}

LIBCXXW_NAMESPACE_END
