/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/rgb.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

rgb rgb::gradient(const rgb &other, component_t i, component_t n) const
{
	if (n == 0) return other; // GIGO

#define SCALE(channel) ((component_t)					\
			(int32_t)channel +				\
			(((int32_t)other.channel-channel)*(int32_t)i+(n/2))/n)

	return rgb(SCALE(r), SCALE(g), SCALE(b), SCALE(a));
}


rgb rgb::gradient(const gradient_t &g, size_t i, size_t n)
{
	if (g.find(0) == g.end())
		throw EXCEPTION(_("Color #0 must be specified for a gradient"));

	auto last=--g.end();

	auto k=last->first;

	// First, map i to the range 0..k

	auto scaled_i= (i*k)/n;

	auto next=g.upper_bound(scaled_i);

	if (next == g.end())
		return (--next)->second; // Edge condition.

	auto prev=next;
	--prev;

	return prev->second.gradient(next->second, scaled_i-prev->first,
				     next->first-prev->first);
}

LIBCXXW_NAMESPACE_END
