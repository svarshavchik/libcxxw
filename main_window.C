/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window.H"
#include "main_window_handler.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "x/w/picture.H"
#include "x/w/screen.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::main_windowObj);

LIBCXXW_NAMESPACE_START

main_windowObj::main_windowObj(const ref<implObj> &impl)
	: generic_windowObj(impl),
	  impl(impl)
{
}

main_windowObj::~main_windowObj()=default;

void main_windowObj::on_delete(const std::function<void ()> &callback)
{
	impl->on_delete(callback);
}

//////////////////////////////////////////////////////////////////////////////
//
// Create a default main window

main_window main_windowBase::create()
{
	return screen::base::create()->create_mainwindow();
}

main_window screenObj::create_mainwindow()
{
	auto background_color=create_solid_color_picture
		(rgb(0xCCCC, 0xCCCC, 0xCCCC));

	rectangle dimensions={0, 0, 100, 100};

	values_and_mask vm(XCB_CW_EVENT_MASK,
			   (uint32_t)
			   main_windowObj::handlerObj::initial_event_mask(),
			   XCB_CW_COLORMAP,
			   impl->toplevelwindow_colormap->id(),
			   XCB_CW_BORDER_PIXEL,
			   impl->xcb_screen->black_pixel);

	main_windowObj::handlerObj::constructor_params params{
		{
			impl->xcb_screen->root, // parent
			impl->toplevelwindow_pictformat->depth, // depth
			dimensions, // initial_position
			XCB_WINDOW_CLASS_INPUT_OUTPUT, // window_class
			impl->toplevelwindow_visual->impl->visual_id, // visual
			vm, // events_and_mask
		},

		impl->toplevelwindow_pictformat,
		[background_color]
		{
			return background_color;
		}
	};

	auto handler=ref<main_windowObj::handlerObj>
		::create(connref->impl->thread, params);

	auto window_impl=ref<main_windowObj::implObj>
		::create(screen(this),
			 handler, dimensions);

	return ptrrefBase::objfactory<main_window>::create(window_impl);
}

LIBCXXW_NAMESPACE_END
