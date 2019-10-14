/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "gc_thread.H"
#include "screen.H"
#include "connection.H"
#include "pixmap.H"
#include "messages.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

gcObj::implObj::implObj(const ref<drawableObj::implObj> &drawable)
	: handlerObj{*drawable},
	  drawable{drawable}
{
}

gcObj::implObj::~implObj()=default;

drawableObj::implObj &gcObj::implObj::get_drawable_impl()
{
	return *drawable;
}

const drawableObj::implObj &gcObj::implObj::get_drawable_impl() const
{
	return *drawable;
}


gcObj::handlerObj::handlerObj(const drawableObj::implObj &drawable)
	: xidObj(drawable.thread_)
{
	xcb_create_gc(conn(), gc_id(),
		      drawable.drawable_id, 0, nullptr);
}

gcObj::handlerObj::~handlerObj()
{
	xcb_free_gc(conn(), gc_id());
}

gcObj::handlerObj::configure_gc::configure_gc(const ref<handlerObj> &implArg,
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

gcObj::handlerObj::configure_gc::~configure_gc()=default;

void gcObj::handlerObj::change_gc_locked(const gcObj::properties &props)
{
	if (!props.references.mask.null())
	{
		if (props.references.mask->impl->get_screen()->impl !=
		    get_drawable_impl().get_screen()->impl)
			throw EXCEPTION("Attempting to set graphic context mask pixmap from a different root window");
		if (props.references.mask->impl->get_depth() != 1)
			throw EXCEPTION("Graphic context mask must have depth of 1");
	}

	if (!props.references.tile.null())
	{
		if (props.references.tile->impl->get_screen()->impl !=
		    get_drawable_impl().get_screen()->impl)
			throw EXCEPTION("Attempting to set graphic context tile pixmap from a different root window");

		if (props.references.tile->impl->get_depth() !=
		    get_drawable_impl().get_depth())
			throw EXCEPTION("Tile pixmap has wrong depth");
	}

	if (!props.references.stipple.null())
	{
		if (props.references.stipple->impl->get_screen()->impl !=
		    get_drawable_impl().get_screen()->impl)
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
				.x=xcoord_t::truncate(rectangle.x),
				.y=xcoord_t::truncate(rectangle.y),
				.width=xdim_t::truncate(rectangle.width),
				.height=xdim_t::truncate(rectangle.height)
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


void gcObj::handlerObj::fill_rectangles(const xcb_rectangle_t *rectangles,
				     size_t rectangles_size)
{
	if (!rectangles || !rectangles_size)
		return;

	xcb_poly_fill_rectangle(conn(), get_drawable_impl().drawable_id,
				gc_id(),
				rectangles_size, rectangles);
}

void gcObj::handlerObj::segments(const xcb_segment_t *segments,
			      size_t segments_size)
{
	if (!segments || !segments_size)
		return;

	xcb_poly_segment(conn(), get_drawable_impl().drawable_id,
			 gc_id(), segments_size,
			 segments);
}


void gcObj::handlerObj::points(const xcb_point_t *points,
			    size_t points_size,
			    gcObj::polyfill fill_type)
{
	if (!points || !points_size)
		return;

	switch (fill_type) {
	case polyfill::none:
		xcb_poly_line(conn(), XCB_COORD_MODE_ORIGIN,
			      get_drawable_impl().drawable_id, gc_id(),
			      points_size, points);
		break;
	case polyfill::complex:
		xcb_fill_poly(conn(), get_drawable_impl().drawable_id,
			      gc_id(),
			      XCB_POLY_SHAPE_COMPLEX,
			      XCB_COORD_MODE_ORIGIN,
			      points_size, points);
		break;
	case polyfill::nonconvex:
		xcb_fill_poly(conn(), get_drawable_impl().drawable_id,
			      gc_id(),
			      XCB_POLY_SHAPE_NONCONVEX,
			      XCB_COORD_MODE_ORIGIN,
			      points_size, points);
		break;
	case polyfill::convex:
		xcb_fill_poly(conn(), get_drawable_impl().drawable_id,
			      gc_id(),
			      XCB_POLY_SHAPE_CONVEX,
			      XCB_COORD_MODE_ORIGIN,
			      points_size, points);
		break;
	}
}


void gcObj::handlerObj::draw_arcs(const xcb_arc_t *arcs, size_t arc_size)
{
	if (!arcs || !arc_size)
		return;

	xcb_poly_arc(conn(), get_drawable_impl().drawable_id, gc_id(),
		     arc_size, arcs);
}


void gcObj::handlerObj::fill_arcs(const xcb_arc_t *arcs, size_t arc_size)
{
	if (!arcs || !arc_size)
		return;

	xcb_poly_fill_arc(conn(), get_drawable_impl().drawable_id, gc_id(),
			  arc_size, arcs);
}

void gcObj::handlerObj::copy(const rectangle &rect,
			     coord_t to_x, coord_t to_y,
			     const const_ref<drawableObj::implObj> &src,
			     const ref<drawableObj::implObj> &dst,
			     const gcObj::properties &props)
{
	configure_gc config{ref{this}, props};

	copy_configured(rect, to_x, to_y, src, dst);
}

void gcObj::handlerObj::copy_configured(const rectangle &rect,
					coord_t to_x, coord_t to_y,
					const const_ref<drawableObj::implObj>
					&src,
					const ref<drawableObj::implObj> &dst)
{
	if (src->thread_ != dst->thread_ ||
	    src->drawable_pictformat != dst->drawable_pictformat)
		throw EXCEPTION(_("copy() to_drawable is not compatible"));

	xcb_copy_area(src->thread_->info->conn,
		      src->drawable_id,
		      dst->drawable_id,
		      gc_id(),
		      coord_t::truncate(rect.x),
		      coord_t::truncate(rect.y),
		      coord_t::truncate(to_x),
		      coord_t::truncate(to_y),
		      coord_t::truncate(rect.width),
		      coord_t::truncate(rect.height));
}

LIBCXXW_NAMESPACE_END
