/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "generic_window_handler.H"
#include "drawable.H"
#include "screen.H"
#include "run_as.H"
#include "connection_thread.H"
#include "xid_t.H"
#include <string>
#include <courier-unicode.h>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::generic_windowObj);

LIBCXXW_NAMESPACE_START

generic_windowObj::generic_windowObj(const ref<implObj> &impl,
				     const layout_impl &lm)
	: containerObj(impl->handler, lm),
	  drawableObj(impl->handler),
	  impl(impl)
{
}

generic_windowObj::~generic_windowObj()=default;

void generic_windowObj::on_disconnect(const functionref<void ()> &callback)
{
	get_screen()->get_connection()->on_disconnect(callback);
}

void generic_windowObj::on_stabilized(const stabilized_callback_t &callback)
{
	impl->handler->thread()->run_as
		([callback, handler=impl->handler]
		 (ONLY IN_THREAD)
		 {
			 handler->stabilized_callbacks(IN_THREAD)
				 .push_back(callback);

			 handler->invoke_stabilized(IN_THREAD);
		 });
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
	impl->handler->thread()->run_as
		([instance=std::string{instance.begin(), instance.end()},
		  resource=std::string{resource.begin(), resource.end()},
		  handler=impl->handler]
		 (ONLY IN_THREAD)
		 {
			 handler->wm_class_instance(IN_THREAD)=instance;
			 handler->wm_class_resource(IN_THREAD)=resource;
		 });
}

void generic_windowObj::set_window_type(const std::string &s)
{
	impl->handler->set_window_type(s);
}

void generic_windowObj::raise()
{
	impl->handler->thread()->run_as
		([handler=impl->handler]
		 (ONLY IN_THREAD)
		 {
			 handler->raise(IN_THREAD);
		 });
}

void generic_windowObj::lower()
{
	impl->handler->thread()->run_as
		([handler=impl->handler]
		 (ONLY IN_THREAD)
		 {
			 handler->lower(IN_THREAD);
		 });
}

bool generic_windowObj::selection_has_owner() const
{
	return selection_has_owner(elementObj::impl
				   ->default_cut_paste_selection());
}

bool generic_windowObj::selection_has_owner(const std::string_view &selection)
	const
{
	return get_screen()->selection_has_owner(selection);
}

bool generic_windowObj::selection_can_be_received() const
{
	return !!impl->handler->element_that_can_receive_selection();
}

void generic_windowObj::receive_selection(ONLY IN_THREAD,
					  const std::string_view &selection)
{
	return impl->handler->receive_selection(IN_THREAD, selection);
}

bool generic_windowObj::cut_or_copy_selection(cut_or_copy_op op,
					      const std::string_view &selection)
{
	return impl->handler->cut_or_copy_selection(op, selection);
}

bool generic_windowObj::cut_or_copy_selection(ONLY IN_THREAD,
					      cut_or_copy_op op,
					      const std::string_view &selection)
{
	return impl->handler->cut_or_copy_selection(IN_THREAD, op, selection);
}

LIBCXXW_NAMESPACE_END
