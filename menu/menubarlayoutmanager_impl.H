/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menubarlayoutmanager_impl_h
#define menubarlayoutmanager_impl_h

#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactoryobj.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/gridfactoryfwd.H"
#include "gridlayoutmanager.H"
#include "menu/menubar_container_implfwd.H"
#include "popup/popupfwd.H"
#include "popup/popup_attachedto_handler.H"

#include <x/mpobj.H>
#include <x/functional.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN menubarlayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

 public:
	//! My container

	const ref<menubar_container_implObj> container_impl;

 private:
	struct protected_info_t {

		//! Spacer position

		//! Position of the spacer element between the left and the
		//! right sections.

		size_t divider_pos=0;

		//! Borders get drawn when there's at least one menu.
		bool borders_present=false;
	};

	protected_info_t protected_info;
 public:

	//! The protected_info can only be accessible while holding a lock.

	inline auto &info(const grid_map_t::lock &) { return protected_info; }

	//! Constructor
	implObj(const ref<menubar_container_implObj> &container_impl);

	//! Destructor
	~implObj();

	//! If borders_changed(), rebuild them.

	//! Invoked from the public object's destructor.

	void check_if_borders_changed();

	//! Create the menubarlayoutmanager object.

	layoutmanager create_public_object() override;

	//! Invoked after the container with the menubar is created.

	//! Insert the divider element().

	void initialize(menubarlayoutmanagerObj *public_object,
			menubar_lock &lock);

	//! Adding a new menu.

	menu add(menubarlayoutmanagerObj *public_object,
		 const gridfactory &factory,
		 const function<menubarfactoryObj::menu_creator_t> &creator,
		 const function<menubarfactoryObj::menu_content_creator_t>
		 &content_creator,
		 menubar_lock &lock);

	//! Fix tabbing order of a newly-added element.

	//! Keep the tabbing order of the menu bar elements to be the first
	//! ones tabbed into.

	void fix_order(IN_THREAD_ONLY, const menu &new_element);

	//! A menu item selection has been made.

	//! This is used as the selection_type called for the underlying
	//! list. This gets called when a menu item has been selected/clicked
	//! on.

	static void menuitem_selected(const listlayoutmanager &lm,
				      size_t i,
				      const callback_trigger_t &trigger,
				      const busy &mcguffin);

	//! Create a popup menu and its attachment handler.

	//! Shared code by the menubar layout manager, to create a new menu
	//! popup and also by the menu layout manager, to create a submenu
	//! popup.


	template<typename creator_t>
	static inline auto create_popup_menu(const elementimpl &e,
					     creator_t &&creator,
					     attached_to attached_to_how)
	{
		return do_create_popup_menu(e,
					    make_function<void
					    (const listlayoutmanager &)>
					    (std::forward<creator_t>(creator)),
					    attached_to_how);
	}

	//! Create a popup menu and its attachment handler.

	static std::tuple<popup, ref<popup_attachedto_handlerObj> >
		do_create_popup_menu(const elementimpl &e,
				     const function<void (const
							  listlayoutmanager &)>
				     &creator,
				     attached_to attached_to_how);

};

LIBCXXW_NAMESPACE_END

#endif
