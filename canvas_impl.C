/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "canvas.H"
#include "element_screen.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

canvasObj::implObj::implObj(const ref<containerObj::implObj> &container,
			    const metrics::mmaxis &horiz_metrics,
			    const metrics::mmaxis &vert_metrics,
			    metrics::halign horizontal_alignment,
			    metrics::valign vertical_alignment)
	: implObj(container, container->get_element_impl().get_screen()->impl,
		  horiz_metrics,
		  vert_metrics,
		  horizontal_alignment,
		  vertical_alignment)
{
}

canvasObj::implObj::implObj(const ref<containerObj::implObj> &container,
			    const ref<screenObj::implObj> &screen,
			    const metrics::mmaxis &horiz_metrics,
			    const metrics::mmaxis &vert_metrics,
			    metrics::halign horizontal_alignment,
			    metrics::valign vertical_alignment)
	: child_elementObj(container, {
			screen->compute_width(horiz_metrics),
				screen->compute_height(horiz_metrics),
				horizontal_alignment,
				vertical_alignment
				},
		"background@libcxx"),
	  horiz_metrics_thread_only(horiz_metrics),
	  vert_metrics_thread_only(vert_metrics)
{
}

canvasObj::implObj::~implObj()=default;

void canvasObj::implObj::initialize(IN_THREAD_ONLY)
{
	recalculate(IN_THREAD);
	child_elementObj::initialize(IN_THREAD);
}

void canvasObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	recalculate(IN_THREAD);
	child_elementObj::theme_updated(IN_THREAD);
}

void canvasObj::implObj::recalculate(IN_THREAD_ONLY)
{
	auto screen=container->get_element_impl().get_screen()->impl;

	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 screen->compute_width(horiz_metrics(IN_THREAD)),
		 screen->compute_height(vert_metrics(IN_THREAD))
		 );
}


LIBCXXW_NAMESPACE_END
