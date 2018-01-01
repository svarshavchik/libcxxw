/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_sliderobj_h
#define pane_sliderobj_h

#include "panelayoutmanager/pane_sliderfwd.H"
#include "x/w/focusableobj.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pane_slider

class LIBCXX_HIDDEN pane_sliderObj : public elementObj,
				     public focusableObj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	pane_sliderObj(const ref<implObj> &impl);

	//! Destructor
	~pane_sliderObj();

	//! Implement get_impl()
	ref<focusableImplObj> get_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
