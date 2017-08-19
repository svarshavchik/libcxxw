/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menufactoryobj_h
#define x_w_menufactoryobj_h

#include <x/w/menufactoryfwd.H>
#include <x/w/menulayoutmanagerfwd.H>
#include <x/w/factoryobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menufactory

class menufactoryObj : public factoryObj {

public:
	//! Constructor
	menufactoryObj(const ref<containerObj::implObj> &container_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~menufactoryObj() LIBCXX_HIDDEN;

	//! Set the type of the next menu item.

	virtual void set_type(const menuitem_type_t &)=0;
};

LIBCXXW_NAMESPACE_END

#endif
