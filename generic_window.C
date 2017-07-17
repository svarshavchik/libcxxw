/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "generic_window_handler.H"
#include "drawable.H"
#include "screen.H"
#include "connection_thread.H"
#include <string>
#include <courier-unicode.h>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::generic_windowObj);

LIBCXXW_NAMESPACE_START

generic_windowObj::generic_windowObj(const ref<implObj> &impl,
				     const ref<layoutmanagerObj::implObj> &lm)
	: containerObj(impl->handler, lm),
	  drawableObj(impl->handler),
	  impl(impl)
{
}

generic_windowObj::~generic_windowObj()
{
	// Simulate what would happen if the generic window was in a container,
	// and it's now been removed from it.

	get_screen()->impl->thread->run_as
		([handler=this->impl->handler]
		 (IN_THREAD_ONLY)
		 {
			 handler->removed_from_container(IN_THREAD);
		 });
}


void generic_windowObj::on_disconnect(const std::function<void ()> &callback)
{
	get_screen()->get_connection()->on_disconnect(callback);
}

void generic_windowObj::set_window_title(const std::string_view &s)
{
	impl->handler->set_window_title(s);
}

void generic_windowObj::set_window_title(const std::u32string_view &s)
{
	std::string out_buf;
	bool ignore;

	unicode::iconvert::fromu::convert(s.begin(), s.end(),
					  unicode::utf_8, out_buf, ignore);

	impl->handler->set_window_title(out_buf);
}

LIBCXXW_NAMESPACE_END
