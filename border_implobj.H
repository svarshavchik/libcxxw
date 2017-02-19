/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef border_implobj_h
#define border_implobj_h

#include "x/w/types.H"
#include "x/w/pictureobj.H"
#include "x/w/picturefwd.H"
#include "x/w/pixmapfwd.H"
#include "x/w/pixmapobj.H"
#include "x/w/gcfwd.H"
#include "x/w/gcobj.H"
#include "x/w/rectangle.H"
#include "border_info.H"
#include "border_implfwd.H"
#include <vector>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! A border implementation

//! \see border_impl

class LIBCXX_HIDDEN border_implObj: virtual public obj, public border_info {

 public:

	//! Default constructor
	border_implObj();

	//! Clone constructor
	border_implObj(const border_info &b);

	//! Destructor
	~border_implObj();

	//! Clone this border.
	border_impl clone() const;

	//! Compute the calculated values from border_info.

	void calculate();

	//! Sum of all dashes, precalculated for speed
	dim_t calculated_dashes_sum;

	//! Actual border width
	dim_t calculated_border_width;

	//! Actual border height
	dim_t calculated_border_height;

	//! A border_radius value of 2 or more has an inner radius.
	static constexpr dim_t inner_radius(dim_t r)
	{
		return r < 2 ? 0:r;
	}

	//! A border_radius value of 2 or more has an inner radius.

	dim_t inner_radius() const
	{
		return inner_radius(radius);
	}

	//! Information that's passed to the various draw...() methods.

	//! Refer to this class as \c LIBCXX_NAMESPACE::w::border_impl::base::draw_info

	struct draw_info {

		//! The border gets rendered into this picture

		//! This is not the actual display element, but rather a
		//! scratch/temporary buffer where the border(s) get
		//! pre-drawn, before the final image gets composited into
		//! the display element.

		picture area_picture;

		//! area_picture represents this rectangle where the border is.
		rectangle area_rectangle;

		//! area_picture's pixmap

		//! Before draw..()ing, the area picture/pixmap gets
		//! cleared to the display element's background color.

		pixmap area_pixmap;

		//! A one-pixel scratch buffer picture for draw...() to use.

		picture mask_picture;

		//! mask_picture's one-bit deep pixmap

		//! The pixmap does not get initialized in any way. Its size
		//! is guaranteed to be at least width*height.

		pixmap mask_pixmap;

		//! mask_pixmap's graphic context.
		gc mask_gc;

		//! Area picture+pixmap's logical x coordinate

		//! Specifies to the draw...() methods that the area
		//! picture/pixmap's represents a rectangle in its parent
		//! display element whose top-left corner is this coordinate.
		coord_t area_x;

		//! Specifies to the draw...() methods that the area
		//! picture/pixmap's represents a rectangle in its parent
		//! display element whose top-left corner is this coordinate.
		coord_t area_y;
	};


	//! Draw a horizontal border

	void draw_horizontal(//! Information needed to draw the border
			     const draw_info &di,

			     //! Draw a top border
			     bool top_border,

			     //! Draw a bottom border

			     //! Both top and bottom border can be set,
			     //! indicating that this border is between two
			     //! elements.
			     bool bottom_border) const;

	struct corner_draw_info;

 private:
	//! Draw a horizontal border

	void draw_horizontal(//! Information needed to draw the border
			     const draw_info &di,

			     //! The starting coordinate to draw from.
			     coord_t x,

			     //! The length of the border to draw, in pixels.
			     dim_t length,

			     //! Draw a top border
			     bool top_border,

			     //! Draw a bottom border

			     //! Both top and bottom border can be set,
			     //! indicating that this border is between two
			     //! elements.
			     bool bottom_border) const;
 public:
	//! Draw a vertical border

	void draw_vertical(//! Information needed to draw the border
			   const draw_info &di,

			   //! Draw a left border
			   bool left_border,

			   //! Draw a right border

			   //! Both left and right border can be set,
			   //! indicating that this border is between two
			   //! elements.
			   bool right_border) const;

 private:
	//! Draw a vertical border

	void draw_vertical(//! Information needed to draw the border
			   const draw_info &di,

			   //! The row to draw from.
			   coord_t y,

			   //! The length of the border to draw, in pixels.
			   dim_t length,

			   //! Draw a left border
			   bool left_border,

			   //! Draw a right border

			   //! Both left and right border can be set,
			   //! indicating that this border is between two
			   //! elements.
			   bool right_border) const;

	//! Composite color #n into the area_picture.

	//! draw_horizontal or draw_vertical prepared a mask for compositing
	//! color #n into area_picture, so we do it here.

	void composite_line(const draw_info &di, size_t n) const;

 public:
	//! Draw a corner, or perhaps a frankencorner.

	//! which_corners is a logical-or of one or more
	//! border_impl::base::corner??() constants. Hopefully there's only
	//! one, in which case this invokes the appropriate draw_corner??()
	//! method here.
	//!
	//! Otherwise this gets translated into draw_horizontal() and
	//! draw_vertical() corners.
	//!
	//! The x/y parameters to draw_??() will be 0 unless area_rectangle's
	//! width or height exceeds calculated_border_width/height, in which
	//! case the slack gets handled by draw_horizontal() and/or
	//! draw_vertical(), and the x/y parameters to draw_corner??()
	//! indicate a centered rectangle in area_pictures that's
	//! exactly calculated_border_width/height in size.

	void draw_corner(//! Information needed to draw the border
			 const draw_info &di,

			 //! Which corner(s) is being drawn.
			 int which_corners) const;

	//! Draw a non-joining corner stub.

	//! When a border does not join through a corner, the border is
	//! simply a stub to the center of the border area.

	void draw_stubs(//! Information needed to draw the border
			const draw_info &di,

			//! Which stubs are being drawn
			int which_stubs) const;

 private:

	//! Draw a top-left corner.
	void draw_cornertl(const draw_info &di,
			   coord_t x,
			   coord_t y) const;

	//! Draw a top-right corner.
	void draw_cornertr(const draw_info &di,
			   coord_t x,
			   coord_t y) const;

	//! Draw a bottom-left corner.
	void draw_cornerbl(const draw_info &di,
			   coord_t x,
			   coord_t y) const;

	//! Draw a bottom-right corner.
	void draw_cornerbr(const draw_info &di,
			   coord_t x,
			   coord_t y) const;

	//! When there's no radius, all the corners are just plain squares.
	void draw_square_corner(const draw_info &di,
				coord_t x,
				coord_t y) const;

 public:
	//! This is no border at all
	using border_info::no_border;

	//! There's no border to draw, here.
	bool no_border(const draw_info &di) const;
 private:
	//! Compute x-centerline of a horizontal border.

	//! The X protocol draws lines of given thickness centered over the
	//! coordinates. Compute the horizontal centerline of this border.

	coord_t compute_xcenter(const draw_info &di) const;

	//! Reverse-engineer border's left and right drawn margin.

	//! This takes the computed xcenter, and subtracts half the width
	//! which gives the left x coordinate of a vertical border, then
	//! adds the width, producing the right margin.

	std::tuple<coord_t, coord_t> compute_xleftright(coord_t xcenter) const;

	//! Reverse-engineer border's top and bottom drawn margin.

	//! This takes the computed ycenter, and subtracts half the height
	//! which gives the top y coordinate of a horizontal border, then
	//! adds the height, producing the right margin.

	std::tuple<coord_t, coord_t> compute_ytopbottom(coord_t ycenter) const;

	//! Compute y-centerline of a horizontal border.

	//! The X protocol draws lines of given thickness centered over the
	//! coordinates. Compute the vertical centerline of this border.

	coord_t compute_ycenter(const draw_info &di) const;

	//! Clear an area of mask_gc

	static void mask_gc_clear(const draw_info &di);

	//! Draw a segment in mask_pixmap, using max_gc.

	void mask_segment(const draw_info &di,

			  //! Absolute starting offset

			  //! For horizontal segments, the absolute X
			  //! position: di.area_x plus x, the position
			  //! in the area_picture where the border gets drawn.
			  //! For vertical segments, the absolutely Y position.
			  //!
			  //! Used to compute dashes_offset for dashed lines.
			  coord_t absolute_offset,

			  //! The width of the segment, in pixels
			  dim_t line_width,
			  //! Starting segment position
			  coord_t x1,
			  //! Starting segment position
			  coord_t y1,
			  //! Ending segment position
			  coord_t x2,
			  //! Ending segment position
			  coord_t y2) const;
};

LIBCXXW_NAMESPACE_END

#endif
