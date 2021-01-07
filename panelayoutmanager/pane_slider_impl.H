/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_slider_impl_h
#define pane_slider_impl_h

#include "x/w/focusableobj.H"
#include "x/w/elementobj.H"
#include "panelayoutmanager/pane_sliderobj.H"

LIBCXXW_NAMESPACE_START

//! Pane slider element interface mixin.

//! The pane_slider_impl_elementObj template combines this mixin with a
//! display element.

class LIBCXX_HIDDEN pane_sliderObj::implObj : virtual public obj {

public:

	//! Constructor
	implObj();

	//! Destructor

	~implObj();

	//! Return my element implementation object.

	virtual elementObj::implObj &slider_element_impl()=0;

	//! Return my focusable implementation object.

	virtual focusableObj::implObj &slider_focusable_impl()=0;
};

LIBCXXW_NAMESPACE_END

#endif
