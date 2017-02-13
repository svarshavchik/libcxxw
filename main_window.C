/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window.H"
#include "main_window_handler.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "connection_thread.H"
#include "batch_queue.H"
#include "x/w/picture.H"
#include "x/w/new_layoutmanager.H"
#include "x/w/screen.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::main_windowObj);

LIBCXXW_NAMESPACE_START

main_windowObj::main_windowObj(const ref<implObj> &impl,
			       const new_layoutmanager &layout_factory)
	: generic_windowObj(impl, layout_factory),
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

main_window main_windowBase::do_create(const function<main_window_creator_t> &f)
{
	return screen::base::create()->do_create_mainwindow(f);
}

main_window main_windowBase::do_create(const new_layoutmanager &factory,
				       const function<main_window_creator_t> &f)
{
	return screen::base::create()->do_create_mainwindow(factory, f);
}

main_window screenObj
::do_create_mainwindow(const function<main_window_creator_t> &f)
{
	return do_create_mainwindow(new_layoutmanager::base::create_grid(), f);
}

main_window screenObj
::do_create_mainwindow(const new_layoutmanager &layout_factory,
		       const function<main_window_creator_t> &f)
{
	rectangle dimensions={0, 0, 1, 1};

	values_and_mask vm(XCB_CW_EVENT_MASK,
			   (uint32_t)
			   main_windowObj::handlerObj::initial_event_mask(),
			   XCB_CW_COLORMAP,
			   impl->toplevelwindow_colormap->id(),
			   XCB_CW_BORDER_PIXEL,
			   impl->xcb_screen->black_pixel);

	main_windowObj::handlerObj::constructor_params params{
		{
			screen(this),
			impl->xcb_screen->root, // parent
			impl->toplevelwindow_pictformat->depth, // depth
			dimensions, // initial_position
			XCB_WINDOW_CLASS_INPUT_OUTPUT, // window_class
			impl->toplevelwindow_visual->impl->visual_id, // visual
			vm, // events_and_mask
		},
		impl->toplevelwindow_pictformat
	};

	auto queue=connref->impl->thread->get_batch_queue();

	auto handler=ref<main_windowObj::handlerObj>
		::create(connref->impl->thread, params);

	auto window_impl=ref<main_windowObj::implObj>::create(handler);

	auto mw=ptrrefBase::objfactory<main_window>::create(window_impl,
							    layout_factory);

	f(mw);

	return mw;
}

LIBCXXW_NAMESPACE_END
