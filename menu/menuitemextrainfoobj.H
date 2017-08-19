/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menuitemextrainfoobj_h
#define menuitemextrainfoobj_h

#include "menu/menuitemextrainfofwd.H"
#include "x/w/containerobj.H"
#include "x/w/elementfwd.H"
#include "x/w/menulayoutmanagerfwd.H"

#include <x/mp.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menuitemextrainfo

class menuitemextrainfoObj : public containerObj {

public:
	class implObj;

	//! Implementation object.

	const ref<implObj> impl;

	//! The type of this menu item.

	mp<menuitem_type_t> menuitem_type;

	//! Constructor
	menuitemextrainfoObj(const ref<implObj> &impl,
			     const menuitem_type_t &menuitem_type)
		LIBCXX_HIDDEN;

	//! Destructor
	~menuitemextrainfoObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
