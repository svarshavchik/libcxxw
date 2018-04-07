/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef container_impl_H
#define container_impl_H

#include "x/w/impl/container.H"
#include "x/w/impl/element.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/impl/draw_infofwd.H"
#include "x/w/rectangle.H"
#include "x/w/layoutmanagerobj.H"

LIBCXXW_NAMESPACE_START

void container_clear_padding(ONLY IN_THREAD,
			     elementObj::implObj &container_element_impl,
			     layoutmanagerObj::implObj &manager,
			     const ref<elementObj::implObj> &e_impl,
			     const draw_info &di,
			     clip_region_set &clip,
			     rectangle_set &child_areas) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
