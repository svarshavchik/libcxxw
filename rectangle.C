#include "x/w/rectangle.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

std::ostream &operator<<(std::ostream &o, const rectangle &r)
{
	o << x::gettextmsg(_("x=%1%, y=%2%, width=%3%, height=%4%"),
			   r.x,
			   r.y,
			   r.width,
			   r.height);
}

LIBCXXW_NAMESPACE_END
