/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "draw_info.H"
#include "richtext/richtext_draw_boundaries.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

// Do only the bare minimum of work. We are told to draw only the
// given areas.

// First, translate element_view from absolute window coordinates
// to relative element coordinates. Compute the intersection with
// the given areas. If the result is empty, draw nothing.
//
// Otherwise we compute the bounding rectangle and draw only the
// fragments that fall within the boundaries. of the bounding
// rectangle.

richtext_draw_boundaries::richtext_draw_boundaries(const draw_info &di,
						   const rectangle_set &areas)
	: limits{bounds
		(({
				std::vector<rectangle>
					rects{di.element_viewport.begin(),
						di.element_viewport.end()
						};

				for (auto &r:rects)
				{
					r.x=coord_t::truncate
						(r.x-di.absolute_location.x);
					r.y=coord_t::truncate
						(r.y-di.absolute_location.y);
				}

				intersect(rectangle_set{rects.begin(),
							rects.end()},
					areas);
			}))},
	  draw_bounds{limits}
{
}

richtext_draw_boundaries::~richtext_draw_boundaries()=default;

void richtext_draw_boundaries::position_at(const rectangle &pos)
{
	if (pos.x < 0 || pos.y < 0)
		throw EXCEPTION("Internal error: negative coordinates in position_at()");

	auto res=intersect({limits}, {pos});

	if (res.empty())
	{
		draw_bounds={};
	}
	else
	{
		draw_bounds=*res.begin();

		draw_bounds.x=coord_t::truncate(draw_bounds.x-pos.x);
		draw_bounds.y=coord_t::truncate(draw_bounds.y-pos.y);
	}

	this->position=pos;
}

LIBCXXW_NAMESPACE_END
