/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframecontainerobj_h
#define focusframecontainerobj_h

#include "focus/focusframecontainerfwd.H"
#include "x/w/containerobj.H"
#include "x/w/focusableownerobj.H"
#include "x/w/factoryfwd.H"
#include "x/w/hotspot_bgcolorobj.H"

LIBCXXW_NAMESPACE_START

//! A focus frame container.

//! \see focusframecontainer

class LIBCXX_HIDDEN focusframecontainerObj
	: public containerObj,
	  public focusableObj::ownerObj {

public:
	class implObj;

	struct new_focusframelayoutmanager;

	//! Internal implementation object.

	const ref<implObj> impl;

	//! Constructor
	focusframecontainerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Internal constructor

	focusframecontainerObj(const ref<implObj> &impl,
			       const new_focusframelayoutmanager &factory)
		LIBCXX_HIDDEN;
	//! Destructor
	~focusframecontainerObj() LIBCXX_HIDDEN;

	//! Replace the display element.
	factory set_focusable();
};

LIBCXXW_NAMESPACE_END

#endif
