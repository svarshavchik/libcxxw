/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_borderlayoutmanagerobj_h
#define x_w_borderlayoutmanagerobj_h

#include <x/w/borderlayoutmanagerfwd.H>
#include <x/w/singletonlayoutmanagerobj.H>
#include <x/w/text_paramfwd.H>
#include <x/w/connection_threadfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see borderlayoutmanager

class borderlayoutmanagerObj : public singletonlayoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	borderlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~borderlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Update the title.

	void update_title(const text_param &);

	//! Update the title

	void update_title(ONLY IN_THREAD, const text_param &);
};

LIBCXXW_NAMESPACE_END

#endif
