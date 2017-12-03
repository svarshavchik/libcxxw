/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "generic_window_handler.H"
#include "drawable.H"
#include "screen.H"
#include "run_as.H"
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

generic_windowObj::~generic_windowObj()=default;

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

void generic_windowObj::set_window_class(const std::string_view &instance,
					 const std::string_view &resource)
{
	impl->handler->IN_THREAD->run_as
		([instance=std::string{instance.begin(), instance.end()},
		  resource=std::string{resource.begin(), resource.end()},
		  handler=impl->handler]
		 (IN_THREAD_ONLY)
		 {
			 handler->wm_class_instance(IN_THREAD)=instance;
			 handler->wm_class_resource(IN_THREAD)=resource;
		 });
}

LIBCXXW_NAMESPACE_END
