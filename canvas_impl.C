/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "canvas.H"
#include "themedim_axis_element.H"
#include "connection_thread.H"
#include "xid_t.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

canvasObj::implObj::implObj(const ref<containerObj::implObj> &container,
			    const dim_axis_arg &horiz,
			    const dim_axis_arg &vert)
	: superclass_t(horiz, vert,
		       container, child_element_init_params
		       {"background@libcxx.com"})
{
	// It's ok to break the rules here, there cannot be any multithreading
	// at this point, the object isn't even constructed. This way, the
	// implementaiton object already has the translated metrics right from
	// the start.

	ONLY IN_THREAD=THREAD;

	auto hv=get_horizvert(IN_THREAD);

	hv->horiz=get_width_axis(IN_THREAD);
	hv->vert=get_height_axis(IN_THREAD);
}

canvasObj::implObj::~implObj()=default;

void canvasObj::implObj::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	recalculate(IN_THREAD);
}

void canvasObj::implObj::theme_updated(ONLY IN_THREAD,
				       const defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	recalculate(IN_THREAD);
}

void canvasObj::implObj::recalculate(ONLY IN_THREAD)
{
	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 get_width_axis(IN_THREAD),
		 get_height_axis(IN_THREAD));
}


LIBCXXW_NAMESPACE_END
