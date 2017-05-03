/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "gc_thread.H"
#include "screen.H"
#include "connection.H"
#include "pixmap.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

gcObj::implObj::implObj(const ref<drawableObj::implObj> &drawable)
	: xidObj(drawable->get_screen()->get_connection()->impl->thread),
	  drawable(drawable)
{
	xcb_create_gc(conn(), gc_id(),
		      drawable->drawable_id, 0, nullptr);
}

gcObj::implObj::~implObj()
{
	xcb_free_gc(conn(), gc_id());
}

gcObj::implObj::configure_gc::configure_gc(const ref<implObj> &implArg,
					   const gcObj::properties &propsArg)
	: impl(implArg), props(propsArg),
	  lock(impl->configured_properties)
{
	if (lock->configured_values == propsArg.raw_values &&
	    lock->configured_dashes == propsArg.dashes &&
	    lock->configured_dashes_offset == propsArg.dashes_offset)
		return;

	impl->change_gc_locked(props);

	lock->configured_values=propsArg.raw_values;
	lock->configured_dashes=propsArg.dashes;
	lock->configured_dashes_offset=propsArg.dashes_offset;
}

gcObj::implObj::configure_gc::~configure_gc()=default;

void gcObj::implObj::change_gc_locked(const gcObj::properties &props)
{
	if (!props.references.mask.null())
	{
		if (props.references.mask->impl->get_screen()->impl !=
		    drawable->get_screen()->impl)
			throw EXCEPTION("Attempting to set graphic context mask pixmap from a different root window");
		if (props.references.mask->impl->get_depth() != 1)
			throw EXCEPTION("Graphic context mask must have depth of 1");
	}

	if (!props.references.tile.null())
	{
		if (props.references.tile->impl->get_screen()->impl !=
		    drawable->get_screen()->impl)
			throw EXCEPTION("Attempting to set graphic context tile pixmap from a different root window");

		if (props.references.tile->impl->get_depth() !=
		    drawable->get_depth())
			throw EXCEPTION("Tile pixmap has wrong depth");
	}

	if (!props.references.stipple.null())
	{
		if (props.references.stipple->impl->get_screen()->impl !=
		    drawable->get_screen()->impl)
			throw EXCEPTION("Attempting to set graphic context stipple pixmap from a different root window");
		if (props.references.stipple->impl->get_depth() != 1)
			throw EXCEPTION("Stipple pixmap must have depth of 1");
	}

	auto value_array=props.raw_values.values();

	xcb_change_gc(conn(), gc_id(), props.raw_values.mask(),
		      &value_array[0]);
	if (props.raw_values.m.find(XCB_GC_LINE_STYLE)->second
	    != XCB_LINE_STYLE_SOLID)
	{
		xcb_set_dashes(conn(), gc_id(),
			       (dim_t::value_type)props.dashes_offset,
			       props.dashes.size(), &props.dashes[0]);
	}

	if (props.has_cliprects)
	{
		xcb_rectangle_t
			xcb_rectangles[props.current_cliprects.size()+1];

		size_t i=0;

		for (const auto &rectangle:props.current_cliprects)
		{
			xcb_rectangles[i++]= xcb_rectangle_t({
				.x=(coord_t::value_type)rectangle.x,
				.y=(coord_t::value_type)rectangle.y,
				.width=(dim_t::value_type)rectangle.width,
				.height=(dim_t::value_type)rectangle.height
			});
		}
		xcb_set_clip_rectangles(conn(),
					XCB_CLIP_ORDERING_UNSORTED,
					gc_id(),
					props.raw_values.m
					.at(XCB_GC_CLIP_ORIGIN_X),
					props.raw_values.m
					.at(XCB_GC_CLIP_ORIGIN_Y),
					i, xcb_rectangles);
	}
}


void gcObj::implObj::fill_rectangles(const xcb_rectangle_t *rectangles,
				     size_t rectangles_size)
{
	if (!rectangles || !rectangles_size)
		return;

	xcb_poly_fill_rectangle(conn(), drawable->drawable_id,
				gc_id(),
				rectangles_size, rectangles);
}

void gcObj::implObj::segments(const xcb_segment_t *segments,
			      size_t segments_size)
{
	if (!segments || !segments_size)
		return;

	xcb_poly_segment(conn(), drawable->drawable_id,
			 gc_id(), segments_size,
			 segments);
}


void gcObj::implObj::points(const xcb_point_t *points,
			    size_t points_size,
			    gcObj::polyfill fill_type)
{
	if (!points || !points_size)
		return;

	switch (fill_type) {
	case polyfill::none:
		xcb_poly_line(conn(), XCB_COORD_MODE_ORIGIN,
			      drawable->drawable_id, gc_id(), points_size, points);
		break;
	case polyfill::complex:
		xcb_fill_poly(conn(), drawable->drawable_id,
			      gc_id(),
			      XCB_POLY_SHAPE_COMPLEX,
			      XCB_COORD_MODE_ORIGIN,
			      points_size, points);
		break;
	case polyfill::nonconvex:
		xcb_fill_poly(conn(), drawable->drawable_id,
			      gc_id(),
			      XCB_POLY_SHAPE_NONCONVEX,
			      XCB_COORD_MODE_ORIGIN,
			      points_size, points);
		break;
	case polyfill::convex:
		xcb_fill_poly(conn(), drawable->drawable_id,
			      gc_id(),
			      XCB_POLY_SHAPE_CONVEX,
			      XCB_COORD_MODE_ORIGIN,
			      points_size, points);
		break;
	}
}


void gcObj::implObj::draw_arcs(const xcb_arc_t *arcs, size_t arc_size)
{
	if (!arcs || !arc_size)
		return;

	xcb_poly_arc(conn(), drawable->drawable_id, gc_id(), arc_size, arcs);
}


void gcObj::implObj::fill_arcs(const xcb_arc_t *arcs, size_t arc_size)
{
	if (!arcs || !arc_size)
		return;

	xcb_poly_fill_arc(conn(), drawable->drawable_id, gc_id(), arc_size, arcs);
}

LIBCXXW_NAMESPACE_END
