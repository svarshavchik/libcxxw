/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menuitemextrainfoobj_h
#define menuitemextrainfoobj_h

#include "activated_in_threadobj.H"
#include "menu/menuitemextrainfofwd.H"
#include "x/w/containerobj.H"
#include "x/w/elementfwd.H"
#include "x/w/menulayoutmanagerfwd.H"

#include <x/mp.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menuitemextrainfo

class LIBCXX_HIDDEN menuitemextrainfoObj : public containerObj,
					   public activated_in_threadObj {

public:
	class implObj;

	//! Implementation object.

	const ref<implObj> impl;

	//! The type of this menu item.

	mp<created_menuitem_type_t> menuitem_type;

	//! Constructor
	menuitemextrainfoObj(const ref<implObj> &impl,
			     const menuitem_type_t &menuitem_type);

 private:
	menuitemextrainfoObj(const ref<implObj> &impl,
			     const menuitem_type_t &menuitem_type,
			     const created_menuitem_type_t
			     &created_menuitem_type);
 public:
	//! Destructor
	~menuitemextrainfoObj();

	//! After construction, update the installed shortcut.

	//! After construction this must be called to install the initial
	//! shortcut, if any. update() takes care of invoking it for updates.

	void update_shortcut(const menuitem_type_t &new_type);

	//! Update the extra info, and element.

	void update(const menuitem_type_t &new_type);

	//! Inherited from activated_in_threadObj

	//! We expect our parent container element to be the list layout
	//! layout manager.

	void activated(IN_THREAD_ONLY) override;

	//! Inherited from activated_in_threadObj

	//! We handle it here, and report us as enabled based on
	//! the element's enabled flag only. We are not going to
	//! forward this to child_elementObj, which will then kick
	//! it upstairs, and when the menu popup is closed it's considered
	//! to be disabled. Obviously we want the shortcut to be active
	//! even when the menu is not open.

	bool enabled(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
