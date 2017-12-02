/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "sxg/sxg_parser.H"
#include <x/xml/doc.H>
#include <x/exception.H>
#include <x/chrcasecmp.H>
#include <x/sentry.H>
#include <x/vector.H>
#include "x/w/gc.H"
#include "x/w/pixmap.H"
#include "x/w/pictformat.H"
#include "picture.H"
#include "pixmap.H"
#include "screen.H"
#include "defaulttheme.H"
#include <courier-unicode.h>
#include <sstream>
#include <cmath>

LIBCXXW_NAMESPACE_START

sxg_parserObj::sxg_operationObj::sxg_operationObj()=default;

sxg_parserObj::sxg_operationObj::~sxg_operationObj()=default;

sxg_parserObj::gc_operationObj::gc_operationObj()=default;

sxg_parserObj::gc_operationObj::~gc_operationObj()=default;

sxg_parserObj::render_operationObj::render_operationObj()=default;

sxg_parserObj::render_operationObj::~render_operationObj()=default;

//////////////////////////////////////////////////////////////////////////
//
// Type erasure: implement sxg_operationObj in terms of a functor.

template<typename functor_type>
class sxg_parserObj::sxg_operationObj::implObj : public sxg_operationObj {

public:
	//! Wrapped functor
	functor_type functor;

	//! Constructor
	template<typename argType>
	implObj(argType &&arg)
		: functor(std::forward<argType>(arg))
	{
	}

	//! Destructor
	~implObj()=default;

	//! Forward execute() to a functor.
	void execute(execution_info &info) override
	{
		functor(info);
	}
};

template<typename functor_type>
inline ref<sxg_parserObj::sxg_operationObj>
sxg_parserObj::make_execute(functor_type &&functor)
{
	return ref<sxg_operationObj::implObj
		   <typename std::decay<functor_type>::type>>
		::create(std::forward<functor_type>(functor));
}

//////////////////////////////////////////////////////////////////////////
//
// Type erasure: implement gc_operationObj in terms of a functor.

template<typename functor_type>
class sxg_parserObj::gc_operationObj::implObj : public gc_operationObj {

public:
	//! Wrapped functor
	functor_type functor;

	//! Constructor
	template<typename argType>
	implObj(argType &&arg)
		: functor(std::forward<argType>(arg))
	{
	}

	//! Destructor
	~implObj()=default;

	//! Forward execute_gc() to a functor.
	void execute_gc(const gc_execute_info &info) override
	{
		functor(info);
	}
};

template<typename functor_type>
inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::make_execute_gc(functor_type &&functor)
{
	return ref<gc_operationObj::implObj
		   <typename std::decay<functor_type>::type>>
		::create(std::forward<functor_type>(functor));
}

//////////////////////////////////////////////////////////////////////////
//
// Type erasure: implement render_operationObj in terms of a functor.

template<typename functor_type>
class sxg_parserObj::render_operationObj::implObj : public render_operationObj {

public:
	//! Wrapped functor
	functor_type functor;

	//! Constructor
	template<typename argType>
	implObj(argType &&arg)
		: functor(std::forward<argType>(arg))
	{
	}

	//! Destructor
	~implObj()=default;

	//! Forward execute_render() to a functor.
	void execute_render(const render_execute_info &info) override
	{
		functor(info);
	}
};

//! Helper method for constructing a subclass of render_operationObj that forwards execute_render() to a functor.

template<typename functor_type>
inline ref<sxg_parserObj::render_operationObj>
sxg_parserObj::make_execute_render(functor_type &&functor)
{
	return ref<render_operationObj::implObj
		   <typename std::decay<functor_type>::type>>
		::create(std::forward<functor_type>(functor));
}

sxg_parserObj::scale_info::scale_info(const size_type_t &sizeArg,
				      double scale_wArg,
				      double scale_hArg,
				      double pixels_per_mm_wArg,
				      double pixels_per_mm_hArg)
	: size(sizeArg),
	  scale_w(scale_wArg),
	  scale_h(scale_hArg),
	  pixels_per_mm_w(pixels_per_mm_wArg),
	  pixels_per_mm_h(pixels_per_mm_hArg),
	  offset_x(0),
	  offset_y(0)
{
	// Precompute the location of the center point.
	//
	// For a default-scaled image, scale_[wh] is how we're scaling the
	// SXG image, so if it's 2x2, a single pixel in the virtual SXG
	// cartesian space becomes a 2x2 pixel in the scaled image.
	//
	// For a fixed image, there is no scaling, so the center pixel is
	// always (0, 0)
	//
	// For 'mm', the virtual SXG coordinate of N is multiplied by
	// pixels-per-mm to compute the coordinate in the scaled image.
	// For 'mmrounded', the pixels-per-mm is additionally multiplied
	// by the factor and rounded off.

	switch (size.type) {
	case size_type_t::scaled:
		offset_x=dim_t::truncate(std::floor(scale_w/2));
		offset_y=dim_t::truncate(std::floor(scale_h/2));
		break;
	case size_type_t::mmrounded:
		pixels_per_mm_w=std::round(pixels_per_mm_w * size.factor);
		pixels_per_mm_h=std::round(pixels_per_mm_h * size.factor);

		if (pixels_per_mm_w == 0)
			++pixels_per_mm_w;

		if (pixels_per_mm_h == 0)
			++pixels_per_mm_h;
		// FALLTHRU
	case size_type_t::mm:
		offset_x=dim_t::value_type(std::floor(pixels_per_mm_w/2));
		offset_y=dim_t::value_type(std::floor(pixels_per_mm_w/2));
		break;
	default:
		break;
	}
}

coord_t sxg_parserObj::scale_info::x_pixel(const xy_t &v) const
{
	return x_pixel(v, v.mode);
}

coord_t sxg_parserObj::scale_info::x_pixel(const xy_t &v, mode_t mode) const
{
	// For the ending-oriented pixel, we compute the "beginning" of the
	// next one, and subtract 1.

	if (mode == ending)
		return x_pixel(v+1, beginning)-1;

	if (mode == width && centered_width != 0)
		return coord_t::truncate(x_pixel(v, beginning) + centered_width/2);

	// At this point we're dealing with either a beginning or a centered
	// orientation.

	dim_t use_centered=mode == centered || mode == width ? 1:0;

	// For scaled sizing, we using the scaling factor, and adjust it to
	// the location of the center pixel, if so.
	//
	// For millimeter-based scaling, we multiply by pixels-per-mm.
	//
	// For fixed scaling, there is no scaling in either case.

	switch (size.type) {
	case size_type_t::scaled:
		return coord_t::truncate
			(coord_t::truncate(std::round(v.value * scale_w))
			 + offset_x * use_centered);
	case size_type_t::mm:
	case size_type_t::mmrounded:
		return coord_t::truncate
			(coord_t::truncate(std::round(v.value * pixels_per_mm_w)
					   ) + offset_x * use_centered);
	default:
		return coord_t::truncate(v.value);
	}
}

coord_t sxg_parserObj::scale_info::y_pixel(const xy_t &v) const
{
	return y_pixel(v, v.mode);
}

coord_t sxg_parserObj::scale_info::y_pixel(const xy_t &v, mode_t mode) const
{
	// For the ending-oriented pixel, we compute the "beginning" of the
	// next one, and subtract 1.

	if (mode == ending)
		return y_pixel(v+1, beginning)-1;

	if (mode == width && centered_width != 0)
		return coord_t::truncate(y_pixel(v, beginning)
					 + centered_width/2);

	// At this point we're dealing with either a beginning or a centered
	// orientation.

	dim_t use_centered=mode == centered || mode == width ? 1:0;

	// For scaled sizing, we using the scaling factor, and adjust it to
	// the location of the center pixel, if so.
	//
	// For millimeter-based scaling, we multiply by pixels-per-mm.
	//
	// For fixed scaling, there is no scaling in either case.

	switch (size.type) {
	case size_type_t::scaled:
		return coord_t::truncate
			(coord_t::truncate(std::round(v.value * scale_h))
			 + offset_y * use_centered);
	case size_type_t::mm:
	case size_type_t::mmrounded:
		return coord_t::truncate
			(coord_t::truncate(std::round(v.value * pixels_per_mm_h)
					   ) + offset_y * use_centered);
	default:
		return coord_t::truncate(v.value);
	}
}

// This is used to scale some value that represents a size, of some kind.
coord_t sxg_parserObj::scale_info::xy_pixel(double value) const
{
	switch (size.type) {
	case size_type_t::scaled:
		return coord_t::truncate
			(std::round(value * (scale_h+scale_w)/2));
	case size_type_t::mm:
	case size_type_t::mmrounded:
		return coord_t::truncate
			(std::round(value * (pixels_per_mm_h+pixels_per_mm_w)/2)
			 /2);
	default:
		return coord_t::truncate(value);
	}
}

sxg_parserObj::gc_execute_info
::gc_execute_info(gc::base::properties &propsArg,
		  const gc &contextArg,
		  const picture &pArg,
		  const scale_info &scaleArg,
		  execution_info &infoArg)
	: props(propsArg), context(contextArg), p(pArg), scale(scaleArg),
	  info(infoArg)
{
}

sxg_parserObj::render_execute_info
::render_execute_info(const picture &dest_pictureArg,
		      const scale_info &scaleArg,
		      execution_info &infoArg)
	: dest_picture(dest_pictureArg),
	  scale(scaleArg),
	  info(infoArg)
{
}

sxg_parserObj::execution_info
::execution_info(const sxg_parserObj &sxg_parser_refArg,
		 dim_t w, dim_t h)
	: sxg_parser_ref(sxg_parser_refArg)
{
		// Compute scaling factors

	scale_w=(double)dim_t::value_type(w)/
		dim_t::value_type(sxg_parser_ref.nominal_width);
	scale_h=(double)dim_t::value_type(h)/
		dim_t::value_type(sxg_parser_ref.nominal_height);

	pixels_per_mm_w=sxg_parser_ref.pixels_per_mm_w();

	pixels_per_mm_h=sxg_parser_ref.pixels_per_mm_h();
}

const_picture sxg_parserObj::execution_info::source_picture(const std::string &n)
	const
{
	// Search for a source picture either in "pictures", which are
	// pixmap-backed pictures, or const_pictures (solid color pictures).

	auto iter1=pictures.find(n);

	if (iter1 != pictures.end())
		return std::get<picture>(iter1->second);

	auto iter2=const_pictures.find(n);

	if (iter2 == const_pictures.end())
		throw EXCEPTION("source picture " << n << " not found");

	return iter2->second;
}

sxg_parserObj::scale_info
sxg_parserObj::execution_info::source_scale(const std::string &n)	const
{
	auto iter=sxg_parser_ref.pictures.find(n);

	// This identifier must be refering to either something we'll find
	// in pixmaps, or pictures. Figure out which one.

	if (iter == sxg_parser_ref.pictures.end())
	{
		auto iter=sxg_parser_ref.pixmaps.find(n);

		if (iter == sxg_parser_ref.pixmaps.end())
			throw EXCEPTION("source picture " << n << " not found");

		return scale_info(iter->second.size,
				  scale_w,
				  scale_h,
				  pixels_per_mm_w,
				  pixels_per_mm_h);
	}

	auto source_size=iter->second.size;

	switch (iter->second.type) {
	case picture_type_t::pixmap:
		return scale_info(source_size,
				  scale_w,
				  scale_h,
				  pixels_per_mm_w,
				  pixels_per_mm_h);

	case picture_type_t::text:

		{
			auto p=std::get<drawable>(dest_picture(n));

			auto s=scale_info(source_size,
					  (dim_t::value_type)
					  p->get_width(),
					  (dim_t::value_type)
					  p->get_height(),
					  1, 1);
			s.offset_x=0;
			s.offset_y=0;
			return s;
		}

	case picture_type_t::solid_color:
		break;
	}

	return scale_info(size_type_t::fixed, 1, 1, 1, 1);
}

const sxg_parserObj::execution_info::pixmap_picture_t &
sxg_parserObj::execution_info::dest_picture(const std::string &n) const
{
	auto iter1=pictures.find(n);

	if (iter1 == pictures.end())
		throw EXCEPTION("dest picture " << n << " not found");

	return iter1->second;
}

sxg_parserObj::execution_info::gc_info::gc_info(const std::string &pixmap_nameArg,
						const gc &contextArg,
						const picture &pArg)
	: pixmap_name(pixmap_nameArg),
	  context(contextArg),
	  p(pArg)
{
}

///////////////////////////////////////////////////////////////////////////////

// Retrieve the text of some element.
//
// "attr_name" is the name of an element. The element anchored at "parent"
// must have exactly one inner attr_name element, which is extracted into
// "value".
//
// If "attr_name" starts with a space, the attr_name is ignored, and
// "parent"'s contents are extracted into "value"; but an extraction error
// still results in a thrown exception, referencing the "attr_name".
//
// validator() is an optional lambda that's called to validate the extracted
// value. If the validation fails an error gets thrown.

template<typename value_type, typename validator_type>
static void get_value(const xml::doc::base::readlock &parent,
		      const char *attr_name,
		      value_type &value,
		      validator_type && validator)
{
	auto lock=parent->clone();

	if (*attr_name && *attr_name != ' ')
	{
		auto xpath=lock->get_xpath(attr_name);

		size_t count=xpath->count();

		if (count != 1)
			throw EXCEPTION("missing <" << attr_name << ">");

		xpath->to_node(1);
	}

	if (*attr_name)
		++attr_name;

	std::istringstream is(lock->get_text());

	if ((is >> value).fail() || !is.eof() || !validator(value))
		throw EXCEPTION("Corrupted value of <"
				<< attr_name << ">");
}

// Determine if element "lock" has an inner element of the given name.

static bool has_value(const xml::doc::base::readlock &lock,
		      const char *attr_name)
{
	auto xpath=lock->get_xpath(attr_name);

	return xpath->count() > 0;
}

// Wrapper for get_value() when no additional validation is required.
// Supplies a validator that always returns true.

template<typename value_type>
static void get_value(const xml::doc::base::readlock &parent,
		      const char *attr_name,
		      value_type &value)
{
	get_value(parent, attr_name, value,
		  []
		  (const value_type &v)
		  {
			  return true;
		  });
}

// Parse <width> and <height> elements.

template<typename value_type>
void sxg_parserObj::get_width_height(const xml::doc::base::readlock &parent,
				     points_t &points,
				     const char *width_elem,
				     const char *height_elem,
				     const char *size_elem,
				     value_type &width, value_type &height)
{
	if (has_value(parent, size_elem))
	{
		std::string d;

		get_value(parent, size_elem, d);

		auto iter=points.dimensions.find(d);

		if (iter == points.dimensions.end())
			throw EXCEPTION("Undefined dimension: " << d);

		width=dim_t::truncate(iter->second.first);
		height=dim_t::truncate(iter->second.second);
		return;
	}

	get_value(parent, width_elem, width);

	get_value(parent, height_elem, height);
}

template<typename value_type>
void sxg_parserObj::get_width_height(const xml::doc::base::readlock &parent,
				     points_t &points,
				     value_type &width, value_type &height)
{
	get_width_height(parent, points, "width", "height", "dimension",
			 width, height);
}

//////////////////////////////////////////////////////////////////////
//
// Adjust rectangular coordinates, after scaling them from virtual SXG
// coordinates to scaled SXG coordinates.
//
// Handle negative width or height: the (x,y) coordinates simply specify
// the right or the bottom coordinate. Add the negative dimension to the
// coordinate, to compute the starting coordinate; and then make the
// dimension positive. This normalizes the (x,y) coordinate to the top/left
// corner, with the width and height made positive.
//
// Additionally, if after scaling the width or height becomes 0, adjust it to
// 1.

template<typename dim_type,
	 typename orig_dim_type>
static void adjust_x_y_width_height(coord_t &rx, coord_t &ry,
				    dim_type &rw, dim_type &rh,
				    orig_dim_type width,
				    orig_dim_type height)
{

	// Adjust 0 width or height. Adjust it to 1 or -1, depending on the
	// sign of the original dimension.

	if (rw == 0)
		rw += width > 0 ? 1:-1;

	if (rh == 0)
		rh += height > 0 ? 1:-1;

	// And now fix the negative dimension accordingly.
	if (rw < 0)
	{
		rx = coord_t::truncate(rx + rw);
		rw = -rw;
	}

	if (rh < 0)
	{
		ry = coord_t::truncate(ry + rh);
		rh = -rh;
	}
}

// Specialization for scaled width or height expressed as dim_t which
// is an unsigned value

template<typename orig_dim_type>
static void adjust_x_y_width_height(coord_t &rx, coord_t &ry,
				    dim_t &rw, dim_t &rh,
				    orig_dim_type width,
				    orig_dim_type height)
{
	if (rw == 0)
	{
		rw=1;

		if (width < 0)
			rx=coord_t::truncate(rx-1);
	}

	if (rh == 0)
	{
		rh=1;

		if (height < 0)
			ry=coord_t::truncate(ry-1);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// Parse miscellenaeous values in the SXG file.

static halign get_halign(const std::string &n)
{
	if (n == "left")
		return halign::left;

	if (n == "right")
		return halign::right;

	if (n == "centered")
		return halign::center;

	throw EXCEPTION("\"" << n << "\" is not a valid horizontal alignment");
}

static valign get_valign(const std::string &n)
{
	if (n == "top")
		return valign::top;

	if (n == "bottom")
		return valign::bottom;

	if (n == "middle")
		return valign::middle;

	throw EXCEPTION("\"" << n << "\" is not a valid vertical alignment");
}

static halign get_halign(const xml::doc::base::readlock &parent,
			 halign default_value,
			 const char *element)
{
	if (has_value(parent, element))
	{
		std::string s;

		get_value(parent, element, s);

		default_value=get_halign(s);
	}
	return default_value;
}

static valign get_valign(const xml::doc::base::readlock &parent,
			 valign default_value,
			 const char *element)
{
	if (has_value(parent, element))
	{
		std::string s;

		get_value(parent, element, s);

		default_value=get_valign(s);
	}
	return default_value;
}

// Whether the given XML element contains a location specification.

static bool has_xy(const xml::doc::base::readlock &lock)
{
	return has_value(lock, "x") || has_value(lock, "y")
		|| has_value(lock, "location");
}

// Obtain the location specification for the given XML element.

void sxg_parserObj::get_xy_value(const xml::doc::base::readlock &parent,
				 const char *name,
				 xy_t &xy)
{
	auto lock=parent->clone();

	auto xpath=lock->get_xpath(name);

	size_t count=xpath->count();

	if (count != 1)
		throw EXCEPTION(std::string("missing <") << name << ">");

	xpath->to_node(1);

	std::string attr_name=" ";

	attr_name += name;

	get_value(lock, attr_name.c_str(), xy.value);

	std::string orientation=lock->get_any_attribute("orientation");

	if (!orientation.empty())
	{
		if (orientation == "centered")
			xy.mode=scale_info::mode_t::centered;
		else if (orientation == "width")
			xy.mode=scale_info::mode_t::width;
		else if (orientation == "beginning")
			xy.mode=scale_info::mode_t::beginning;
		else if (orientation == "ending")
			xy.mode=scale_info::mode_t::ending;
		else throw EXCEPTION("Unknown @orientation value");
	}
}

void sxg_parserObj::get_xy(const xml::doc::base::readlock &lock,
			   points_t &points,
			   xy_t &x,
			   xy_t &y)
{
	if (has_value(lock, "location"))
	{
		std::string l;

		get_value(lock, "location", l);

		auto iter=points.points.find(l);

		if (iter == points.points.end())
			throw EXCEPTION("Undefined location: " << l);

		x=iter->second.first;
		y=iter->second.second;
		return;
	}

	get_xy_value(lock, "x", x);
	get_xy_value(lock, "y", y);
}

bool sxg_parserObj::optional_xy(const xml::doc::base::readlock &render_element,
				points_t &points,
				xy_t &x,
				xy_t &y)
{
	if (has_xy(render_element))
	{
		get_xy(render_element, points, x, y);
		return true;
	}
	return false;
}

// A parsed rectangle

// The rectangle's coordinates get parsed in the constructor. When the
// command that uses this rectangle specification gets executed, scale()
// computes the scaled rectangle's dimensions and location.

struct LIBCXX_HIDDEN sxg_parserObj::sxg_rectangle {

 public:

	xy_t x, y;

	double width, height;

	sxg_rectangle(const xml::doc::base::readlock &parent,
		      points_t &points)
	{
		get_xy(parent, points, x, y);
		get_width_height(parent, points, width, height);
	}

	void scale(const scale_info &scale, rectangle_set &r) const
	{
		convert_to(scale,
			   [&]
			   (const auto &rx,
			    const auto &ry,
			    const auto &rw,
			    const auto &rh)
			   {
				   r.insert({rx, ry, rw, rh});
			   });
	}

	void scale(const scale_info &scale, rectangle &r) const
	{
		convert_to(scale,
			   [&]
			   (const auto &rx,
			    const auto &ry,
			    const auto &rw,
			    const auto &rh)
			   {
				   r={rx, ry, rw, rh};
			   });
	}

	// convert_to: helper the performs the actual calculations and
	// passes the results to a callback.

	template<typename functor>
		void convert_to(const scale_info &scale,
				functor &&f) const
	{
		do_convert_to(scale,
			      make_function<void (coord_t, coord_t,
						  dim_t, dim_t)>
			      (std::forward<functor>(f)));
	}

	void do_convert_to(const scale_info &scale,
			   const function<void (coord_t, coord_t,
						dim_t, dim_t)> &f) const
	{
		if (width == 0 || height == 0)
			return;

		coord_t rx=scale.x_pixel(x, scale_info::beginning);
		coord_t ry=scale.y_pixel(y, scale_info::beginning);
		coord_t rw=coord_t::truncate
			(scale.x_pixel(x+width,
				       scale_info::beginning)-rx);
		coord_t rh=coord_t::truncate
			(scale.y_pixel(y+height,
				       scale_info::beginning)-ry);

		if (rw == 0)
			rw += width > 0 ? 1:-1;

		if (rh == 0)
			rh += height > 0 ? 1:-1;

		if (rw < 0)
		{
			rx = coord_t::truncate(rx + rw);
			rw = coord_t::truncate(-rw);
		}

		if (rh < 0)
		{
			ry = coord_t::truncate(ry + rh);
			rh = coord_t::truncate(-rh);
		}
		f(rx, ry, dim_t::truncate(rw), dim_t::truncate(rh));
	}
};

// root element specifies the width and height of the sxg canvas.

inline bool sxg_parserObj::parse_root(const xml::doc::base::readlock &root)
{
	auto lock=root->clone();

	auto xpath=lock->get_xpath("/sxg");

	size_t count=xpath->count();

	if (count != 1)
		return false;

	xpath->to_node(1);

	get_width_height(lock, points, "width", "height",
			 "dimensionsize",
			 nominal_width, nominal_height);

	nominal_depth=0;

	if (has_value(lock, "depth"))
		get_value(lock, "depth",
			  nominal_depth, [](auto v) { return v >= 0; });

	get_width_height(lock, points, "widthmm", "heightmm", "dimensionmm",
			 nominal_width_mm, nominal_height_mm);

	if (has_value(lock, "widthfactor") ||
	    has_value(lock, "heightfactor"))
		get_width_height(lock, points,
				 "widthfactor", "heightfactor",
				 "dimensionfactor",
				 widthfactor, heightfactor);

	if (nominal_width <= 0 || nominal_height <= 0 ||
	    nominal_width_mm <= 0 || nominal_height_mm <= 0 ||
	    widthfactor <= 0 || heightfactor <= 0)
		throw EXCEPTION("SXG file dimensions zero or negative");


	return true;
}

// Parse a <size> element, somewhere

sxg_parserObj::size_type_t
sxg_parserObj::parse_size_type_t(const xml::doc::base::readlock &parent)
{
	auto lock=parent->clone();

	auto xpath=lock->get_xpath("size");

	size_t count=xpath->count();

	if (count == 0)
		return sxg_parserObj::size_type_t::scaled;

	if (count > 1)
		throw EXCEPTION("multiple <size> elements for the same object.");

	xpath->to_node(1);

	std::string s;
	double factor=1;

	if (has_value(lock, "type"))
	{
		get_value(lock, "type", s);

		if (has_value(lock, "factor"))
			get_value(lock, "factor", factor,
				  []
				  (double f)
				  {
					  return f > 0;
				  });
	}
	else
	{
		s=lock->get_text();
	}

	if (s == "scaled")
		return sxg_parserObj::size_type_t::scaled;

	if (s == "mm")
		return sxg_parserObj::size_type_t::mm;

	if (s == "mmrounded")
	{
		return sxg_parserObj::size_type_t
			(sxg_parserObj::size_type_t::mmrounded, factor);
	}

	if (s == "fixed")
		return sxg_parserObj::size_type_t::fixed;

	throw EXCEPTION("invalid <size> element.");
}

// Go through and parse all <pixmap> elements.

inline void sxg_parserObj::parse_pixmaps(const xml::doc::base::readlock &root)
{
	std::unordered_set<std::string> gc_ids;

	auto lock=root->clone();

	auto xpath=lock->get_xpath("/sxg/pixmap");

	size_t count=xpath->count();

	for (size_t i=1; i <= count; ++i)
	{
		xpath->to_node(i);

		auto text=lock->get_any_attribute("id");

		if (text.empty() || pixmaps.count(text)
		    || pictures.count(text))
			throw EXCEPTION("missing or duplicate <pixmap> @id: "
					+text);

		try {
			sxg_parserObj::pixmap_info new_pixmap;

			if (has_value(lock, "mask"))
				new_pixmap.depth=1;

			new_pixmap.size=parse_size_type_t(lock);

			get_width_height(lock, points, new_pixmap.width,
					 new_pixmap.height);

			if (new_pixmap.width <= 0 || new_pixmap.height <= 0)
				throw EXCEPTION("<pixmap> width or height is"
						" zero or negative");

			// Enumerated list of <gc> elements.

			auto gc_xpath=lock->get_xpath("gc");

			size_t gc_count=gc_xpath->count();

			for (size_t j=1; j <= gc_count; ++j)
			{
				gc_xpath->to_node(j);

				auto text=lock->get_any_attribute("id");

				if (text.empty())
					throw EXCEPTION("<gc> @id attribute not"
							" found");

				if (gc_ids.count(text) > 0)
					throw EXCEPTION("duplicate <gc> id: "
							<< text);

				new_pixmap.graphic_contexts.insert(text);
				gc_ids.insert(text);
			}

			pixmaps.insert({text, new_pixmap});
		} catch (const exception &e)
		{
			throw EXCEPTION("<pixmap id='"
					<< text << "'>: " << e);
		}
	}
}

// Parse attributes that specify a color. This can appear in a text <picture>
// (default color), a solid color picture (the main attraction), and a RENDER
// <fill> element.

sxg_parserObj::color_info::color_info(const xml::doc::base::readlock &lock)
{
	if (has_value(lock, "color"))
	{
		get_value(lock, "color", theme_color);
		red=green=blue=alpha=1;
	}

	static double color_info::* const channels[4]={
		&color_info::red,
		&color_info::green,
		&color_info::blue,
		&color_info::alpha,
	};

	static const char * channel_names[4]={
		"r",
		"g",
		"b",
		"a"
	};

	for (int i=0; i<4; ++i)
	{
		if (has_value(lock, channel_names[i]))
			get_value(lock, channel_names[i],
				  (this->*channels[i]),
				  []
				  (double v)
				  {
					  return v >= 0;
				  });
	}
}

rgb sxg_parserObj::color_info::get_color(const defaulttheme &theme) const
{
	if (theme_color.empty())
	{
		return {
			rgb_component_t(rgb::maximum * red),
			rgb_component_t(rgb::maximum * green),
			rgb_component_t(rgb::maximum * blue),
			rgb_component_t(rgb::maximum * alpha)
				};
	}

	auto color=theme->get_theme_color(theme_color);

	auto red_value=color.r * red;
	auto green_value=color.g * green;
	auto blue_value=color.b * blue;
	auto alpha_value=color.a * alpha;

	if (red_value > rgb::maximum)
		red_value=rgb::maximum;
	if (green_value > rgb::maximum)
		green_value=rgb::maximum;
	if (blue_value > rgb::maximum)
		blue_value=rgb::maximum;
	if (alpha_value > rgb::maximum)
		alpha_value=rgb::maximum;

	return rgb{rgb_component_t(red_value),
			rgb_component_t(green_value),
			rgb_component_t(blue_value),
			rgb_component_t(alpha_value)};
}

// Parse all <font> elements. Hijack theme code to do all the work for us.

inline void sxg_parserObj::parse_fonts(const xml::doc::base::readlock &root)
{
	auto lock=root->clone();

	auto xpath=lock->get_xpath("/sxg/font");

	theme->load_fonts(lock, xpath,
			  [&, this]
			  (const std::string &id, const auto &new_font)
			  {
				  fonts.insert({id, new_font});
			  },
			  [&, this]
			  (const std::string &from,
			   auto &new_font)
			  {
				  auto iter=fonts.find(from);

				  if (iter == fonts.end())
					  return false;

				  new_font=iter->second;
				  return true;
			  });
}

// Parse a text <picture>

inline void sxg_parserObj
::parse_text_picture(const xml::doc::base::readlock &lock,
		     picture_info &info)
{
	std::string font="label_font";
	sxg_parserObj::color_info color(lock); // Temporary

	if (lock->get_first_element_child())
		do
		{
			auto name=lock->name();

			if (name == "font")
			{
				font=lock->get_text();
				continue;
			}

			if (name == "rgb")
			{
				color=sxg_parserObj::color_info(lock);
				continue;
			}

			if (name == "align")
			{
				auto n=lock->get_text();

				info.align=get_halign(n);
			}
			if (name != "text")
				continue;

			std::string s=lock->get_text();

			info.text_info
				.emplace_back(font, color,
					      unicode::iconvert::tou
					      ::convert(s,
							unicode::utf_8).first);
		} while (lock->get_next_element_sibling());
}

// Parse all the <picture> elements.

inline void sxg_parserObj::parse_pictures(const xml::doc::base::readlock &root)
{
	auto lock=root->clone();

	auto xpath=lock->get_xpath("/sxg/picture");

	size_t count=xpath->count();

	for (size_t i=1; i <= count; ++i)
	{
		xpath->to_node(i);

		auto text=lock->get_any_attribute("id");

		if (text.empty() || pictures.count(text) ||
		    pixmaps.count(text))
			throw EXCEPTION("missing or duplicate <picture> @id: "
					<< text);


		try {
			sxg_parserObj::picture_info new_picture;

			// Figure out the picture type.

			sxg_parserObj::picture_type_t picture_type=
				sxg_parserObj::picture_type_t::pixmap;

			{
				auto type=lock->clone();

				auto type_xpath=type->get_xpath("type");

				if (type_xpath->count())
				{
					if (type_xpath->count() > 1)
						throw EXCEPTION("pixmap id "
								<< text
								<< ": multiple <type>"
								" elements");
					type_xpath->to_node(1);

					auto type_text=type->get_text();

					if (type_text == "pixmap")
						picture_type=
							sxg_parserObj::picture_type_t::pixmap;
					else if (type_text == "rgb")
						picture_type=
							sxg_parserObj::picture_type_t
							::solid_color;
					else if (type_text == "text")
						picture_type=
							sxg_parserObj::picture_type_t
							::text;
					else throw EXCEPTION("Unknown value for "
							     "<type>: " + type_text);
				}
			}

			new_picture.type=picture_type;

			// Parse elements appropriate to the picture type.

			auto &new_pic_iter=
				pictures.insert({text, new_picture})
				.first->second;

			switch (picture_type) {
			case sxg_parserObj::picture_type_t::pixmap:
				new_pic_iter.size=parse_size_type_t(lock);
				get_width_height(lock, points,
						 new_pic_iter.width,
						 new_pic_iter.height);

				if (new_pic_iter.width <= 0
				    || new_pic_iter.height <= 0)
					throw EXCEPTION("<picture> width or height is "
							"zero or negative");
				break;
			case sxg_parserObj::picture_type_t::solid_color:
				new_pic_iter.color=sxg_parserObj::color_info(lock);
				break;

			case sxg_parserObj::picture_type_t::text:
				parse_text_picture(lock, new_pic_iter);
				break;
			}
		} catch (const exception &e)
		{
			std::ostringstream o;

			o << "<picture id='"
			  << text << "'>: " << e;

			throw EXCEPTION(o.str());
		}
	}
}

struct LIBCXX_HIDDEN sxg_parserObj::parse_render_gc_info {

	bool in_clip_mask=false;
};

///////////////////////////////////////////////////////////////////////////////
//
// <gc> elements.
//
// The parsing function return a gc_operationObj, representing the parsed
// form of the element, whose execute_gc() gets invoked to execute the GC
// operation when drawing the SXG picture.

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_function(const xml::doc::base::readlock &lock)
{
	auto v=gc::base::function_from_string(lock->get_text());

	return make_execute_gc
		([v]
		 (const gc_execute_info &info)
		 {
			 info.props.function(v);
		 });
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_foreground(const xml::doc::base::readlock &lock)
{
	auto text=lock->get_text();

	double i;

	std::istringstream ii(text);

	if ((ii >> i).fail() || !ii.eof() || (i < 0 && i > 1))
		throw EXCEPTION("Empty or invalid <foreground> value");

	return make_execute_gc
		([i]
		 (const gc_execute_info &info)
		 {
			 info.props.foreground(i *
					       ((1 << (depth_t::value_type)
						 info.context
						 ->gc_drawable
						 ->get_depth())-1));
		 });
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_background(const xml::doc::base::readlock &lock)
{
	auto text=lock->get_text();

	double i;

	std::istringstream ii(text);

	if ((ii >> i).fail() || !ii.eof() || (i < 0 && i > 1))
		throw EXCEPTION("Empty or invalid <foreground> value");

	return make_execute_gc
		([i]
		 (const gc_execute_info &info)
		 {
			 info.props.background(i *
					       ((1 << (depth_t::value_type)
						 info.context
						 ->gc_drawable
						 ->get_depth())-1));
		 });
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_fill_arc_mode(const xml::doc::base::readlock &lock)
{
	auto v=gc::base::fill_arc_mode_from_string(lock->get_text());

	return make_execute_gc
		([v]
		 (const gc_execute_info &info)
		 {
			 info.props.arc_mode(v);
		 });
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_line_width(const xml::doc::base::readlock &lock)
{
	bool flag=false;

	auto text=lock->get_any_attribute("slim");

	if (!text.empty() && *text.c_str() == '1')
		flag=true;

	double w=0;

	get_value(lock, " line_width", w, [](double v) { return v >= 0; });

	return make_execute_gc
		([w, flag]
		 (const gc_execute_info &info)
		 {
			 uint32_t xy=0;

			 if (w > 0)
			 {
				 xy=(coord_t::value_type)info.scale.xy_pixel(w);

				 // A non-0 line width can't scale to an
				 // actual 0 line width.

				 if (xy == 0)
					 xy=1;
			 }

			 // ... unless there's a slim flag

			 if (xy == 1 && flag)
				 xy=0;

			 info.props.line_width(xy);
		 });
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_line_style(const xml::doc::base::readlock &lock)
{
	auto v=gc::base::line_style_from_string(lock->get_text());

	return make_execute_gc
		([v]
		 (const gc_execute_info &info)
		 {
			 info.props.line_style(v);
		 });
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_cap_style(const xml::doc::base::readlock &lock)
{
	auto v=gc::base::cap_style_from_string(lock->get_text());

	return make_execute_gc
		([v]
		 (const gc_execute_info &info)
		 {
			 info.props.cap_style(v);
		 });
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_join_style(const xml::doc::base::readlock &lock)
{
	auto v=gc::base::join_style_from_string(lock->get_text());

	return make_execute_gc
		([v]
		 (const gc_execute_info &info)
		 {
			 info.props.join_style(v);
		 });
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_fill(const xml::doc::base::readlock &lock)
{
	xy_t x, y;
	double width, height;

	get_xy(lock, points, x, y);
	get_width_height(lock, points, width, height);

	return make_execute_gc
		([x, y, width, height]
		 (const gc_execute_info &info)
		 {
			 if (width == 0 || height == 0)
				 return;

			 auto x1=info.scale.x_pixel(x,
						    scale_info::beginning);
			 auto rw=dim_t::truncate
				 (info.scale.x_pixel(x+width,
						     scale_info::beginning)-x1);

			 auto y1=info.scale.y_pixel(y,
						    scale_info::beginning);
			 auto rh=dim_t::truncate
				 (info.scale.y_pixel(y+height,
						     scale_info::beginning)-y1);

			 adjust_x_y_width_height(x1, y1,
						 rw, rh,
						 width, height);

			 info.context->fill_rectangle(x1, y1,
						      rw,
						      rh,
						      info.props);
		 });
}

ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_clear(const xml::doc::base::readlock &lock)
{
	return make_execute_gc
		([]
		 (const gc_execute_info &info)
		 {
			 auto drawable=info.context->gc_drawable;

			 info.context->fill_rectangle(0, 0,
						      drawable->get_width(),
						      drawable->get_height(),
						      info.props);
		 });
}

vector<sxg_parserObj::sxg_rectangle>
sxg_parserObj::parse_rectangles(const xml::doc::base::readlock &lock)
{
	auto clone=lock->clone();

	auto xpath=clone->get_xpath("rectangle");

	size_t n=xpath->count();

	auto v=vector<sxg_rectangle>::create();

	v->reserve(n);

	for (size_t i=0; i < n; ++i)
	{
		xpath->to_node(i+1);

		v->emplace_back(clone, points);
	}
	return v;
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_clip(const xml::doc::base::readlock &lock,
			     const std::string &id,
			     const parse_render_gc_info &info)
{
	if (info.in_clip_mask)
		throw EXCEPTION("Nested <clip> elements not allowed.");

	xy_t x, y;
	std::string pixmap;

	// The <clip> can specify either a pixmap ...

	if (has_value(lock, "pixmap"))
		get_value(lock, "pixmap", pixmap);

	auto has_xy=optional_xy(lock, points, x, y);

	auto rectangles=parse_rectangles(lock); // ... or rectangles.

	if (pixmap.empty() && rectangles->size() == 0)
		throw EXCEPTION("Either <pixmap> or <rectangle> must be specified for a <clip>");

	auto clone=lock->clone();

	// Recursively parse_gc() the elements in <clipped>

	auto xpath=clone->get_xpath("clipped");

	size_t n=xpath->count();

	if (n != 1)
		throw EXCEPTION("Missing <clipped> inside <clip>");

	xpath->to_node(1);

	auto nested=parse_gc(clone, id, {true});

	return make_execute_gc
		([=]
		 (const gc_execute_info &info)
		 {
			 auto pixmap_iter=info.info.pixmaps.find(pixmap);

			 auto xs=info.scale.x_pixel(x);
			 auto ys=info.scale.y_pixel(y);

			 if (!has_xy)
				 xs=ys=0;

			 auto sentry=make_sentry([&]
						 {
							 info.props.clipmask();
						 });

			 sentry.guard();

			 // If <pixmap> was not specified, use clipping
			 // rectangles.

			 if (pixmap_iter == info.info.pixmaps.end())
			 {
				 rectangle_set convrectangles;

				 size_t n=rectangles->size();

				 for (size_t i=0; i<n; ++i)
					 rectangles->at(i)
						 .scale(info.scale,
							convrectangles);

				 info.props.clipmask(convrectangles,
						     dim_t::truncate(xs),
						     dim_t::truncate(ys));

			 }
			 else
			 {
				 info.props.clipmask(pixmap_iter->second,
						     dim_t::truncate(xs),
						     dim_t::truncate(ys));
			 }

			 nested->execute(info.info);
		 });
}

// Parse <fill_style>

bool sxg_parserObj::get_fill_style_pixmap(const xml::doc::base::readlock &lock,
					  points_t &points,
					  const char *name,
					  std::string &pixmap,
					  xy_t &x,
					  xy_t &y)
{
	get_value(lock, name, pixmap);

	return optional_xy(lock, points, x, y);
}

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_fill_style(const xml::doc::base::readlock &lock)
{
	std::string pixmap;

	xy_t x, y;
	bool has_xy=false;

	gc::base::properties &(gc::base::properties::*fill_style)
		(const ref<pixmapObj> &, coord_t, coord_t)=nullptr;

	if (has_value(lock, "tiled"))
	{
		has_xy=get_fill_style_pixmap(lock, points, "tiled", pixmap,
					     x, y);
		fill_style= &gc::base::properties::fill_style_tiled;
	} else if (has_value(lock, "stippled"))
	{
		has_xy=get_fill_style_pixmap(lock, points, "stippled", pixmap,
					     x, y);
		fill_style= &gc::base::properties::fill_style_stippled;
	} else if (has_value(lock, "opaque_stippled"))
	{
		has_xy=get_fill_style_pixmap(lock, points, "opaque_stippled",
					     pixmap,
					     x, y);
		fill_style= &gc::base::properties::fill_style_opaque_stippled;
	}

	return make_execute_gc
		([=]
		 (const gc_execute_info &info)
		 {
			 if (pixmap.empty())
			 {
				 info.props.fill_style_solid();
				 return;
			 }

			 auto pixmap_iter=info.info.pixmaps.find(pixmap);

			 auto xs=info.scale.x_pixel(x);
			 auto ys=info.scale.y_pixel(y);

			 if (!has_xy)
				 xs=ys=0;

			 if (pixmap_iter == info.info.pixmaps.end())
				 throw EXCEPTION("<pixmap> " +
						 pixmap
						 + " was not defined");
			 (info.props.*fill_style)(pixmap_iter->second, xs, ys);
		 });
}

// Parse <dashes>

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_dashes(const xml::doc::base::readlock &lock)
{
	auto clone=lock->clone();

	auto xpath=clone->get_xpath("dash");

	std::vector<double> dashes;

	size_t n=xpath->count();

	dashes.reserve(n);

	for (size_t i=1; i <= n; ++i)
	{
		xpath->to_node(i);

		double v;

		get_value(clone, " dash", v);

		dashes.push_back(v);
	}

	return make_execute_gc
		([dashes]
		 (const gc_execute_info &info)
		 {
			 info.props.dashes.clear();
			 info.props.dashes.reserve(dashes.size());

			 for (double dash:dashes)
			 {
				 auto s=info.scale.xy_pixel(dash);

				 if (s == 0)
					 s=1;

				 if (s > 255)
					 s=255;
				 info.props.dashes
					 .push_back(dim_t::truncate(s));
			 }

		 });
}

// A wrapper for a parsed point.

// A point specification gets parsed by the constructor. When the command
// that uses the point gets executed, get() or get_fixedprec() computes the
// point's scaled coordinate.

class LIBCXX_HIDDEN sxg_parserObj::sxg_point {
 public:

	xy_t x, y;

	sxg_point(const xml::doc::base::readlock &lock,
		  points_t &points)
	{
		get_xy(lock, points, x, y);
	}


	template<typename point_type>
		void get(const scale_info &scale, point_type &t) const
	{
		t.x=scale.x_pixel(x);
		t.y=scale.y_pixel(y);
	}

	void get_fixedprec(const scale_info &scale,
			   picture::base::point &fp) const
	{
		auto sx=scale.x_pixel(x);
		auto sy=scale.y_pixel(y);

		fp.x.fraction=0;
		fp.y.fraction=0;

		fp.x.integer=coord_t::truncate(sx);
		fp.y.integer=coord_t::truncate(sy);
	}
};

// Parse <line>.

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_line(const xml::doc::base::readlock &lock)
{
	auto clone=lock->clone();

	gc::base::polyfill fill_type=gc::base::polyfill::none;

	if (has_value(clone, "fill"))
	{
		std::string fill_type_str;

		get_value(clone, "fill", fill_type_str);

		if (fill_type_str.empty() ||
		    fill_type_str == "complex")
		{
			fill_type=gc::base::polyfill::complex;
		}
		else if (fill_type_str == "nonconvex")
		{
			fill_type=gc::base::polyfill::nonconvex;
		}
		else if (fill_type_str == "convex")
		{
			fill_type=gc::base::polyfill::convex;
		}
		else throw EXCEPTION("Invalid <fill> value");
	}

	auto xpath=clone->get_xpath("point");

	std::vector<sxg_point> my_points;

	size_t n=xpath->count();

	my_points.reserve(n);

	for (size_t i=1; i <= n; ++i)
	{
		xpath->to_node(i);

		my_points.emplace_back(clone, points);
	}

	return make_execute_gc
		([my_points, fill_type]
		 (const gc_execute_info &info)
		 {
			 if (my_points.size() <= 1)
				 return;

			 gc::base::polyline line[my_points.size()];

			 size_t i=0;

			 auto scale=info.scale;

			 scale.centered_width=info.props.line_width();

			 for (const auto &p:my_points)
			 {
				 p.get(scale, line[i]);
				 ++i;
			 }

			 info.props.dashes_offset=0;
			 info.context->lines(&line[0], i, info.props,
					     fill_type);
		 });
}

// Parsed arc specification.

struct LIBCXX_HIDDEN sxg_parserObj::sxg_arc_info {
	xy_t x;
	xy_t y;
	double width;
	double height;
	int16_t angle1;
	int16_t angle2;
};

// Parse <arcs>

inline ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_arcs(const xml::doc::base::readlock &lock)
{
	auto clone=lock->clone();

	bool fill_flag=has_value(clone, "fill");

	auto xpath=clone->get_xpath("arc");

	std::vector<sxg_arc_info> arc_info;

	size_t n=xpath->count();

	arc_info.reserve(n);

	for (size_t i=1; i <= n; ++i)
	{
		xpath->to_node(i);

		sxg_arc_info info;

		get_xy(clone, points, info.x, info.y);

		get_width_height(clone, points, info.width, info.height);

		double angle;

		get_value(clone, "angle1", angle);
		info.angle1=std::round(angle*64);

		get_value(clone, "angle2", angle);
		info.angle2=std::round(angle*64);

		arc_info.push_back(info);
	}

	return make_execute_gc
		([=]
		 (const gc_execute_info &info)
		 {
			 if (arc_info.empty())
				 return;

			 gc::base::arc arcs[arc_info.size()];
			 size_t i=0;

			 for (const auto &arc:arc_info)
			 {
				 auto &gc_arc=arcs[i];

				 gc_arc.x=info.scale
					 .x_pixel(arc.x);
				 gc_arc.y=info.scale
					 .y_pixel(arc.y);

				 auto w=dim_t::truncate
					 (info.scale
					  .x_pixel(arc.x+arc.width)
					  - gc_arc.x);

				 auto h=dim_t::truncate
					 (info.scale
					  .y_pixel(arc.y+arc.height)
					  - gc_arc.y);

				 adjust_x_y_width_height(gc_arc.x,
							 gc_arc.y,
							 w, h,
							 arc.width,
							 arc.height);

				 if (w == 0 || h == 0)
					 continue;

				 /*
				   The X protocol appears to specify the
				   bounding rectangle inclusively. An arc
				   bound by a rectangle 50x50 pixels gets
				   specified with a width/height of 49/49.
				   Make this adjustment.
				  */

				 gc_arc.width=w-1;
				 gc_arc.height=h-1;

				 gc_arc.angle1=arc.angle1;
				 gc_arc.angle2=arc.angle2;
				 ++i;
			 }

			 if (fill_flag)
				 info.context->fill_arcs(arcs, i, info.props);
			 else
				 info.context->draw_arcs(arcs, i, info.props);
		 });
}

// Parse a single <gc> operation.

ref<sxg_parserObj::gc_operationObj>
sxg_parserObj::parse_gc_op(const xml::doc::base::readlock &lock,
			   const std::string &id,
			   const parse_render_gc_info &info)
{
	auto name=lock->name();

	try {
		if (name == "function")
			return parse_gc_function(lock);

		if (name == "foreground")
			return parse_gc_foreground(lock);

		if (name == "background")
			return parse_gc_background(lock);

		if (name == "fill_arc_mode")
			return parse_gc_fill_arc_mode(lock);

		if (name == "line_width")
			return parse_gc_line_width(lock);

		if (name == "dashes")
			return parse_gc_dashes(lock);

		if (name == "line_style")
			return parse_gc_line_style(lock);

		if (name == "join_style")
			return parse_gc_join_style(lock);

		if (name == "cap_style")
			return parse_gc_cap_style(lock);

		if (name == "fill")
			return parse_gc_fill(lock);

		if (name == "clear")
			return parse_gc_clear(lock);

		if (name == "fill_style")
			return parse_gc_fill_style(lock);

		if (name == "line")
			return parse_gc_line(lock);

		if (name == "arcs")
			return parse_gc_arcs(lock);

		if (name == "clip")
			return parse_gc_clip(lock, id, info);
	} catch (const exception &e)
	{
		std::ostringstream o;

		o << "<" << name << ">: " << e;
		throw EXCEPTION(o.str());
	}
	throw EXCEPTION("unknown <gc> element: <" + name + ">");
}

// Parse graphic context property initialization.

inline ref<sxg_parserObj::sxg_operationObj>
sxg_parserObj::parse_gc(const xml::doc::base::readlock &lock)
{
	auto id=lock->get_any_attribute("id");

	if (id.empty())
		throw EXCEPTION("@id attribute not found for <gc>");

	return parse_gc(lock, id, parse_render_gc_info());
}

ref<sxg_parserObj::sxg_operationObj>
sxg_parserObj::parse_gc(const xml::doc::base::readlock &old_lock,
			const std::string &id,
			const parse_render_gc_info &info)
{
	auto lock=old_lock->clone();

	// Parse the <gc> list of instructions now...

	std::vector<ref<gc_operationObj>> instructions;

	instructions.reserve(lock->get_child_element_count());

	if (lock->get_first_element_child())
		do
		{
			instructions.push_back(parse_gc_op(lock, id, info));
		} while (lock->get_next_element_sibling());

	// Then, we'll execute them by:
	return make_execute
		([id, instructions]
		 (execution_info &info)
		 {
			 // Finding the property object
			 auto gc_iter=info.gcs.find(id);

			 if (gc_iter == info.gcs.end())
				 throw EXCEPTION("<gc> " +
						 id + " was not defined");

			 // And the pixmap, to figure out the scaling to use.
			 auto pixmap_iter=
				 info.sxg_parser_ref.pixmaps
				 .find(gc_iter->second.pixmap_name);

			 if (pixmap_iter == info.sxg_parser_ref.pixmaps.end())
				 throw EXCEPTION("<pixmap> " +
						 gc_iter->second.pixmap_name
						 + " was not defined");

			 scale_info scale(pixmap_iter->second.size,
					  info.scale_w,
					  info.scale_h,
					  info.pixels_per_mm_w,
					  info.pixels_per_mm_h);

			 gc_execute_info init(gc_iter->second.properties,
					      gc_iter->second.context,
					      gc_iter->second.p,
					      scale,
					      info);

			 for (const auto &instruction:instructions)
				 instruction->execute_gc(init);
		 });
}

//////////////////////////////////////////////////////////////////////////////
//
// <render> elements

// Parse a <src> with a <picture>, or a <mask> with a <picture>. in a
// <composite>

bool sxg_parserObj::parse_src_mask(const xml::doc::base::readlock &render_element,
				   points_t &points,
				   const char *what,
				   const char *pic_or_pixmap,
				   std::string &picture,
				   xy_t &x,
				   xy_t &y)
{
	auto p=render_element->clone();

	auto xpath=p->get_xpath(what);

	if (xpath->count() != 1)
		throw EXCEPTION("<" << what <<
				"> is missing in <composite>");

	xpath->to_node(1);

	get_value(p, pic_or_pixmap, picture);
	return optional_xy(p, points, x, y);
}

// Parse a <composite>

inline ref<sxg_parserObj::render_operationObj>
sxg_parserObj::parse_render_composite(const xml::doc::base::readlock &render_element)
{
	std::string op_str;

	get_value(render_element, "op", op_str);

	auto op=picture::base::render_pict_op_from_string(op_str);

	xy_t x, y;
	double width=0, height=0;
	bool srcsize=false;

	bool has_xy=optional_xy(render_element, points, x, y);

	if (has_value(render_element, "srcsize"))
		srcsize=true;
	else
		get_width_height(render_element, points, width, height);

	halign halign_value=get_halign(render_element, halign::left, "halign");
	valign valign_value=get_valign(render_element, valign::top, "valign");

	std::string src_picture;
	xy_t src_x, src_y;

	bool has_src=parse_src_mask(render_element, points, "src", "picture",
				    src_picture, src_x, src_y);

	std::string mask_picture;
	xy_t mask_x, mask_y;
	bool has_maskxy=false;

	if (has_value(render_element, "mask"))
		has_maskxy=parse_src_mask(render_element, points, "mask",
					  "pixmap",
					  mask_picture, mask_x,
					  mask_y);

	return make_execute_render
		([=]
		 (const render_execute_info &info)
		 {
			 auto src=info.info.source_picture(src_picture);
			 auto src_scale=info.info.source_scale(src_picture);

			 auto x_scaled=
				 info.scale.x_pixel(x, scale_info::beginning);
			 auto y_scaled=
				 info.scale.y_pixel(y, scale_info::beginning);

			 if (!has_xy)
				 x_scaled=y_scaled=0;

			 auto src_x_scaled=
				 src_scale.x_pixel(src_x, scale_info::beginning);
			 auto src_y_scaled=
				 src_scale.y_pixel(src_y, scale_info::beginning);

			 dim_t width_scaled, height_scaled;
			 dim_t width_copy2, height_copy2;

			 if (!has_src)
				 src_x_scaled=src_y_scaled=0;

			 if (srcsize)
			 {
				 auto d=std::get<drawable>
					 (info.info.dest_picture(src_picture));

				 width_scaled=d->get_width();
				 height_scaled=d->get_height();

				 width_copy2=width_scaled;
				 height_copy2=height_scaled;
			 }
			 else
			 {
				 width_scaled=dim_t::truncate
					 (info.scale.x_pixel(x+width,
							     scale_info::beginning)
					  -x_scaled);
				 height_scaled=dim_t::truncate
					 (info.scale.y_pixel(y+height,
							     scale_info::beginning)
					  -y_scaled);

				 auto width_copy=width_scaled;
				 auto height_copy=height_scaled;
				 width_copy2=width_scaled;
				 height_copy2=height_scaled;

				 adjust_x_y_width_height(src_x_scaled,
							 src_y_scaled,
							 width_scaled,
							 height_scaled,
							 width,
							 height);

				 adjust_x_y_width_height(x_scaled,
							 y_scaled,
							 width_copy,
							 height_copy,
							 width,
							 height);

			 }

			 if (halign_value == halign::right)
				 x_scaled = coord_t::truncate
					 (x_scaled - width_scaled);

			 if (halign_value == halign::center)
				 x_scaled = coord_t::truncate
					 (x_scaled - width_scaled/2);

			 if (valign_value == valign::bottom)
				 y_scaled = coord_t::truncate
					 (y_scaled - height_scaled);

			 if (valign_value == valign::middle)
				 y_scaled = coord_t::truncate
					 (y_scaled - height_scaled/2);

			 if (!mask_picture.empty())
			 {
				 auto mask=info.info
					 .source_picture(mask_picture);

				 auto mask_scale=info.info
					 .source_scale(mask_picture);

				 auto mask_x_scaled=
					 mask_scale.x_pixel(mask_x,
							    scale_info::beginning);
				 auto mask_y_scaled=
					 mask_scale.y_pixel(mask_y,
							    scale_info::beginning);

				 if (!has_maskxy)
					 mask_x_scaled=mask_y_scaled=0;

				 adjust_x_y_width_height(mask_x_scaled,
							 mask_y_scaled,
							 width_copy2,
							 height_copy2,
							 width,
							 height);

				 info.dest_picture->composite(src, mask,
							      src_x_scaled,
							      src_y_scaled,
							      mask_x_scaled,
							      mask_y_scaled,
							      x_scaled,
							      y_scaled,
							      width_scaled,
							      height_scaled,
							      op);
			 }
			 else
			 {
				 info.dest_picture->composite(src,
							      src_x_scaled,
							      src_y_scaled,
							      x_scaled,
							      y_scaled,
							      width_scaled,
							      height_scaled,
							      op);
			 }
		 });
}

// Parse a <repeat>

inline ref<sxg_parserObj::render_operationObj>
sxg_parserObj::parse_render_repeat(const xml::doc::base::readlock &render_element)
{
	auto value=picture::base::render_repeat_from_string(render_element
							    ->get_text());

	return make_execute_render
		([=]
		 (const render_execute_info &info)
		 {
			 info.dest_picture->repeat(value);
		 });
}

// Parse a <clip>

inline ref<sxg_parserObj::render_operationObj>
sxg_parserObj::parse_render_clip(const xml::doc::base::readlock &render_element,
				 const std::string &id,
				 const parse_render_gc_info &info)
{
	if (info.in_clip_mask)
		throw EXCEPTION("Nested <clip> elements not allowed.");

	std::string clip_to_pixmap;
	xy_t x_origin, y_origin;

	if (has_value(render_element, "pixmap"))
		get_value(render_element, "pixmap", clip_to_pixmap);

	bool has_xy=optional_xy(render_element, points, x_origin, y_origin);

	auto rectangles=parse_rectangles(render_element);

	auto clone=render_element->clone();

	// Parse the instructions nested inside <clip>

	auto xpath=clone->get_xpath("clipped");

	size_t n=xpath->count();

	if (n != 1)
		throw EXCEPTION("Missing <clipped> inside <clip>");

	xpath->to_node(1);

	auto nested=parse_render(clone, id, { true });

	return make_execute_render
		([=]
		 (const render_execute_info &info)
		 {
			 if (clip_to_pixmap.empty())
			 {
				 rectangle_set convrectangles;
				 size_t n=rectangles->size();

				 for (size_t i=0; i<n; ++i)
					 rectangles->at(i)
						 .scale(info.scale,
							convrectangles);

				 auto xs=info.scale.x_pixel(x_origin);
				 auto ys=info.scale.y_pixel(y_origin);

				 if (!has_xy)
					 xs=ys=0;

				 picture::base::rectangular_clip_mask
					 mask{info.dest_picture,
						 convrectangles, xs, ys};

				 nested->execute(info.info);
				 return;
			 }

			 auto iter=info.info.pixmaps.find(clip_to_pixmap);

			 if (iter == info.info.pixmaps.end())
				 throw EXCEPTION("clip pixmap "
						 + clip_to_pixmap
						 + " was not found");

			 auto pixmap=iter->second;

			 if (pixmap->get_depth() != 1)
				 throw EXCEPTION("clip pixmap "
						 + clip_to_pixmap
						 + " must have a "
						 "<mask /> flag");

			 picture::base::clip_mask
				 clip{info.dest_picture, pixmap,
					 info.scale.x_pixel(x_origin),
					 info.scale.y_pixel(y_origin)};
			 nested->execute(info.info);
		 });
}

static render_pict_op
parse_optional_op(const xml::doc::base::readlock &render_element,
		  render_pict_op default_value)
{
	if (has_value(render_element, "op"))
	{
		std::string op_str;

		get_value(render_element, "op", op_str);

		default_value=picture::base::render_pict_op_from_string(op_str);
	}
	return default_value;
}

// Parse a <fill>

inline ref<sxg_parserObj::render_operationObj>
sxg_parserObj::parse_render_fill(const xml::doc::base::readlock &render_element)
{
	render_pict_op op=parse_optional_op(render_element,
					    render_pict_op::op_src);

	color_info color(render_element);

	auto clone=render_element->clone();

	auto xpath=clone->get_xpath("rectangle");

	size_t n=xpath->count();

	std::vector<sxg_rectangle> rectangles;

	rectangles.reserve(n);

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		rectangles.emplace_back(clone, points);
	}

	return make_execute_render
		([=]
		 (const render_execute_info &info)
		 {
			 rectangle rvec[n];

			 for (size_t i=0; i<n; ++i)
				 rectangles.at(i).scale(info.scale, rvec[i]);

			 info.dest_picture->fill_rectangles(rvec, n,
							    color.get_color
							    (info.info
							     .sxg_parser_ref
							     .theme),
							    op);
		 });
}

// Parsed triangle coordinates.

// The constructor parses three points. scale() scales the points'
// coordinates, when the command that uses thees coordinates gets executed.

struct LIBCXX_HIDDEN sxg_parserObj::sxg_triangle {

 public:

	sxg_point p1, p2, p3;

	// Helper used by the constructor to parse out one of the points.

	static xml::doc::base::readlock
		get_point(const xml::doc::base::readlock &parent,
			  const char *point_name)
	{
		auto clone=parent->clone();

		auto xpath=clone->get_xpath(point_name);

		if (xpath->count() != 1)
			throw EXCEPTION("Duplicate or missing <"
					<< point_name
					<< ">");
		xpath->to_node(1);

		return clone;
	}

	sxg_triangle(const xml::doc::base::readlock &parent,
		     points_t &points)
		: p1(get_point(parent, "p1"), points),
		p2(get_point(parent, "p2"), points),
		p3(get_point(parent, "p3"), points)
		{
		}

	void scale(const scale_info &scale,
		   std::set<picture::base::triangle> &t) const
	{
		picture::base::triangle new_triangle;

		p1.get_fixedprec(scale, new_triangle.p1);
		p2.get_fixedprec(scale, new_triangle.p2);
		p3.get_fixedprec(scale, new_triangle.p3);
		t.insert(new_triangle);
	}
};

// Parse <triangles>

inline ref<sxg_parserObj::render_operationObj>
sxg_parserObj::parse_render_triangles(const xml::doc::base::readlock &render_element)
{
	render_pict_op op=parse_optional_op(render_element,
					    render_pict_op::op_src);
	xy_t x, y;

	bool has_xy=optional_xy(render_element, points, x, y);

	std::string mask;

	if (has_value(render_element, "mask"))
		get_value(render_element, "mask", mask);

	std::string src;

	get_value(render_element, "src", src);

	std::vector<sxg_triangle> triangles;

	auto clone=render_element->clone();

	auto xpath=clone->get_xpath("triangle");

	size_t n=xpath->count();

	triangles.reserve(n);

	for (size_t i=1; i<=n; ++i)
	{
		xpath->to_node(i);

		triangles.emplace_back(clone, points);
	}

	return make_execute_render
		([=]
		 (const render_execute_info &info)
		 {
			 std::set<picture::base::triangle> tries;

			 for (const auto &t:triangles)
				 t.scale(info.scale, tries);

			 auto src_picture=info.info.source_picture(src);
			 auto src_scale=info.info.source_scale(src);

			 auto src_x_scaled=src_scale.x_pixel(x);
			 auto src_y_scaled=src_scale.y_pixel(y);

			 if (!has_xy)
				 src_x_scaled=src_y_scaled=0;

			 if (!mask.empty())
			 {
				 auto mask_picture=std::get<drawable>
					 (info.info.dest_picture(mask));

				 info.dest_picture
					 ->fill_triangles(tries,
							  src_picture,
							  mask_picture
							  ->get_pictformat(),
							  op,
							  src_x_scaled,
							  src_y_scaled);
			 }
			 else
			 {

				 info.dest_picture
					 ->fill_triangles(tries,
							  src_picture,
							  op,
							  src_x_scaled,
							  src_y_scaled);
			 }
		 });
}

// Parse <tri>

ref<sxg_parserObj::render_operationObj>
sxg_parserObj::parse_render_tri(const xml::doc::base::readlock &render_element,
				pic_func_no_mask func_1,
				pic_func_mask func_2)
{
	render_pict_op op=parse_optional_op(render_element,
					    render_pict_op::op_src);
	xy_t x, y;

	bool has_xy=optional_xy(render_element, points, x, y);

	std::string mask;

	if (has_value(render_element, "mask"))
		get_value(render_element, "mask", mask);

	std::string src;

	get_value(render_element, "src", src);

	std::vector<sxg_point> strip;

	auto clone=render_element->clone();

	auto xpath=clone->get_xpath("point");

	size_t n=xpath->count();

	strip.reserve(n);

	for (size_t i=1; i<=n; ++i)
	{
		xpath->to_node(i);

		strip.emplace_back(clone, points);
	}

	return make_execute_render
		([=]
		 (const render_execute_info &info)
		 {
			 if (n == 0)
				 return;

			 picture::base::point pointlist[n];

			 for (size_t i=0; i<n; ++i)
				 strip[i].get_fixedprec(info.scale,
							pointlist[i]);

			 auto src_picture=info.info.source_picture(src);
			 auto src_scale=info.info.source_scale(src);

			 auto src_x_scaled=src_scale.x_pixel(x);
			 auto src_y_scaled=src_scale.y_pixel(y);

			 if (!has_xy)
				 src_x_scaled=src_y_scaled=0;

			 if (!mask.empty())
			 {
				 auto mask_picture=std::get<drawable>
					 (info.info.dest_picture(mask));

				 ((*info.dest_picture).*func_2)
					 (pointlist,
					  n,
					  src_picture,
					  mask_picture->get_pictformat(),
					  op,
					  src_x_scaled,
					  src_y_scaled);
			 }
			 else
			 {
				 ((*info.dest_picture).*func_1)
					 (pointlist,
					  n,
					  src_picture,
					  op,
					  src_x_scaled,
					  src_y_scaled);
			 }
		 });
}

// Parse a single <render> element.

inline ref<sxg_parserObj::render_operationObj>
sxg_parserObj::parse_render_op(const xml::doc::base::readlock &render_element,
			       const std::string &id,
			       const parse_render_gc_info &info)
{
	auto name=render_element->name();

	try {
		if (name == "composite")
			return parse_render_composite(render_element);
		if (name == "repeat")
			return parse_render_repeat(render_element);
		if (name == "clip")
			return parse_render_clip(render_element,
						    id,
						    info);
		if (name == "fill")
			return parse_render_fill(render_element);
		if (name == "triangles")
			return parse_render_triangles(render_element);
		if (name == "tristrip")
			return parse_render_tri(render_element,
						&pictureObj::fill_tri_strip,
						&pictureObj::fill_tri_strip);
		if (name == "trifan")
			return parse_render_tri(render_element,
						&pictureObj::fill_tri_fan,
						&pictureObj::fill_tri_fan);
	} catch (const exception &e)
	{
		std::ostringstream o;

		o << "<" << name << ">: " << e;
		throw EXCEPTION(o.str());
	}

	throw EXCEPTION("unknown <render> element: <" + name + ">");
}

// Parse all <render>s.

inline ref<sxg_parserObj::sxg_operationObj>
sxg_parserObj::parse_render(const xml::doc::base::readlock &render_element)
{
	auto id=render_element->get_any_attribute("id");

	if (id.empty())
		throw EXCEPTION("@id attribute not found for <render>");

	return parse_render(render_element, id, parse_render_gc_info());
}

ref<sxg_parserObj::sxg_operationObj>
sxg_parserObj::parse_render(const xml::doc::base::readlock &render_element,
			    const std::string &id,
			    const parse_render_gc_info &info)
{
	auto lock=render_element->clone();

	// Parse the <render> list of instructions now...

	std::vector<ref<render_operationObj>> instructions;

	instructions.reserve(lock->get_child_element_count());

	if (lock->get_first_element_child())
		do
		{
			instructions.push_back(parse_render_op(lock,
							       id,
							       info));
		} while (lock->get_next_element_sibling());

	// Then, we'll execute them by:
	return make_execute
		([id, instructions]
		 (execution_info &info)
		 {
			 // Find the destination picture object.

			 auto dest_pic_info=info.dest_picture(id);

			 scale_info scale(std::get<size_type_t>
					  (dest_pic_info),
					  info.scale_w,
					  info.scale_h,
					  info.pixels_per_mm_w,
					  info.pixels_per_mm_h);

			 render_execute_info
				 init(std::get<picture>(dest_pic_info),
				      scale, info);

			 for (const auto &instruction:instructions)
				 instruction->execute_render(init);
		 });
}

// Recursively parse all <gc> and <render> elements.

inline void sxg_parserObj::parse_operations(const xml::doc::base::readlock &root)
{
	auto lock=root->clone();

	auto xpath=lock->get_xpath("/sxg");

	if (lock->get_first_element_child())
		do
		{
			auto name=lock->name();

			if (name == "gc")
				operations.push_back(parse_gc(lock));
			else if (name == "render")
				operations.push_back(parse_render(lock));
		} while (lock->get_next_element_sibling());
}

// Parse the <location> and <dimension> top-level elements.

inline void sxg_parserObj::get_points(const xml::doc::base::readlock &root,
				      const char *element)
{
	auto clone=root->clone();

	auto xpath=clone->get_xpath(element);

	size_t n=xpath->count();

	for (size_t i=1; i<=n; ++i)
	{
		xpath->to_node(i);

		auto name=clone->name();

		std::string id=clone->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION("Missing @id in a <" +
					std::string(element) +
					">");

		std::pair<xy_t, xy_t> p;

		get_xy_value(clone, "x", p.first);
		get_xy_value(clone, "y", p.second);

		if (!points.points.insert({id, p}).second)
			throw EXCEPTION("<" + std::string(element)
					+ "> " + id + " redefined");
	}
}

inline void sxg_parserObj::
get_dimensions(const xml::doc::base::readlock &root,
	       const char *element)
{
	auto clone=root->clone();

	auto xpath=clone->get_xpath(element);

	size_t n=xpath->count();

	for (size_t i=1; i<=n; ++i)
	{
		xpath->to_node(i);

		auto name=clone->name();

		std::string id=clone->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION("Missing @id in a <" +
					std::string(element) +
					">");

		std::pair<double, double> p;

		get_value(clone, "width", p.first);
		get_value(clone, "height", p.second);

		if (!points.dimensions.insert({id, p}).second)
			throw EXCEPTION("<" + std::string(element)
					+ "> " + id + " redefined");
	}
}

dim_t sxg_parserObj::depth() const
{
	return nominal_depth;
}

dim_t sxg_parserObj::width_scale_factor() const
{
	return widthfactor;
}

dim_t sxg_parserObj::height_scale_factor() const
{
	return heightfactor;
}

double sxg_parserObj::pixels_per_mm_w() const
{
	auto screen_impl=screenref->impl;

	return (double)dim_t::value_type(screen_impl->width_in_pixels())
		/dim_t::value_type(screen_impl->width_in_millimeters())
		* theme->themescale;
}

double sxg_parserObj::pixels_per_mm_h() const
{
	auto screen_impl=screenref->impl;

	return (double)dim_t::value_type(screen_impl->height_in_pixels())
		/dim_t::value_type(screen_impl->height_in_millimeters())
		* theme->themescale;
}

void sxg_parserObj::render(const picture &p,
			   const drawable &d) const
{
	execution_info info{*this, d->get_width(), d->get_height()};

	// Start by clearing the picture

	p->fill_rectangle({0, 0, d->get_width(), d->get_height()},
			  rgb(0, 0, 0, 0));

	info.pictures.insert({"main",
			{p, d, size_type_t::scaled}});

	// Create all pixmaps

	for (const auto &pixmap:pixmaps)
	{
		auto width=pixmap.second.width;
		auto height=pixmap.second.height;

		scale_info scale(pixmap.second.size,
				 info.scale_w,
				 info.scale_h,
				 info.pixels_per_mm_w,
				 info.pixels_per_mm_h);

		auto p_width=dim_t::truncate(scale.x_pixel(width));
		auto p_height=dim_t::truncate(scale.y_pixel(height));

		auto new_pixmap=
			d->create_pixmap(p_width, p_height,
					 pixmap.second.depth);

		// Clear the rectangle, initially.

		{
			gcObj::properties props;

			props.function(gc::base::function::CLEAR);

			new_pixmap->create_gc()
				->fill_rectangle(0, 0,
						 new_pixmap->get_width(),
						 new_pixmap->get_height(),
						 props);
		}

		info.pixmaps.insert(std::make_pair(pixmap.first, new_pixmap));

		auto p=new_pixmap->create_picture();

		// Create each pixmap's graphic contexts.

		for (const auto &gc_name:pixmap.second.graphic_contexts)
			info.gcs.insert(std::make_pair
					(gc_name,
					 execution_info::gc_info
					 (pixmap.first,
					  new_pixmap->create_gc(), p)));

		info.pictures.insert({pixmap.first,
				{p, new_pixmap, pixmap.second.size}});
	}

	for (const auto &picture_info:pictures)
	{
		switch (picture_info.second.type) {
		default:
			throw EXCEPTION("Unknown picture type");

		case picture_type_t::text:
			{
				auto tp=create_text_picture(picture_info.second,
							    d);

				info.pictures.insert
					({picture_info.first,
						{std::get<picture>(tp),
								std::get
								<pixmap>(tp),
								picture_info
								.second
								.size}});
			}
			break;
		case picture_type_t::solid_color:
			info.const_pictures
				.insert({picture_info.first,
							d->get_screen()->impl
							->create_solid_color_picture
							(picture_info.second.color
							 .get_color(theme))});
			break;
		case picture_type_t::pixmap:

			auto width=picture_info.second.width;
			auto height=picture_info.second.height;

			scale_info scale(picture_info.second.size,
					 info.scale_w,
					 info.scale_h,
					 info.pixels_per_mm_w,
					 info.pixels_per_mm_h);

			auto p_w=dim_t::truncate(scale.x_pixel(width, scale_info::beginning));
			auto p_h=dim_t::truncate(scale.y_pixel(height, scale_info::beginning));

			auto new_pixmap=d->create_pixmap(p_w, p_h);

			auto new_pic=new_pixmap->create_picture();

			new_pic->fill_rectangle({0, 0, p_w, p_h},
						rgb(0, 0, 0, 0));

			info.pictures
				.insert({picture_info.first,
						{new_pic, new_pixmap,
								picture_info
								.second.size}});
			break;
		}
	}

	// Ready to rock-n-roll

	for (const auto &op:operations)
		op->execute(info);
}

pixmap_points_of_interest_t sxg_parserObj::render_points(dim_t w, dim_t h) const
{
	execution_info info{*this, w, h};

	// This will be main picture's scale, when rendered.
	scale_info main_scale(size_type_t::scaled,
			      info.scale_w,
			      info.scale_h,
			      info.pixels_per_mm_w,
			      info.pixels_per_mm_h);

	pixmap_points_of_interest_t points;

	// Return predefined points to caller.

	for (const auto &p:this->points.points)
	{
		auto x_pixel=main_scale.x_pixel(p.second.first);
		auto y_pixel=main_scale.y_pixel(p.second.second);

		points.insert(std::make_pair(p.first,
					     std::make_pair
					     (x_pixel, y_pixel)));
	}

	return points;
}

///////////////////////////////////////////////////////////////////////////////

sxg_parserObj::sxg_parserObj(const std::string &filename,
			     const screen &screenref,
			     const defaulttheme &theme)
	: screenref(screenref), theme(theme)
{
#ifdef SXG_PARSER_CONSTRUCTOR_TEST
	SXG_PARSER_CONSTRUCTOR_TEST();
#endif
	auto config=xml::doc::create(filename, "nonet xinclude");

	auto root=config->readlock();

	if (!root->get_root())
	{
	corrupted:
		throw EXCEPTION("corrupted sxg header");
	}

	get_points(root, "location");
	get_dimensions(root, "dimension");

	if (!parse_root(root))
		goto corrupted;

	parse_pixmaps(root);
	parse_fonts(root);
	parse_pictures(root);

	parse_operations(root);
}

sxg_parserObj::~sxg_parserObj()=default;

template<typename numerator,
	 typename denominator>
static inline auto divide(numerator n,
			  denominator d,
			  icon_scale scale)
{
	switch (scale) {
	case icon_scale::nomore:
		return n/d;
	case icon_scale::atleast:
		return (n + (d-1))/d;
	default:
		break;
	}
	return (n + (d/2))/d;
}

dim_t sxg_parserObj::adjust_width(dim_t proposed_width, icon_scale scale) const
{
	dim_t r=divide(dim_t::value_type(proposed_width),
		       dim_t::value_type(width_scale_factor()),
		       scale);

	if (r == 0)
		r=1;

	return dim_t::truncate(r*width_scale_factor());
}

dim_t sxg_parserObj::adjust_height(dim_t proposed_height, icon_scale scale)
	const
{
	dim_t r=divide(dim_t::value_type(proposed_height),
		       dim_t::value_type(height_scale_factor()),
		       scale);

	if (r == 0)
		r=1;

	return dim_t::truncate(r*height_scale_factor());
}

dim_t sxg_parserObj::default_width() const
{
	return width_for_mm(nominal_width_mm);
}

dim_t sxg_parserObj::width_for_mm(double w) const
{
	return adjust_width(dim_t::truncate(pixels_per_mm_w() * w),
			    icon_scale::nearest);
}

dim_t sxg_parserObj::default_height() const
{
	return height_for_mm(nominal_height_mm);
}

dim_t sxg_parserObj::height_for_mm(double h) const
{
	return adjust_height(dim_t::truncate(pixels_per_mm_h() * h),
			     icon_scale::nearest);
}

dim_t sxg_parserObj::height_for_width(dim_t width, icon_scale scale) const
{
	/*
               default_width           width
               -------------     =  ----------
               default_height          x


               x = default_height * width / default_width
	*/

	return adjust_height
		(dim_t::truncate
		 (std::round(divide(dim_squared_t::truncate(default_height()
							    * width),
				    dim_t::value_type(default_width()),
				    scale))),
		 scale);
}

dim_t sxg_parserObj::width_for_height(dim_t height, icon_scale scale) const
{
	/*
               default_width             x
               -------------     =  ----------
               default_height          height


               x = height * default_width / default_height
	*/

	return adjust_width
		(dim_t::truncate
		 (std::round(divide(dim_squared_t::truncate(default_width()
							    * height),
				    dim_t::value_type(default_height()),
				    scale))),
		 scale);
}

std::optional <rgb> sxg_parserObj::background_color()
	const
{
	auto iter=pictures.find("background");

	if (iter != pictures.end() &&
	    iter->second.type == picture_type_t::solid_color)
	{
		return iter->second.color.get_color(theme);
	}

	return {};
}

LIBCXXW_NAMESPACE_END
