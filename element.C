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
#include "x/w/picture.H"

LIBCXXW_NAMESPACE_START

elementObj::elementObj(const ref<implObj> &impl)
	: impl(impl)
{
}

elementObj::~elementObj()
{
	get_screen()->impl->thread->run_as
		(RUN_AS,
		 [impl=this->impl]
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

void elementObj::set_background_color(const std::experimental::string_view
				      &name,
				      const rgb &default_value)
{
	impl->set_background_color(name, default_value);
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

ref<obj> elementObj::do_on_state_update(const element_state_update_handler_t &h)
{
	return impl->do_on_state_update(h);
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
