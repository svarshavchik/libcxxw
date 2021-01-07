/*
** Copyright 2017-2021 Double Precision, Inc.
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

inline static constexpr xcb_render_pointfix_t
to_pointfix(const pictureObj::point &p)
{
	return { p.x.value, p.y.value };
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
			      const rectarea &rectangles)
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
	rectarea s={ r };

	set_clip_rectangles(s, x, y);
}

void pictureObj::implObj::set_clip_rectangles(const rectarea &clipregion,
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
	typedef pictureObj::fixedprec fixedprec;

	auto n=gradient.size();

	if (n == 0)
	{
		// First GIGO edge case:

		xcb_render_fixed_t stops[2]={0, fixedprec({1,0}).value};
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

	fixedprec last; // The previous fraction of the FIXEDPREC stop.

	size_t j=0; // Output index into the stops+colors array.

	size_t i;

	// Note that, above, we ensured that n is at least 2, and
	// gradients[0].first is always 0.

	for (i=0; i+1<n; ++i)
	{
		fixedprec fp{0, (uint16_t)std::floor((gradients[i].first/one)
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

			if (last == fixedprec{0, (uint16_t)~0})
				continue;

			if (fp <= last)
			{
				fp=last;
				++fp.value;
			}
		}

		// Make sure the FIXEDPREC fraction is monotonously increasing.
		last=fp;

		stops[j]=fp.value;
		colors[j]=to_color(gradients[i].second);
		++j;
	}

	// The last one is always at position 1.0

	stops[j]=fixedprec{1,0}.value;
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
	typedef std::hash<picture::base::fixedprec> fixedprec_hash;

	return std::hash<rgb_gradient>::operator()(k.g)
		+ fixedprec_hash::operator()(k.x1)
		+ fixedprec_hash::operator()(k.x2)
		+ fixedprec_hash::operator()(k.y1)
		+ fixedprec_hash::operator()(k.y2)
		+ static_cast<size_t>(k.repeat);
}

bool screen_picturecacheObj::linear_gradient_cache_key_t
::operator==(const linear_gradient_cache_key_t &o) const noexcept
{
	return x1 == o.x1 && y1 == o.y1 && x2 == o.x2 && y2 == o.y2 &&
		repeat == o.repeat && g == o.g;
}

// Scale a gradient coordinate

// Code shared by create_linear_gradient_picture() and
// create_radial_gradient_picture(). Both of them have a pair of gradient
// coordinates, specified as (x, y) duples between 0.0 and 1.0. Scale this
// to fixedprec x,y; for a [w, h] big display element, and then offset
// the coordinates by offset_x/offset_y.
//
// offset_x/offset_y are the coordinates of the display element in its
// window, so the gradient focal point ends up reflecting window coordinates.

static std::tuple<picture::base::fixedprec,
		  picture::base::fixedprec>
scale_gradient_coordinates(double gx, double gy,
			   coord_t offset_x,
			   coord_t offset_y,
			   dim_t w,
			   dim_t h)
{
	// If width and height is 5, for example we need to scale lg.[xy][12]
	// from 0 to 4.9999... using the fixedprec type. So, we convert
	// convert width and height to fixedprec type, then decrement the
	// value, to get the bottom-right coordinate.

	picture::base::fixedprec x1{xcoord_t::truncate(w)},
		y1{xcoord_t::truncate(h)};

	--x1.value;
	--y1.value;

	// Now, we can scale each value by lg.[xy][12], add the offset, and
	// truncate it.

	picture::base::fixedprec offset_xfp{offset_x}, offset_yfp{offset_y};

	x1.value=x1.truncate(x1.value * gx + offset_xfp.value);
	y1.value=y1.truncate(y1.value * gy + offset_yfp.value);

	return {x1, y1};
}

const_picture screenObj::implObj
::create_linear_gradient_picture(const linear_gradient &lg,
				 coord_t offset_x,
				 coord_t offset_y,
				 dim_t w,
				 dim_t h,
				 render_repeat repeat)
{
	auto default_color=valid_gradient(lg.gradient);

	// logical coordinate (1.0, 1.0) is the bottom-right
	// pixel address, expressed as fixedprec. If the given width or
	// height is 0, that's an edge case, punt.

	if (w == 0 || h == 0)
	{
		return create_solid_color_picture(default_color);
	}

	auto [x1, y1]=scale_gradient_coordinates(lg.x1, lg.y1,
						 offset_x,
						 offset_y,
						 w, h);

	auto [x2, y2]=scale_gradient_coordinates(lg.x2, lg.y2,
						 offset_x,
						 offset_y,
						 w, h);

	return picturecache->linear_gradients->find_or_create
		({lg.gradient, x1, y1, x2, y2, repeat},
		 [&, this]
		 {
			 auto picture_impl=ref<linearGradientPictureImplObj>
				 ::create(this->thread, lg.gradient,
					  picture::base::point{x1, y1},
					  picture::base::point{x2, y2});

			 picture_impl->repeat(repeat);

			 return picture::create(picture_impl);
		 });
}

namespace {
#if 0
}
#endif

//! Create a radial gradient picture

class LIBCXX_HIDDEN radialGradientPictureImplObj : public pictureObj::implObj {

 public:
	radialGradientPictureImplObj(const connection_thread &thread,
				     const rgb_gradient &gradient,
				     const pictureObj::point &inner,
				     const pictureObj::point &outer,
				     const pictureObj::fixedprec &inner_radius,
				     const pictureObj::fixedprec &outer_radius)
		: pictureObj::implObj(thread)
	{
		gradient_normalize(gradient,
				   [&, this]
				   (auto stops,
				    auto colors,
				    size_t n)
				   {
					   xcb_render_create_radial_gradient
						   (picture_conn()->conn,
						    picture_id(),
						    to_pointfix(inner),
						    to_pointfix(outer),
						    inner_radius.value,
						    outer_radius.value,
						    n,
						    stops, colors);
				   });
	}

	~radialGradientPictureImplObj()
	{
		xcb_render_free_picture(picture_conn()->conn, picture_id());
	}
};
#if 0
{
#endif
};

const_picture screenObj::implObj
::create_radial_gradient_picture(const radial_gradient &g,
				 coord_t offset_x,
				 coord_t offset_y,
				 dim_t w,
				 dim_t h,
				 render_repeat repeat)
{
	auto default_color=valid_gradient(g.gradient);

	// logical coordinate (1.0, 1.0) is the bottom-right
	// pixel address, expressed as fixedprec. If the given width or
	// height is 0, that's an edge case, punt.

	if (w == 0 || h == 0)
	{
		return create_solid_color_picture(default_color);
	}

	auto [inner_center_x, inner_center_y]=
		scale_gradient_coordinates(g.inner_center_x, g.inner_center_y,
					   offset_x,
					   offset_y,
					   w, h);

	auto [outer_center_x, outer_center_y]=
		scale_gradient_coordinates(g.outer_center_x, g.outer_center_y,
					   offset_x,
					   offset_y,
					   w, h);

	picture::base::fixedprec
		inner_radius{g.inner_radius *
			dim_t::truncate(g.inner_radius_axis == g.horizontal ||
					(g.inner_radius_axis == g.shortest &&
					 w<h) ||
					(g.inner_radius_axis == g.longest &&
					 w>h)
					? w:h)};
	picture::base::fixedprec
		outer_radius{g.outer_radius *
			dim_t::truncate(g.outer_radius_axis == g.horizontal ||
					(g.outer_radius_axis == g.shortest &&
					 w<h) ||
					(g.outer_radius_axis == g.longest &&
					 w>h) ? w:h)};

	if (inner_radius.value < 0)
		inner_radius.value=0;

	if (outer_radius.value < 0)
		outer_radius.value=0;

	// The inner circle must be entirely enclosed by the outer circle.
	//
	// Automatically adjust the radii to enforce this requirement.
	//
	// Start by computing the distance between the center points.

	double delta_x=inner_center_x.distance(outer_center_x);
	double delta_y=inner_center_y.distance(outer_center_y);

	// delta_[xy] are already in fixedprec precision.

	// Therefore, the result of the pythagorean theorem goes directly
	// into distance.value.
	//
	// Round off using trunc().

	picture::base::fixedprec distance;

	distance.value=number<decltype(distance.value), void>
		::truncate(std::trunc
			   (std::sqrt(delta_x * delta_x + delta_y * delta_y)));

	// We want to overestimate the distance, so bump it by 1, to account
	// for rounding off.

	if (distance.value < std::numeric_limits<decltype(distance.value)>
	    ::max())
		++distance.value;

	// Now, the outer radius must be at least as big enough as the distance
	// between the center points, in order to enclose the inner circle.

	if (outer_radius < distance)
		outer_radius=distance;

	// The difference between the distance and the outer circle's radius
	// is now the upper limit on the inner circle's radius.

	picture::base::fixedprec max_inner_radius;

	max_inner_radius.value=outer_radius.value - distance.value;

	// Enforce it.

	if (inner_radius > max_inner_radius)
		inner_radius=max_inner_radius;

	return picturecache->radial_gradients->find_or_create
		({g.gradient, inner_center_x, inner_center_y,
				outer_center_x, outer_center_y,
				inner_radius, outer_radius,
				repeat},
		 [&, this]
		 {
			 auto picture_impl=ref<radialGradientPictureImplObj>
				 ::create(this->thread, g.gradient,
					  picture::base::point{inner_center_x,
							  inner_center_y},
					  picture::base::point{outer_center_x,
							  outer_center_y},
					  inner_radius, outer_radius);

			 picture_impl->repeat(repeat);

			 return picture::create(picture_impl);
		 });
}

size_t screen_picturecacheObj::radial_gradient_cache_key_t_hash
::operator()(const radial_gradient_cache_key_t &k) const noexcept
{
	typedef std::hash<picture::base::fixedprec> fixedprec_hash;

	return std::hash<rgb_gradient>::operator()(k.g)
		+ fixedprec_hash::operator()(k.inner_x)
		+ fixedprec_hash::operator()(k.inner_y)
		+ fixedprec_hash::operator()(k.outer_x)
		+ fixedprec_hash::operator()(k.outer_y)
		+ fixedprec_hash::operator()(k.inner_radius)
		+ fixedprec_hash::operator()(k.outer_radius)
		+ static_cast<size_t>(k.repeat);
}

bool screen_picturecacheObj::radial_gradient_cache_key_t
::operator==(const radial_gradient_cache_key_t &o) const noexcept
{
	return inner_x == o.inner_x &&
		inner_y == o.inner_y &&
		outer_x == o.outer_x &&
		outer_y == o.outer_y &&
		inner_radius == o.inner_radius &&
		outer_radius == o.outer_radius &&
		repeat == o.repeat && g == o.g;
}

rgb valid_gradient(const rgb_gradient &gradient)
{
	auto iter=gradient.find(0);

	if (iter == gradient.end())
		throw EXCEPTION(_("Invalid gradient specification"));
	return iter->second;
}

LIBCXXW_NAMESPACE_END
