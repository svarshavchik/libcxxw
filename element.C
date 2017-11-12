/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element.H"
#include "element_screen.H"
#include "screen.H"
#include "batch_queue.H"
#include "background_color.H"
#include "generic_window_handler.H"
#include "draw_info.H"
#include "busy.H"
#include "x/w/picture.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

elementObj::elementObj(const ref<implObj> &impl)
	: impl(impl)
{
}

elementObj::~elementObj()
{
	impl->THREAD->run_as
		([impl=this->impl]
		 (IN_THREAD_ONLY)
		 {
			 impl->get_window_handler().removing_element(IN_THREAD,
								     impl);
		 });
}

ref<obj> elementObj::connection_mcguffin() const
{
	return get_screen()->connection_mcguffin();
}

screen elementObj::get_screen()
{
       return impl->get_screen();
}

const_screen elementObj::get_screen() const
{
       return impl->get_screen();
}

void elementObj::show_all()
{
	impl->request_visibility_recursive(true);
}

void elementObj::hide_all()
{
	impl->request_visibility_recursive(false);
}

void elementObj::show()
{
	impl->request_visibility(true);
}

void elementObj::hide()
{
	impl->request_visibility(false);
}

void elementObj::set_background_color(const color_arg &name)
{
	impl->set_background_color(name);
}

void elementObj::set_background_color(const const_picture &background_color)
{
	impl->set_background_color(background_color);
}

void elementObj::remove_background_color()
{
	impl->remove_background_color();
}

const_picture elementObj::create_solid_color_picture(const rgb &color) const
{
	return impl->get_screen()->create_solid_color_picture(color);
}

void elementObj::on_state_update(const std::function<element_state_callback_t
				 > &cb)
{
	impl->on_state_update(cb);
}

void elementObj::on_pointer_focus(const std::function<focus_callback_t> &callback)
{
	impl->on_pointer_focus(callback);
}

void elementObj::create_custom_tooltip(const std::function<void
				       (const tooltip_factory &)>
				       &tooltip_factory) const
{
	impl->THREAD->run_as
		([impl=this->impl, tooltip_factory]
		 (IN_THREAD_ONLY)
		 {
			 impl->data(IN_THREAD).tooltip_factory=tooltip_factory;
		 });
}


void elementObj::remove_tooltip() const
{
	impl->THREAD->run_as
		([impl=this->impl]
		 (IN_THREAD_ONLY)
		 {
			 impl->data(IN_THREAD).tooltip_factory=nullptr;
		 });
}

x::ref<x::obj> elementObj::get_busy_mcguffin() const
{
	return impl->get_window_handler().get_busy_mcguffin();
}

ref<elementObj::implObj> elementObj::get_minimum_override_element_impl()
{
	return impl;
}

std::ostream &operator<<(std::ostream &o, const element_state &s)
{
	return o << "state update: " << (int)s.state_update
		 << ", shown=" << s.shown
		 << ", position: " << s.current_position << std::endl;
}

///////////////////////////////////////////////////////////////////

std::pair<coord_t, coord_t> draw_info::background_xy_to(coord_t x,
							coord_t y,
							coord_t offset_x,
							coord_t offset_y) const
{
	coord_squared_t::value_type x2=coord_t::value_type(background_x)
		- coord_t::value_type(x);

	coord_squared_t::value_type y2=coord_t::value_type(background_y)
		- coord_t::value_type(y);

	x2 += coord_t::value_type(offset_x);
	y2 += coord_t::value_type(offset_y);

	return { coord_t::truncate(x2), coord_t::truncate(y2) };
}
LIBCXXW_NAMESPACE_END
