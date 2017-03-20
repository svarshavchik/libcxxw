/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "generic_window_handler.H"
#include "drawable.H"
#include "x/w/screen.H"
#include <string>
#include <courier-unicode.h>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::generic_windowObj);

LIBCXXW_NAMESPACE_START

generic_windowObj::generic_windowObj(const ref<implObj> &impl,
				     const new_layoutmanager &layout_factory)
	: containerObj(impl->handler, layout_factory),
	  drawableObj(impl->handler),
	  impl(impl)
{
}

generic_windowObj::~generic_windowObj()=default;

void generic_windowObj::set_window_title(const std::experimental::string_view &s)
{
	impl->handler->set_window_title(s);
}

void generic_windowObj::set_window_title(const std::experimental::u32string_view &s)
{
	std::string out_buf;
	bool ignore;

	unicode::iconvert::fromu::convert(s.begin(), s.end(),
					  unicode::utf_8, out_buf, ignore);

	impl->handler->set_window_title(out_buf);
}

LIBCXXW_NAMESPACE_END
