/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "picture.H"
#include "pictformat.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "screen_picturecache.H"
#include "x/w/rgbfwd.H"
#include "screen.H"
#include "messages.H"

#include <x/functional.H>
#include <vector>
#include <algorithm>
#include <cmath>

LIBCXXW_NAMESPACE_START

inline static constexpr xcb_render_fixed_t
to_fixed(const pictureObj::fixedprec &f)
{
	return (((xcb_render_fixed_t)f.integer << 16) | f.fraction);
}

inline static constexpr xcb_render_pointfix_t
to_pointfix(const pictureObj::point &p)
{
	return { to_fixed(p.x), to_fixed(p.y) };
}

inline static constexpr xcb_render_color_t to_color(const rgb &rgb)
{
	return xcb_render_color_t({
			.red=rgb.r,
			.green=rgb.g,
			.blue=rgb.b,
			.alpha=rgb.a});
}

static void to_xcb_rectangles(std::vector<xcb_rectangle_t> &v,
			      const rectangle_set &rectangles)
{
	v.clear();
	v.reserve(rectangles.size());

	for (const auto &r : rectangles)
	{
		v.push_back({xcoord_t::truncate(r.x),
					xcoord_t::truncate(r.y),
					xdim_t::truncate(r.width),
					xdim_t::truncate(r.height)});
	}
}


pictureObj::implObj::implObj(const connection_thread &thread)
	: picture_xid(thread)
{
}

pictureObj::implObj::~implObj()=default;

void pictureObj::implObj::set_clip_rectangle(const rectangle &r,
					     coord_t x,
					     coord_t y)
{
	rectangle_set s={ r };

	set_clip_rectangles(s, x, y);
}

void pictureObj::implObj::set_clip_rectangles(const rectangle_set &clipregion,
					      coord_t x,
					      coord_t y)
{
	std::vector<xcb_rectangle_t> v;

	to_xcb_rectangles(v, clipregion);

	xcb_render_set_picture_clip_rectangles(picture_conn()->conn,
					       picture_id(),
					       xcoord_t::truncate(x),
					       xcoord_t::truncate(y),
					       v.size(),
					       v.data());
}

void pictureObj::implObj::clear_clip()
{
	uint32_t v=XCB_NONE;

	xcb_render_change_picture(picture_conn()->conn, picture_id(),
				  (uint32_t)XCB_RENDER_CP_CLIP_MASK, &v);
}

void pictureObj::implObj::composite(const const_picture_internal &src,
				    coord_t src_x,
				    coord_t src_y,
				    coord_t dst_x,
				    coord_t dst_y,
				    dim_t width,
				    dim_t height,
				    render_pict_op op)
{
	xcb_render_composite(picture_conn()->conn,
			     (uint8_t)op,
			     src->picture_id(),
			     XCB_NONE,
			     picture_id(),
			     xcoord_t::truncate(src_x),
			     xcoord_t::truncate(src_y),
			     0, 0,
			     xcoord_t::truncate(dst_x),
			     xcoord_t::truncate(dst_y),
			     xdim_t::truncate(width),
			     xdim_t::truncate(height));
}

void pictureObj::implObj::composite(const const_picture_internal &src,
				    const const_picture_internal &mask,
				    coord_t src_x,
				    coord_t src_y,
				    coord_t mask_x,
				    coord_t mask_y,
				    coord_t dst_x,
				    coord_t dst_y,
				    dim_t width,
				    dim_t height,
				    render_pict_op op)
{
	xcb_render_composite(picture_conn()->conn,
			     (uint8_t)op,
			     src->picture_id(),
			     mask->picture_id(),
			     picture_id(),
			     xcoord_t::truncate(src_x),
			     xcoord_t::truncate(src_y),
			     xcoord_t::truncate(mask_x),
			     xcoord_t::truncate(mask_y),
			     xcoord_t::truncate(dst_x),
			     xcoord_t::truncate(dst_y),
			     xdim_t::truncate(width),
			     xdim_t::truncate(height));
}

void pictureObj::implObj::fill_tri_strip(const point *points,
					 size_t n_points,
					 const const_picture_internal &src,
					 render_pict_op op,
					 coord_t src_x,
					 coord_t src_y)
{
	do_fill_tri_strip(points, n_points, src, XCB_NONE, op, src_x, src_y);
}

void pictureObj::implObj::fill_tri_strip(const point *points,
					 size_t n_points,
					 const const_picture_internal &src,
					 const const_pictformat &mask,
					 render_pict_op op,
					 coord_t src_x,
					 coord_t src_y)
{
	do_fill_tri_strip(points, n_points, src, mask->impl->id, op,
			  src_x, src_y);
}

void pictureObj::implObj::do_fill_tri_strip(const point *points,
					    size_t n_points,
					    const const_picture_internal &src,
					    xcb_render_pictformat_t mask,
					    render_pict_op op,
					    coord_t src_x,
					    coord_t src_y)
{
	if (n_points <= 0)
		return;


	xcb_render_pointfix_t strip[n_points];

	for (size_t i=0; i<n_points; ++i)
		strip[i]=xcb_render_pointfix_t(to_pointfix(points[i]));

	xcb_render_tri_strip(picture_conn()->conn,
			     (uint8_t)op,
			     src->picture_id(),
			     picture_id(),
			     mask,
			     coord_t::value_type(src_x),
			     coord_t::value_type(src_y),
			     n_points,
			     &strip[0]);
}

void pictureObj::implObj::repeat(render_repeat value)
{
	uint32_t v=(uint32_t)value;

	xcb_render_change_picture(picture_conn()->conn, picture_id(),
				  (uint32_t)XCB_RENDER_CP_REPEAT, &v);
}

void pictureObj::implObj::fill_rectangle(coord_t x, coord_t y,
					 dim_t width, dim_t height,
					 const rgb &color,
					 render_pict_op op)
{
	return fill_rectangle(rectangle{x, y, width, height}, color, op);
}

void pictureObj::implObj::fill_rectangle(const rectangle &rect,
					 const rgb &color,
					 render_pict_op op)
{

	return fill_rectangles(&rect, 1, color, op);
}

void pictureObj::implObj::fill_rectangles(const rectangle *rectangles,
					  size_t n,
					  const rgb &color,
					  render_pict_op op)
{
	if (n <= 0)
		return;

	xcb_rectangle_t rects[n];

	for (size_t i=0; i<n; ++i)
		rects[i]=xcb_rectangle_t({
				.x=xcoord_t::truncate(rectangles[i].x),
				.y=xcoord_t::truncate(rectangles[i].y),
				.width=xdim_t::truncate(rectangles[i].width),
				.height=xdim_t::truncate(rectangles[i].height),
					});

	xcb_render_fill_rectangles(picture_conn()->conn,
				   (uint8_t)op,
				   picture_id(),
				   to_color(color),
				   n,
				   &rects[0]);
}

void pictureObj::implObj::fill_triangles(const std::set<triangle> &triangles,
					 const const_picture_internal &src,
					 render_pict_op op,
					 coord_t src_x,
					 coord_t src_y)
{
	do_fill_triangles(triangles, src, XCB_NONE, op, src_x, src_y);
}

void pictureObj::implObj::fill_triangles(const std::set<triangle> &triangles,
					 const const_picture_internal &src,
					 const const_pictformat &mask,
					 render_pict_op op,
					 coord_t src_x,
					 coord_t src_y)
{
	do_fill_triangles(triangles, src, mask->impl->id, op, src_x, src_y);
}

void pictureObj::implObj::do_fill_triangles(const std::set<triangle> &triangles,
					    const const_picture_internal &src,
					    xcb_render_pictformat_t mask,
					    render_pict_op op,
					    coord_t src_x,
					    coord_t src_y)
{
	if (triangles.empty())
		return;

	std::vector<xcb_render_triangle_t> tris;

	tris.reserve(triangles.size());

	for (const auto &t:triangles)
	{
		tris.push_back(xcb_render_triangle_t({to_pointfix(t.p1),
						to_pointfix(t.p2),
						to_pointfix(t.p3)}));
	}

	xcb_render_triangles(picture_conn()->conn,
			     (uint8_t)op,
			     src->picture_id(),
			     picture_id(),
			     mask,
			     coord_t::value_type(src_x),
			     coord_t::value_type(src_y),
			     tris.size(),
			     &tris[0]);
}

void pictureObj::implObj::fill_tri_fan(const point *points,
				       size_t n_points,
				       const const_picture_internal &src,
				       render_pict_op op,
				       coord_t src_x,
				       coord_t src_y)
{
	do_fill_tri_fan(points, n_points, src, XCB_NONE, op, src_x, src_y);
}

void pictureObj::implObj::fill_tri_fan(const point *points,
				       size_t n_points,
				       const const_picture_internal &src,
				       const const_pictformat &mask,
				       render_pict_op op,
				       coord_t src_x,
				       coord_t src_y)
{
	do_fill_tri_fan(points, n_points, src, mask->impl->id, op,
			src_x, src_y);
}

void pictureObj::implObj::do_fill_tri_fan(const point *points,
					  size_t n_points,
					  const const_picture_internal &src,
					  xcb_render_pictformat_t mask,
					  render_pict_op op,
					  coord_t src_x,
					  coord_t src_y)
{
	if (n_points <= 0)
		return;

	xcb_render_pointfix_t fan[n_points];

	for (size_t i=0; i<n_points; ++i)
		fan[i]=xcb_render_pointfix_t(to_pointfix(points[i]));

	xcb_render_tri_fan(picture_conn()->conn,
			   (uint8_t)op,
			   src->picture_id(),
			   picture_id(),
			   mask,
			   coord_t::value_type(src_x),
			   coord_t::value_type(src_y),
			   n_points,
			   &fan[0]);
}

////////////////////////////////////////////////////////////////////////////
//

pictureObj::implObj::fromDrawableObj
::fromDrawableObj(const connection_thread &thread,
		  xcb_drawable_t drawable,
		  xcb_render_pictformat_t format)
	: implObj(thread)
{
	xcb_render_create_picture(picture_conn()->conn,
				  picture_id(),
				  drawable,
				  format,
				  0,
				  nullptr);
}

pictureObj::implObj::fromDrawableObj::~fromDrawableObj()
{
	xcb_render_free_picture(picture_conn()->conn, picture_id());
}

///////////////////////////////////////////////////////////////////////////

// Quick and dirty conversion of an rgb_gradient tuples into two RENDER-
// friendly arrays: namely a list of FIXED stops and COLORs
//
// The callback function gets invoked with three parameters: pointers to
// xcb_render_fixed_t and xcb_render_color_t buffers, and the number of
// values in both buffers.
//
// The callback is guaranteed to be invoked, n is guaranteed to be at least 2,
// the first xcb_render_fixed_t is guaranteed to be 0, and the last one us
// guaranteed to be 1. In the cases where the input is garbage, we just make
// things up.

typedef void normalized_gradient_callback_t(const xcb_render_fixed_t *,
					    const xcb_render_color_t *,
					    size_t);

static void
do_gradient_normalize(const rgb_gradient &gradient,
		      const function<normalized_gradient_callback_t> &callback)
{
	auto n=gradient.size();

	if (n == 0)
	{
		// First GIGO edge case:

		xcb_render_fixed_t stops[2]={0, to_fixed({1,0})};
		xcb_render_color_t colors[2]={to_color({}), to_color({})};

		callback(stops, colors, 2);
		return;
	}

	// Second edge case. The returned buffer should be at least 2.

	std::pair<rgb_gradient::key_type, rgb_gradient::mapped_type>
		gradients[n == 1 ? 2:n];

	xcb_render_fixed_t stops[n == 1 ? 2:n];
	xcb_render_color_t colors[n == 1 ? 2:n];

	std::copy(gradient.begin(), gradient.end(), gradients);

	if (n == 1)
	{
		gradients[1]=gradients[0];
		++n;
	}

	// The first order of business is to sort the array by the size_t.

	std::sort(gradients, gradients+n,
		  []
		  (const auto &a, const auto &b)
		  {
			  return a.first < b.first;
		  });

	gradients[0].first=0;

	if (gradients[n-1].first == 0)
		gradients[n-1].first=1;

	double one=gradients[n-1].first;

	uint16_t last=0; // The previous fraction of the FIXEDPREC stop.

	size_t j=0; // Output index into the stops+colors array.

	size_t i;

	// Note that, above, we ensured that n is at least 2, and
	// gradients[0].first is always 0.

	for (i=0; i+1<n; ++i)
	{
		pictureObj::fixedprec
			fp{0, (uint16_t)std::floor((gradients[i].first/one)
						   * (uint16_t)-1)};

		if (i > 0)
		{
			// Since the previous fraction was "last", fp.fraction
			// must be more than that.
			//
			// When we end up scaling multiple large size_t values
			// to the same FIXEDPREC fraction, what we will end up
			// doing is incrementing, by one, each individual
			// value.
			//
			// But, if we reached the uint16_t limit, we have no
			// choice but to drop the last set of values.
			//
			// Note that we are not iterating until the last value,
			// so we will always emit the 1 value, below.

			if (last == (uint16_t)~0)
				continue;

			if (fp.fraction <= last)
			{
				fp.fraction=last;
				++fp.fraction;
			}
		}

		// Make sure the FIXEDPREC fraction is monotonously increasing.
		last=fp.fraction;

		stops[j]=to_fixed(fp);
		colors[j]=to_color(gradients[i].second);
		++j;
	}

	// The last one is always at position 1.0

	pictureObj::fixedprec v{1,0};

	stops[j]=to_fixed(v);
	colors[j]=to_color(gradients[i].second);
	++j;

	callback(stops, colors, j);
}

template<typename functor>
static void gradient_normalize(const rgb_gradient &gradient,
			       functor &&f)
{
	do_gradient_normalize(gradient,
			      make_function<normalized_gradient_callback_t>
			      (std::forward<functor>(f)));
}

namespace {
#if 0
}
#endif

//! Create a linear gradient picture

class LIBCXX_HIDDEN linearGradientPictureImplObj : public pictureObj::implObj {

 public:
	linearGradientPictureImplObj(const connection_thread &thread,
				     const rgb_gradient &gradient,
				     const pictureObj::point &p1,
				     const pictureObj::point &p2)
		: pictureObj::implObj(thread)
	{
		gradient_normalize(gradient,
				   [&, this]
				   (auto stops,
				    auto colors,
				    size_t n)
				   {
					   xcb_render_create_linear_gradient
						   (picture_conn()->conn,
						    picture_id(),
						    to_pointfix(p1),
						    to_pointfix(p2),
						    n,
						    stops, colors);
				   });
	}

	~linearGradientPictureImplObj()
	{
		xcb_render_free_picture(picture_conn()->conn, picture_id());
	}
};

#if 0
{
#endif
}

size_t screen_picturecacheObj::linear_gradient_cache_key_t_hash
::operator()(const linear_gradient_cache_key_t &k) const noexcept
{
	return std::hash<rgb_gradient>::operator()(k.g)
		+ coord_t::truncate(k.x1)
		+ coord_t::truncate(k.x2)
		+ coord_t::truncate(k.y1)
		+ coord_t::truncate(k.y2)
		+ static_cast<size_t>(k.repeat);
}

bool screen_picturecacheObj::linear_gradient_cache_key_t
::operator==(const linear_gradient_cache_key_t &o) const noexcept
{
	return x1 == o.x1 && y1 == o.y1 && x2 == o.x2 && y2 == o.y2 &&
		repeat == o.repeat && g == o.g;
}

const_picture screenObj::implObj
::create_linear_gradient_picture(const linear_gradient &lg,
				 dim_t w,
				 dim_t h,
				 render_repeat repeat)
{
	auto default_color=valid_gradient(lg.gradient);

	// logical coordinate (1.0, 1.0) is the bottom-right
	// pixel address, or (w-1, h-1).

	if (w == 0 || h == 0)
	{
		return create_solid_color_picture(default_color);
	}

	--w;
	--h;
	coord_t x1{coord_t::truncate(std::trunc(lg.x1 * dim_t::truncate(w)))};
	coord_t x2{coord_t::truncate(std::trunc(lg.x2 * dim_t::truncate(w)))};

	coord_t y1{coord_t::truncate(std::trunc(lg.y1 * dim_t::truncate(h)))};
	coord_t y2{coord_t::truncate(std::trunc(lg.y2 * dim_t::truncate(h)))};

	return picturecache->linear_gradients->find_or_create
		({lg.gradient, x1, y1, x2, y2, repeat},
		 [&, this]
		 {
			 auto picture_impl=ref<linearGradientPictureImplObj>
				 ::create
				 (this->thread, lg.gradient,
				  pictureObj::point{x1, y1},
				  pictureObj::point{x2, y2});

			 picture_impl->repeat(repeat);

			 return picture::create(picture_impl);
		 });
}

rgb valid_gradient(const rgb_gradient &gradient)
{
	auto iter=gradient.find(0);

	if (iter == gradient.end())
		throw EXCEPTION(_("Invalid gradient specification"));
	return iter->second;
}

LIBCXXW_NAMESPACE_END
