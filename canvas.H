/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef canvas_h
#define canvas_h

#include "x/w/canvasobj.H"
#include "x/w/screenobj.H"
#include "x/w/dim_axis_arg.H"
#include "connection_threadfwd.H"
#include "child_elementobj.H"
#include "themedim_axis_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Base class for a canvas implementation.

class LIBCXX_HIDDEN canvasObj::implObj
	: public themedim_axis_widthObj<
	themedim_axis_heightObj<child_elementObj>> {

	//! Alias.

	typedef themedim_axis_widthObj<
		themedim_axis_heightObj<child_elementObj>> superclass_t;

public:
	//! Constructor
	implObj(const ref<containerObj::implObj> &container,
		const dim_axis_arg &width,
		const dim_axis_arg &height);

	//! Destructor
	~implObj();

	//! Inherit from elementObj::implObj
	void initialize(IN_THREAD_ONLY) override;

	//! Inherit from elementObj::implObj
	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;

 private:
	//! Recalculate my metrics
	void recalculate(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
