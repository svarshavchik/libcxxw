/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element.H"
#include "element_screen.H"

LIBCXXW_NAMESPACE_START

elementObj::elementObj(const ref<implObj> &impl)
	: impl(impl)
{
}

elementObj::~elementObj()
{
}

screen elementObj::get_screen()
{
       return impl->get_screen();
}

const_screen elementObj::get_screen() const
{
       return impl->get_screen();
}

void elementObj::show()
{
	impl->request_visibility(true);
}

void elementObj::hide()
{
	impl->request_visibility(false);
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

LIBCXXW_NAMESPACE_END
