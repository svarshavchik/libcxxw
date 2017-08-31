/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "popup/popup.H"
#include "menu/menulayoutmanager_impl.H"
#include "menu/menulistitemfactoryobj.H"
#include "menu/menuitemextrainfo.H"
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
	auto f=append_menu_item();

	for (const auto &s:v)
	{
		f->set_type(std::get<menuitem_type_t>(s));
		append_text_or_separator(f, std::get<text_param>(s));
	}
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
	auto f=insert_menu_item(item_number);

	for (const auto &s:v)
	{
		f->set_type(std::get<menuitem_type_t>(s));
		insert_text_or_separator(item_number++,
					 f,
					 std::get<text_param>(s));
	}
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
	auto f=replace_menu_item(item_number);

	for (const auto &s:v)
	{
		f->set_type(std::get<menuitem_type_t>(s));

		replace_text_or_separator(item_number++, f,
					  std::get<text_param>(s));
	}
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
	auto f=replace_all_menu_items();

	for (const auto &s:v)
	{
		f->set_type(std::get<menuitem_type_t>(s));
		append_text_or_separator(f, std::get<text_param>(s));
	}
}

void menulayoutmanagerObj::update(size_t item_number,
				  const menuitem_type_t &new_type)
{
	auto extrainfo=implObj::get_extrainfo(listlayoutmanager(this),
					      item_number);

	if (extrainfo)
		extrainfo->update(new_type);
}

menulayoutmanagerptr menulayoutmanagerObj::get_item_layoutmanager(size_t i)
{
	menulayoutmanagerptr l;

	auto extrainfo=impl->get_extrainfo(listlayoutmanager(this), i);

	if (extrainfo)
	{
		auto t=extrainfo->submenu();

		if (t)
			l=t->get_layoutmanager();
	}

	return l;
}

LIBCXXW_NAMESPACE_END
