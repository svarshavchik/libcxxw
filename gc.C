/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gc.H"
#include "pixmap.H"

#include <x/chrcasecmp.H>
#include "messages.H"

LIBCXXW_NAMESPACE_START

#include "gc_constants.inc.C"

std::string gc::base::function_to_string(gc::base::function v)
{
	return gc_function_to_string(v);
}

gc::base::function gc::base::function_from_string(const std::string_view &s)
{
	return gc_function_from_string(s);
}

std::string gc::base::line_style_to_string(gc::base::line_style v)
{
	return gc_line_style_to_string(v);
}

gc::base::line_style gc::base::line_style_from_string(const std::string_view &s)
{
	return gc_line_style_from_string(s);
}

std::string gc::base::fill_arc_mode_to_string(gc::base::fill_arc_mode v)
{
	return gc_fill_arc_mode_to_string(v);
}

gc::base::fill_arc_mode gc::base::fill_arc_mode_from_string(const std::string_view &s)
{
	return gc_fill_arc_mode_from_string(s);
}

std::string gc::base::cap_style_to_string(cap_style v)
{
	return gc_cap_style_to_string(v);
}

cap_style gc::base::cap_style_from_string(const std::string_view &s)
{
	return gc_cap_style_from_string(s);
}

std::string gc::base::join_style_to_string(join_style v)
{
	return gc_join_style_to_string(v);
}

join_style gc::base::join_style_from_string(const std::string_view &s)
{
	return gc_join_style_from_string(s);
}

gcObj::properties::properties()
	: raw_values(XCB_GC_FUNCTION, XCB_GX_COPY,
		     XCB_GC_PLANE_MASK, ~0,
		     XCB_GC_FOREGROUND, 0,
		     XCB_GC_BACKGROUND, 1,
		     XCB_GC_LINE_WIDTH, 0,
		     XCB_GC_LINE_STYLE, XCB_LINE_STYLE_SOLID,
		     XCB_GC_CAP_STYLE, XCB_CAP_STYLE_BUTT,
		     XCB_GC_JOIN_STYLE, XCB_JOIN_STYLE_MITER,
		     XCB_GC_FILL_STYLE, XCB_FILL_STYLE_SOLID,
		     XCB_GC_FILL_RULE, XCB_FILL_RULE_EVEN_ODD,
		     XCB_GC_TILE_STIPPLE_ORIGIN_X, 0,
		     XCB_GC_TILE_STIPPLE_ORIGIN_Y, 0,
		     // XCB_GC_FONT, ,
		     XCB_GC_SUBWINDOW_MODE, XCB_SUBWINDOW_MODE_CLIP_BY_CHILDREN,
		     XCB_GC_GRAPHICS_EXPOSURES, 1,
		     XCB_GC_CLIP_ORIGIN_X, 0,
		     XCB_GC_CLIP_ORIGIN_Y, 0,
		     XCB_GC_CLIP_MASK, XCB_NONE,
		     XCB_GC_DASH_OFFSET, 0, // Handled via set_dashes()
		     XCB_GC_ARC_MODE, XCB_ARC_MODE_PIE_SLICE)
{
}

gcObj::properties::~properties() noexcept
{
}

gcObj::properties::properties(const properties &)=default;

gcObj::properties &gcObj::properties::operator=(const properties &)=default;

gcObj::properties &gcObj::properties::function(gcObj::function function_value)
{
	raw_values.m[XCB_GC_FUNCTION]=(uint32_t)function_value;
	return *this;
}

gcObj::properties &gcObj::properties::arc_mode(fill_arc_mode value)
{
	raw_values.m[XCB_GC_ARC_MODE]=(uint32_t)value;
	return *this;
}

gcObj::properties &gcObj::properties::cap_style(w::cap_style value)
{
	raw_values.m[XCB_GC_CAP_STYLE]=(uint32_t)value;
	return *this;
}

gcObj::properties &gcObj::properties::join_style(w::join_style value)
{
	raw_values.m[XCB_GC_JOIN_STYLE]=(uint32_t)value;
	return *this;
}

gcObj::properties &gcObj::properties::fill_style_solid()
{
	raw_values.m[XCB_GC_FILL_STYLE]=XCB_FILL_STYLE_SOLID;
	raw_values.m.erase(XCB_GC_TILE);
	raw_values.m.erase(XCB_GC_STIPPLE);
	raw_values.m[XCB_GC_TILE_STIPPLE_ORIGIN_X]=0;
	raw_values.m[XCB_GC_TILE_STIPPLE_ORIGIN_Y]=0;
	references.tile=pixmapptr();
	references.stipple=pixmapptr();
	return *this;
}

gcObj::properties &gcObj::properties::fill_style_tiled(const pixmap &tileArg,
						       coord_t x,
						       coord_t y)
{
	fill_style_solid();

	raw_values.m[XCB_GC_FILL_STYLE]=XCB_FILL_STYLE_TILED;
	raw_values.m[XCB_GC_TILE]=tileArg->impl->pixmap_id();
	raw_values.m[XCB_GC_TILE_STIPPLE_ORIGIN_X]=(coord_t::value_type)x;
	raw_values.m[XCB_GC_TILE_STIPPLE_ORIGIN_Y]=(coord_t::value_type)y;

	references.tile=tileArg;
	return *this;
}

gcObj::properties &gcObj::properties
      ::fill_style_stippled(const pixmap &tileArg,
			    coord_t x,
			    coord_t y)
{
	fill_style_solid();

	raw_values.m[XCB_GC_FILL_STYLE]=XCB_FILL_STYLE_STIPPLED;
	raw_values.m[XCB_GC_STIPPLE]=tileArg->impl->pixmap_id();
	raw_values.m[XCB_GC_TILE_STIPPLE_ORIGIN_X]=(coord_t::value_type)x;
	raw_values.m[XCB_GC_TILE_STIPPLE_ORIGIN_Y]=(coord_t::value_type)y;
	references.stipple=tileArg;
	return *this;
}

gcObj::properties &gcObj::properties
      ::fill_style_opaque_stippled(const pixmap &tileArg,
				   coord_t x,
				   coord_t y)
{
	fill_style_solid();

	raw_values.m[XCB_GC_FILL_STYLE]=XCB_FILL_STYLE_OPAQUE_STIPPLED;
	raw_values.m[XCB_GC_STIPPLE]=tileArg->impl->pixmap_id();
	raw_values.m[XCB_GC_TILE_STIPPLE_ORIGIN_X]=(coord_t::value_type)x;
	raw_values.m[XCB_GC_TILE_STIPPLE_ORIGIN_Y]=(coord_t::value_type)y;

	references.stipple=tileArg;
	return *this;
}

gcObj::	properties &gcObj::properties::fill_rule(w::fill_rule value)
{
	raw_values.m[XCB_GC_FILL_RULE]=(uint32_t)value;
	return *this;
}

gcObj::properties &gcObj::properties::foreground(uint32_t pixel)
{
	raw_values.m[XCB_GC_FOREGROUND]=pixel;
	return *this;
}

gcObj::properties &gcObj::properties::background(uint32_t pixel)
{
	raw_values.m[XCB_GC_BACKGROUND]=pixel;
	return *this;
}

gcObj::properties &gcObj::properties::line_width(uint32_t width)
{
	raw_values.m[XCB_GC_LINE_WIDTH]=width;
	return *this;
}

uint32_t gcObj::properties::line_width() const
{
	auto p=raw_values.m.find(XCB_GC_LINE_WIDTH);

	return p == raw_values.m.end() ? 0:p->second;
}

gcObj::properties &gcObj::properties::line_style(gcObj::line_style
						 line_styleArg)
{
	raw_values.m[XCB_GC_LINE_STYLE]=(uint32_t)line_styleArg;
	return *this;
}

gcObj::properties &gcObj::properties::clipmask(const pixmap &pixmap_arg,
					       uint32_t origin_x,
					       uint32_t origin_y)
{
	clipmask();

	references.mask=pixmap_arg;

	raw_values.m[XCB_GC_CLIP_MASK]=pixmap_arg->impl->pixmap_id();
	raw_values.m[XCB_GC_CLIP_ORIGIN_X]=origin_x;
	raw_values.m[XCB_GC_CLIP_ORIGIN_Y]=origin_y;
	return *this;
}

gcObj::properties &gcObj::properties::clipmask(const rectarea &r,
					       uint32_t origin_x,
					       uint32_t origin_y)
{
	clipmask();

	raw_values.m.erase(XCB_GC_CLIP_MASK);
	current_cliprects=r;
	has_cliprects=true;
	raw_values.m[XCB_GC_CLIP_ORIGIN_X]=origin_x;
	raw_values.m[XCB_GC_CLIP_ORIGIN_Y]=origin_y;
	return *this;
}

gcObj::properties &gcObj::properties::clipmask()
{
	references.mask=pixmapptr();
	has_cliprects=false;

	raw_values.m[XCB_GC_CLIP_MASK]=XCB_NONE;
	raw_values.m[XCB_GC_CLIP_ORIGIN_X]=0;
	raw_values.m[XCB_GC_CLIP_ORIGIN_Y]=0;
	return *this;
}

gcObj::gcObj(const drawable &gc_drawable,const ref<implObj> &impl)
	: gc_drawable(gc_drawable),
	  impl(impl)
{
}

gcObj::~gcObj() noexcept
{
}

void gcObj::fill_rectangle(const rectangle &rectangleArg,
			   const properties &props)
{
	rectarea rectangles;

	rectangles.push_back(rectangleArg);
	fill_rectangles(rectangles, props);
}

void gcObj::fill_rectangles(const rectarea &rectangles,
			    const properties &props)
{
	if (rectangles.empty())
		return;

	xcb_rectangle_t xcb_rectangles[rectangles.size()];

	size_t i=0;

	for (const auto &rectangle:rectangles)
	{
		xcb_rectangles[i++]= xcb_rectangle_t({
				.x=xcoord_t::truncate(rectangle.x),
				.y=xcoord_t::truncate(rectangle.y),
				.width=xdim_t::truncate(rectangle.width),
				.height=xdim_t::truncate(rectangle.height)
					});
	}

	// Make sure noone changes gc until we finish drawing
	implObj::configure_gc config(impl, props);
	impl->fill_rectangles(&xcb_rectangles[0], i);
}

void gcObj::segment(const line &lineArg,
		    const properties &props)
{
	std::vector<line> dummy;

	dummy.push_back(lineArg);

	segments(dummy, props);
}

void gcObj::segments(const std::vector<line> &linesArg,
		     const properties &props)
{
	if (linesArg.empty())
		return;

	xcb_segment_t xcb_segments[linesArg.size()];

	size_t i=0;

	for (const auto &line:linesArg)
	{
		xcb_segments[i++]=xcb_segment_t{
			xcoord_t::truncate(line.x1),
			xcoord_t::truncate(line.y1),
			xcoord_t::truncate(line.x2),
			xcoord_t::truncate(line.y2)};
	}

	// Make sure noone changes gc until we finish drawing

	implObj::configure_gc config(impl, props);
	impl->segments(&xcb_segments[0], i);
}

void gcObj::lines(const std::vector<polyline> &linesArg,
		  const properties &props,
		  polyfill fill_type)
{
	if (linesArg.size() < 2)
		return;
	lines(&linesArg.at(0), linesArg.size(), props, fill_type);
}

void gcObj::lines(const polyline *line,
		  size_t nlines,
		  const properties &props,
		  polyfill fill_type)
{
	if (nlines < 2)
		return;

	xcb_point_t xcb_points[nlines];

	for (size_t i=0; i<nlines; ++i)
		xcb_points[i] = xcb_point_t{
			xcoord_t::truncate(line[i].x),
			xcoord_t::truncate(line[i].y)};

	// Make sure noone changes gc until we finish drawing

	implObj::configure_gc config(impl, props);
	impl->points(&xcb_points[0], nlines, fill_type);
}
void gcObj::draw_arc(const arc &arcArg,
		     const properties &props)
{
	draw_arcs(&arcArg, 1, props);
}

// Convert public ARC structures to xcb_arcs

static void convert(const gcObj::arc *arcs, size_t n_arcs,
		    xcb_arc_t *buf)
{
	for (size_t i=0; i<n_arcs; ++i)
	{
		*buf=xcb_arc_t({
				.x=xcoord_t::truncate(arcs[i].x),
				.y=xcoord_t::truncate(arcs[i].y),
				.width=xdim_t::truncate(arcs[i].width),
				.height=xdim_t::truncate(arcs[i].height),
				.angle1=arcs[i].angle1,
				.angle2=arcs[i].angle2,
					});
		++buf;
	}
}

void gcObj::draw_arcs(const arc *arcs,
		      size_t n_arcs,
		      const properties &props)
{
	if (n_arcs == 0)
		return;

	xcb_arc_t xcb_arcs[n_arcs];

	convert(arcs, n_arcs, xcb_arcs);

	// Make sure noone changes gc until we finish drawing

	implObj::configure_gc config(impl, props);
	impl->draw_arcs(&xcb_arcs[0], n_arcs);
}

void gcObj::fill_arc(const arc &arcArg,
		     const properties &props)
{
	fill_arcs(&arcArg, 1, props);
}

void gcObj::fill_arcs(const arc *arcs,
		      size_t n_arcs,
		      const properties &props)
{
	if (n_arcs == 0)
		return;

	xcb_arc_t xcb_arcs[n_arcs];

	convert(arcs, n_arcs, xcb_arcs);

	// Make sure noone changes gc until we finish drawing

	implObj::configure_gc config(impl, props);
	impl->fill_arcs(&xcb_arcs[0], n_arcs);
}

LIBCXXW_NAMESPACE_END
