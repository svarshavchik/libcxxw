/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_gcobj_H
#define x_w_gcobj_H

#include <x/obj.H>
#include <x/w/gcfwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/drawablefwd.H>
#include <x/w/rectangle.H>
#include <x/w/values_and_mask.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

//! A graphic context

//! TBD

class gcObj : virtual public obj {

	//! The drawable this graphic context is for.
	const drawable gc_drawable;

public:
	class LIBCXX_HIDDEN implObj;
	class LIBCXX_HIDDEN handlerObj;

	inline const drawable &get_drawable() { return gc_drawable; }

	inline const const_drawable &get_drawable() const
	{
		return gc_drawable;
	}
	//! Internal implementation object

	const ref<implObj> impl;

	//! Constructor
	gcObj(const drawable &gc_drawable, const ref<implObj> &impl);

	//! Destructor
	~gcObj();

	//! Graphic context rendering function

	//! Directly corresponds to XCB_GX_* constants.
	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::mode

	enum class function {
		CLEAR = 0,
		AND = 1,
		AND_REVERSE = 2,
		COPY = 3,
		AND_INVERTED = 4,
		NOOP = 5,
		XOR = 6,
		OR = 7,
		NOR = 8,
		EQUIV = 9,
		INVERT = 10,
		OR_REVERSE = 11,
		COPY_INVERTED = 12,
		OR_INVERTED = 13,
		NAND = 14,
		SET = 15,
        };

	//! How to fill an arc

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::fill_arc_mode

	enum class fill_arc_mode {
		chord = 0,
		pieslice = 1,
	};

	//! Line style

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::line_style

	enum class line_style {
		solid=0,
		on_off=1,
		double_dash=2,
	};

	//! Collected GC rendering properties

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::properties
	//! The methods in this class are not thread safe.

	class properties {

	public:

		//! Some properties want to keep references to other objects

		class references_t {

		public:
			//! Reference on the currently set clipmask
			pixmapptr mask;

			//! Reference on the currently set tile
			pixmapptr tile;

			//! Reference on the currently set stipple
			pixmapptr stipple;
		};

	private:
		//! Raw values.

		values_and_mask raw_values;

		//! Current references
		references_t references;

		//! Clip rectangles
		rectarea current_cliprects;

		//! Whether there are cliprects.
		bool has_cliprects=false;

	public:
		friend class handlerObj;

		//! Specification for dashed lines
		std::vector<uint8_t> dashes;

		//! Starting dash offset
		dim_t dashes_offset=0;

		//! Default constructor
		properties();

		//! Default destructor
		~properties();

		//! Copy constructor
		properties(const properties &);

		//! Assignment operator
		properties &operator=(const properties &);

		//! Set render mode
		properties &function(gcObj::function function_value);

		//! Set foreground color
		properties &foreground(uint32_t pixel);

		//! Set background type
		properties &background(uint32_t pixel);

		//! Set cap-style
		properties &cap_style(w::cap_style value);

		//! Set join-style
		properties &join_style(w::join_style value);

		//! Set fill-style to solid
		properties &fill_style_solid();

		//! Set fill-style to tiled
		properties &fill_style_tiled(const pixmap &tileArg,
					     coord_t x=0,
					     coord_t y=0);

		//! Set fill-style to stippled
		properties &fill_style_stippled(const pixmap &tileArg,
						coord_t x=0,
						coord_t y=0);

		//! Set fill-style to opaque-stippled
		properties &fill_style_opaque_stippled(const pixmap &tileArg,
						       coord_t x=0,
						       coord_t y=0);

		//! Set fill-rule
		properties &fill_rule(w::fill_rule value);

		//! Set arc type
		properties &arc_mode(fill_arc_mode value);

		//! Set line width
		properties &line_width(uint32_t width);

		//! Get line width
		uint32_t line_width() const;

		//! Set line style
		properties &line_style(gcObj::line_style line_styleArg);

		//! Set clip mask pixmap
		properties &clipmask(const pixmap &pixmap,
				     uint32_t origin_x=0,
				     uint32_t origin_y=0);

		//! Set clip mask rectangles
		properties &clipmask(const rectarea &r,
				     uint32_t origin_x=0,
				     uint32_t origin_y=0);

		//! Clear clip mask
		properties &clipmask();
	};

	//! Fill one rectangle
	void fill_rectangle(const rectangle &rectangleArg,
			    const properties &props);

	//! Fill one rectangle
	void fill_rectangle(coord_t x,
			    coord_t y,
			    dim_t width,
			    dim_t height,
			    const properties &props)
	{
		fill_rectangle({x, y, width, height}, props);
	}

	//! Fill rectangles
	void fill_rectangles(const rectarea &rectangles,
			     const properties &props);

	//! A line

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::line

	struct line {

		//! Line coordinate
		coord_t x1;

		//! Line coordinate
		coord_t y1;

		//! Line coordinate
		coord_t x2;

		//! Line coordinate
		coord_t y2;
	};

	//! Draw a single line
	inline void segment(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
			    const properties &props)
	{
		segment(line({
			.x1=x1,
			.y1=y1,
			.x2=x2,
			.y2=y2,
		}), props);
	}

	//! Draw a single line
	void segment(const line &lineArg,
		     const properties &props);

	//! Draw lines
	void segments(const std::vector<line> &linesArg,
		      const properties &props);

	//! A multipoint line.

	struct polyline {
		//! X coordinate
		coord_t x;
		//! Y coordinate
		coord_t y;
	};

	//! Fill type for lines

	enum class polyfill {none, complex, nonconvex, convex};

	//! Draw a polyline or a fillpoly

	void lines(const std::vector<polyline> &linesArg,
		   const properties &props,
		   polyfill fill_type=polyfill::none);

	//! Draw a polyline or a fillpoly
	void lines(const polyline *line,
		   size_t nlines,
		   const properties &props,
		   polyfill fill_type=polyfill::none);

	//! Definition of an arc

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::arc

	struct arc {
		//! Arc bounding box X coordinate
		coord_t  x;
		//! Arc bounding box Y coordinate
		coord_t  y;
		//! Arc width
		dim_t width;
		//! Arc height
		dim_t height;
		//! Arc starting angle * 64
		int16_t  angle1;
		//! Arc ending angle * 64
		int16_t  angle2;
	};

	//! Draw an arc
	inline void draw_arc(coord_t x, coord_t y,
			     dim_t width, dim_t height,
			     short angle1, short angle2,
			     const properties &props)
	{
		draw_arc(arc({
			.x=x,
			.y=y,
			.width=width,
                        .height=height,
                        .angle1=angle1,
                        .angle2=angle2,
		}), props);
	}


	//! Draw an arc
	void draw_arc(const arc &arcArg,
		      const properties &props);

	//! Draw arcs
	void draw_arcs(const std::vector<arc> &arcs,
		       const properties &props)
	{
		draw_arcs(&arcs.at(0), arcs.size(), props);
	}

	//! Draw arcs
	void draw_arcs(const arc *arcs,
		       size_t n_arcs,
		       const properties &props);

	//! Fill an arc
	inline void fill_arc(coord_t x, coord_t y,
			     dim_t width, dim_t height,
			     short angle1, short angle2,
			     const properties &props)
	{
		fill_arc(arc({
			.x=x,
			.y=y,
			.width=width,
                        .height=height,
                        .angle1=angle1,
                        .angle2=angle2,
		}), props);
	}

	//! Fill an arc
	void fill_arc(const arc &arcArg,
		      const properties &props);

	//! Fill arcs
	void fill_arcs(const std::vector<arc> &arcs,
		       const properties &props)
	{
		if (arcs.empty())
			return;
		fill_arcs(&arcs.at(0), arcs.size(), props);
	}

	//! Draw arcs
	void fill_arcs(const arc *arcs,
		       size_t n_arcs,
		       const properties &props);

	//! Copy a portion of this gc's drawable to another position in it.

	void copy(const rectangle &rect,
		  coord_t to_x,
		  coord_t to_y);

	//! Copy a portion of this drawable to another drawable.

	//! The other drawable must have the same pictformat
	void copy(const rectangle &rect,
		  coord_t to_x,
		  coord_t to_y,
		  const drawable &to_drawable,
		  const properties &props={}) const;
};

LIBCXXW_NAMESPACE_END

#endif
