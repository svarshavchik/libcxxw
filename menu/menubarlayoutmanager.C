/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "menu/menu_impl.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "menu/menubar_container_impl.H"
#include "messages.H"
#include "x/w/label.H"
#include "generic_window_handler.H"
#include "grid_map_info.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

size_t menubarlayoutmanagerObj::menus() const
{
	notmodified();
	return impl->info(grid_lock).divider_pos;
}

size_t menubarlayoutmanagerObj::right_menus() const
{
	notmodified();
	return (*grid_lock)->cols(0)-menus()-1;
}

menu menubarlayoutmanagerObj::get_menu(size_t n) const
{
	notmodified();
	if (n >= menus())
		throw EXCEPTION(_("Menu does not exist"));

	return (*grid_lock)->get(0, n);
}

menu menubarlayoutmanagerObj::get_right_menu(size_t n) const
{
	if (n >= right_menus())
		throw EXCEPTION(_("Menu does not exist"));

	return (*grid_lock)->get(0, menus()+1+n);
}

///////////////////////////////////////////////////////////////////////////

menubarlayoutmanagerObj::menubarlayoutmanagerObj(const ref<implObj> &impl)
	: gridlayoutmanagerObj{impl}, impl{impl}
{
}

menubarlayoutmanagerObj::~menubarlayoutmanagerObj()
{
	impl->check_if_borders_changed();
}

// Implement menubarfactoryObj::implObj.
//
// A template that implements do_add() by invoking a lambda.

template<typename add_impl_t>
class LIBCXX_HIDDEN menubarfactory_implObj : public menubarfactoryObj {

 public:

	menubarfactory_implObj(const menubarlayoutmanager &layout,
			       add_impl_t &&impl)
		: menubarfactoryObj(layout),
		impl(std::move(impl))
		{
		}

	~menubarfactory_implObj()=default;

	add_impl_t impl;

	menu do_add_impl(const function<menu_creator_t> &callback,
			 const function<menu_content_creator_t>
			 &content_callback,
			 const shortcut &sc,
			 const const_popup_list_appearance &appearance)
		override
	{
		layout->set_modified();
		return impl(this->layout, callback, content_callback,
			    sc,
			    appearance);
	}
};

template<typename add_impl_t>
static auto create_menubarfactory(const menubarlayoutmanager &layout,
				  add_impl_t &&arg)
{
	return ref<menubarfactory_implObj
		   <typename std::remove_reference<add_impl_t>
		    ::type>>::create(layout,
				     std::forward<add_impl_t>(arg));

}

menubarfactory menubarlayoutmanagerObj::append_menus()
{
	return create_menubarfactory
		(menubarlayoutmanager(this),
		 []
		 (const menubarlayoutmanager &lm,
		  const auto &creator,
		  const auto &content_creator,
		  const shortcut &sc,
		  const const_popup_list_appearance &appearance)
		 {
			 auto mb=lm->impl->add(&*lm,
					       lm->impl->insert_columns
					       (&*lm, 0, lm->impl->info
						(lm->grid_lock)
						.divider_pos),
					       creator,
					       content_creator,
					       appearance,
					       sc);

			 ++lm->impl->info(lm->grid_lock).divider_pos;
			 return mb;
		 });
}

menubarfactory menubarlayoutmanagerObj::insert_menus(size_t pos)
{
	return create_menubarfactory
		(menubarlayoutmanager(this),
		 [pos]
		 (const menubarlayoutmanager &lm,
		  const auto &creator,
		  const auto &content_creator,
		  const shortcut &sc,
		  const const_popup_list_appearance &appearance)
		 mutable
		 {
			 if (pos > lm->impl->info(lm->grid_lock)
			     .divider_pos)
				 throw EXCEPTION(_("Existing menu does not exist."));

			 auto mb=lm->impl->add(&*lm,
					       lm->impl->insert_columns(&*lm,
									0, pos),
					       creator, content_creator,
					       appearance, sc);

			 ++lm->impl->info(lm->grid_lock).divider_pos;
			 ++pos;

			 return mb;
		 });
}

menubarfactory menubarlayoutmanagerObj::append_right_menus()
{
	return create_menubarfactory
		(menubarlayoutmanager(this),
		 []
		 (const menubarlayoutmanager &lm,
		  const auto &creator,
		  const auto &content_creator,
		  const shortcut &sc,
		  const const_popup_list_appearance &appearance)
		 {
			 return lm->impl->add(&*lm,
					      lm->impl->append_columns(&*lm,
								       0),
					      creator, content_creator,
					      appearance, sc);
		 });
}

menubarfactory menubarlayoutmanagerObj::insert_right_menus(size_t pos)
{
	return create_menubarfactory
		(menubarlayoutmanager(this),
		 [pos]
		 (const menubarlayoutmanager &lm,
		  const auto &creator,
		  const auto &content_creator,
		  const shortcut &sc,
		  const const_popup_list_appearance &appearance)
		 mutable
		 {
			 if (pos > lm->cols(0)-lm->impl->info
			     (lm->grid_lock).divider_pos)
				 throw EXCEPTION(_("Existing menu does not exist."));

			 auto mb=lm->impl->add(&*lm,
					       lm->impl->insert_columns
					       (&*lm, 0,
						lm->impl->info(lm->grid_lock)
						.divider_pos+1+pos),
					       creator, content_creator,
					       appearance,
					       sc);
			 ++pos;
			 return mb;
		 });
}


void menubarlayoutmanagerObj::remove_menu(size_t pos)
{
	if (menus() <= pos)
		throw EXCEPTION(gettextmsg(_("Menu #%1% does not exist"),
					   pos));

	modified=true;
	impl->remove(0, pos);

	--impl->info(grid_lock).divider_pos;
}

void menubarlayoutmanagerObj::remove_right_menu(size_t pos)
{
	if (right_menus() <= pos)
		throw EXCEPTION(gettextmsg(_("Menu #%1% does not exist"),
					   pos));

	modified=true;
	impl->remove(0,
		     impl->info(grid_lock)
		     .divider_pos+1+pos);
}

LIBCXXW_NAMESPACE_END
