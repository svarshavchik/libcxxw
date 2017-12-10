/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler_resources.H"
#include "icon_images_set_element.H"
#include "background_color_element.H"
#include "screen.H"
#include "screen_depthinfo.H"

LIBCXXW_NAMESPACE_START

static background_color default_background_color(const screen &s,
						 const color_arg &color)
{
	return s->impl->create_background_color(color);
}

static inline generic_windowObj::handlerObj::constructor_params
create_constructor_params(const screen &parent_screen,
			  const color_arg &background_color,
			  size_t nesting_level)
{
	rectangle dimensions={0, 0, 1, 1};

	values_and_mask vm(XCB_CW_EVENT_MASK,
			   (uint32_t)
			   generic_windowObj::handlerObj::initial_event_mask(),
			   XCB_CW_COLORMAP,
			   parent_screen->impl->toplevelwindow_colormap->id(),
			   XCB_CW_BORDER_PIXEL,
			   parent_screen->impl->xcb_screen->black_pixel);

	return {
		{
			parent_screen,
			parent_screen->impl->xcb_screen->root, // parent
			parent_screen->impl->toplevelwindow_pictformat->depth, // depth
			dimensions, // initial_position
			XCB_WINDOW_CLASS_INPUT_OUTPUT, // window_class
			parent_screen->impl->toplevelwindow_visual->impl->visual_id, // visual
			vm, // events_and_mask
		},
		parent_screen->impl->toplevelwindow_pictformat,
		nesting_level,
		background_color
	};
}

generic_windowObj::handlerObj::resourcesObj
::resourcesObj(IN_THREAD_ONLY,
	       const screen &parent_screen,
	       const color_arg &background_color,
	       const shared_handler_data &handler_data,
	       size_t nesting_level)
	: generic_window_handler_and_resources_t
	  (make_function<icon()>
	   ([this]
	    {
		    return this->create_icon({"disabled_mask"});
	    }),
	   default_background_color(parent_screen, background_color),
	   default_background_color(parent_screen, "modal_shade"),

	   IN_THREAD, handler_data,
	   create_constructor_params(parent_screen, background_color,
				     nesting_level))
{
}

generic_windowObj::handlerObj::resourcesObj::~resourcesObj()=default;

const icon generic_windowObj::handlerObj::resourcesObj
::disabled_mask(IN_THREAD_ONLY)
{
	return icon_1tag<disabled_mask_tag>::tagged_icon(IN_THREAD);
}

void generic_windowObj::handlerObj::resourcesObj
::set_background_color(IN_THREAD_ONLY,
		       const background_color &c)
{
	background_color_element<background_color_tag>::update(IN_THREAD, c);
	background_color_changed(IN_THREAD);
}

const background_color generic_windowObj::handlerObj::resourcesObj
::current_background_color(IN_THREAD_ONLY)
{
	return background_color_element<background_color_tag>::get(IN_THREAD);
}
const background_color generic_windowObj::handlerObj::resourcesObj
::shaded_color(IN_THREAD_ONLY)
{
	return background_color_element<shaded_color_tag>::get(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
