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

main_windowObj::~main_windowObj() noexcept=default;

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

	main_windowObj::handlerObj::constructor_params params{
		impl->toplevelwindow_pictformat,
		[background_color]
		{
			return background_color;
		}
	};


	auto handler=ref<main_windowObj::handlerObj>
		::create(connref->impl->thread, params);

	rectangle dimensions={
		coord_t{0},
		coord_t{0},
		dim_t{100},
		dim_t{100}
	};

	auto window_impl=ref<main_windowObj::implObj>
		::create(screen(this),
			 handler,
			 impl->xcb_screen->root,
			 dimensions,
			 XCB_WINDOW_CLASS_INPUT_OUTPUT);

	return ptrrefBase::objfactory<main_window>::create(window_impl);
}

LIBCXXW_NAMESPACE_END
