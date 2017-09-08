/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container_element.H"
#include "editor_peephole_impl.H"
#include "peephole/peephole_get_element.H"
#include "editor.H"
#include "editor_impl.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "fonts/current_fontcollection.H"
#include "fonts/fontcollection.H"
#include "x/w/button_event.H"

LIBCXXW_NAMESPACE_START

editor_peephole_implObj::~editor_peephole_implObj()=default;

void editor_peephole_implObj::recalculate(IN_THREAD_ONLY,
					  editorObj::implObj &e)
{
	dim_t height=e.nominal_height(IN_THREAD);
	dim_t width=e.nominal_width(IN_THREAD);

	if (width == dim_t::infinite())
		--width;

	if (height == dim_t::infinite())
		--height;

	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 {width, width, width},
		 {height, height, height});
}

bool editor_peephole_implObj::process_button_event(IN_THREAD_ONLY,
						   const button_event &be,
						   xcb_timestamp_t
						   timestamp)
{
	if ((be.button != 1 && be.button != 2) || !be.press)
		return false;

	get_element([&]
		    (const focusable &f)
		    {
			    f->get_impl()->set_focus_only(IN_THREAD);
		    });
	return true;
}

void editor_peephole_implObj::report_motion_event(IN_THREAD_ONLY,
						  const motion_event &me)
{
	// If we're hiding the pointer, remove it.
	remove_cursor_pointer(IN_THREAD);
	peepholeObj::implObj::report_motion_event(IN_THREAD, me);
}

LIBCXXW_NAMESPACE_END
