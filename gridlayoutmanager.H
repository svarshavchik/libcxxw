/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_H
#define gridlayoutmanager_h

#include "x/w/gridlayoutmanager.H"
#include "layoutmanager.H"

LIBCXXW_NAMESPACE_START

//! The grid layout manager implementation object.

class LIBCXX_HIDDEN gridlayoutmanagerObj::implObj
	: public layoutmanagerObj::implObj {

 public:

	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl);

	//! Destructor
	~implObj();

	/////////////////////////////////////////////////////////////////////
	//
	// Overridden methods.

	void recalculate(IN_THREAD_ONLY) override;

	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void
			       (const child_element &e)> &callback) override;

	layoutmanager create_public_object() override;
};

LIBCXXW_NAMESPACE_END

#endif
