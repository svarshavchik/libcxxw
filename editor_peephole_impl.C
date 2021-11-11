/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/container_element.H"
#include "editor_peephole_impl.H"
#include "editor.H"
#include "editor_impl.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "drag_destination_element.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/impl/fonts/fontcollection.H"
#include "x/w/impl/container_element.H"
#include "x/w/button_event.H"

LIBCXXW_NAMESPACE_START

editor_peephole_implObj::~editor_peephole_implObj()=default;

void editor_peephole_implObj::recalculate(ONLY IN_THREAD,
					  editor_implObj &e)
{
	auto [width, height]=e.nominal_size(IN_THREAD);

	if (width == dim_t::infinite())
		--width;

	if (height == dim_t::infinite())
		--height;

	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 {width, width, width},
		 {height, height, height});
}

void editor_peephole_implObj::report_motion_event(ONLY IN_THREAD,
						  const motion_event &me)
{
	// If we're hiding the pointer, remove it.
	remove_cursor_pointer(IN_THREAD);
        superclass_t::report_motion_event(IN_THREAD, me);
}

LIBCXXW_NAMESPACE_END
