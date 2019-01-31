/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_border_implobj_h
#define x_w_impl_border_implobj_h

#include <x/w/types.H>
#include <x/w/pictureobj.H>
#include <x/w/picturefwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/pixmapobj.H>
#include <x/w/gcfwd.H>
#include <x/w/gcobj.H>
#include <x/w/elementobj.H>
#include <x/w/rectangle.H>
#include <x/w/defaultthemefwd.H>
#include <x/w/impl/border_info.H>
#include <x/w/impl/border_implfwd.H>
#include <x/w/connection_threadfwd.H>
#include <vector>
#include <tuple>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN surrounding_elements_info;

//! A border implementation

//! \see border_impl

class border_implObj: virtual public obj, public border_info {

 public:

	//! Constructor
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

	//! Both hradius and vradius must be 2 or more, otherwise it's 0.

	dim_t inner_hradius() const
	{
		return inner_radius(vradius) != 0
			? inner_radius(hradius):dim_t{0};
	}

	//! A border_radius value of 2 or more has an inner radius.

	//! Both hradius and vradius must be 2 or more, otherwise it's 0.

	dim_t inner_vradius() const
	{
		return inner_radius(hradius) != 0
			? inner_radius(vradius):dim_t{0};
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

		//! Fill in the background color.

		//! Called before calling draw_horizontal(). Passes
		//! 0 and area_rectangle.width for the 3rd and 4th parameter
		//! to the overloaded function.

		void background_horizontal(ONLY IN_THREAD,
					   const elementptr &above_element,
					   const elementptr &below_element)
			const;

		//! Fill in the background color
		//!
		//! \overload
		void background_horizontal(ONLY IN_THREAD,
					   const element_implptr &above_element,
					   const element_implptr &below_element,
					   coord_t start,
					   dim_t size)
			const;

		//! Fill in the background color.

		//! Called before calling draw_vertical(). Passes
		//! 0 and area_rectangle.height for the 3rd and 4th parameter
		//! to the overloaded function.

		void background_vertical(ONLY IN_THREAD,
					 const elementptr &left_element,
					 const elementptr &right_element)
			const;

		//! Fill in the background color.

		//! \overload
		void background_vertical(ONLY IN_THREAD,
					 const element_implptr &left_element,
					 const element_implptr &right_element,
					 coord_t start,
					 dim_t size)
			const;
	};

	//! Draw a horizontal border
	void draw_horizontal(//! Invoked from the connection thread

			     ONLY IN_THREAD,

			     //! Information needed to draw the border
			     const draw_info &di) const;

	struct corner_draw_info;

 private:
	//! Draw a horizontal border

	void draw_horizontal(//! Invoked from the connection thread
			     ONLY IN_THREAD,

			     //! Information needed to draw the border
			     const draw_info &di,

			     //! The starting coordinate to draw from.
			     coord_t x,

			     //! The length of the border to draw, in pixels.
			     dim_t length) const;
 public:

	//! Draw a vertical border

	void draw_vertical(//! Invoked from the connection thread

			   ONLY IN_THREAD,

			   //! Information needed to draw the border
			   const draw_info &di) const;

 private:
	//! Draw a vertical border

	void draw_vertical(//! Invoked from the connection thread
			   ONLY IN_THREAD,

			   //! Information needed to draw the border
			   const draw_info &di,

			   //! The row to draw from.
			   coord_t y,

			   //! The length of the border to draw, in pixels.
			   dim_t length) const;

	//! Composite color into the area_picture.

	//! draw_horizontal or draw_vertical prepared a mask for compositing
	//! color into area_picture, so we do it here.

	void composite_line(ONLY IN_THREAD,
			    const draw_info &di,
			    const background_color &c) const;

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

	void draw_corner(//! Invoked from the connection thread
			 ONLY IN_THREAD,

			 //! Information needed to draw the border
			 const draw_info &di,

			 //! Which corner(s) is being drawn.
			 int which_corners,

			 //! Elements around the border
			 const struct surrounding_elements_info &elements

			 ) const;

	//! Draw a non-joining corner stub.

	//! When a border does not join through a corner, the border is
	//! simply a stub to the center of the border area.

	void draw_stubs(//! Invoked from the connection thread
			ONLY IN_THREAD,

			//! Information needed to draw the border
			const draw_info &di,

			//! Which stubs are being drawn
			int which_stubs) const;

 private:

	//! Draw a top-left corner.
	void draw_cornertl(ONLY IN_THREAD,
			   const corner_draw_info &di,
			   const surrounding_elements_info &elements) const;

	//! Draw a top-right corner.
	void draw_cornertr(ONLY IN_THREAD,
			   const corner_draw_info &di,
			   const surrounding_elements_info &elements) const;

	//! Draw a bottom-left corner.
	void draw_cornerbl(ONLY IN_THREAD,
			   const corner_draw_info &di,
			   const surrounding_elements_info &elements) const;

	//! Draw a bottom-right corner.
	void draw_cornerbr(ONLY IN_THREAD,
			   const corner_draw_info &di,
			   const surrounding_elements_info &elements) const;

	//! When there's no radius, all the corners are just plain squares.
	void draw_square_corner(ONLY IN_THREAD,
				const draw_info &di,
				coord_t x,
				coord_t y) const;

	//! There's both a horizontal and a vertical radius to be drawn.
	void draw_round_corner(ONLY IN_THREAD,
			       const draw_info &di,
			       coord_t x,
			       coord_t y,
			       bool subtract_width,
			       bool subtract_height,
			       const element_implptr &corner_element)
		const;

 public:
	//! There's no border to draw, here.
	bool no_corner_border(const draw_info &di) const;

	//! There's no border to draw, here.
	bool no_horizontal_border(const draw_info &di) const;

	//! There's no border to draw, here.
	bool no_vertical_border(const draw_info &di) const;
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

	void mask_segment(//! What to draw.

			  const draw_info &di,

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

	//! Invert the max_pixmap.

	//! After mask_segment(), XOR all the bits in the mask, flipping them.

	void mask_segment_xor(const draw_info &di,
			      dim_t line_width,
			      coord_t x1, coord_t y1,
			      coord_t x2, coord_t y2) const;
};

LIBCXXW_NAMESPACE_END

#endif
