/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/border_impl.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/gc.H"
#include "grid_element.H"
#include "x/w/impl/element.H"
#include "picture.H"
#include "grid_element.H"
#include "corner_borderfwd.H"
#include "x/w/impl/draw_info.H"
#include "x/w/impl/background_color.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

border_implObj::border_implObj(const border_info &b)
	: border_info(b)
{
}

border_implObj::~border_implObj()=default;

///////////////////////////////////////////////////////////////////////////////
//
// Based on calculated dimensions, and draw_info, compute various coordinates
// needed to draw the corner of this border.

struct border_implObj::corner_draw_info {

	// The draw_info we're based on.

	const draw_info &di;

	// The border we're for.
	const border_implObj &me;

	// First, any padding on each side, if the alloted area for the
	// border exceeds our calculated dimensions.

	dim_t left_pad, right_pad, top_pad, bottom_pad;

	// We may have two, three, or four, corners to draw. Start unraveling
	// this by dividing calculated_border_width/height into two halfs,
	// each.

	dim_t left_half_width, right_half_width,
		top_half_height,
		bottom_half_height;

	// left_pad and top_pad are the starting (x,y) coordinates
	// for the real border.
	coord_t x, y;

	// Adding left_half_width and top_half_height to (x, y)
	// produces the center point.
	coord_t center_x, center_y;

	corner_draw_info(const draw_info &di,
			 const border_implObj &me);

	// Draw extra lines in the padding areas.

	void draw_top_pad(ONLY IN_THREAD)
	{
		me.draw_vertical(IN_THREAD,
				 di, 0, top_pad);
	}

	void draw_bottom_pad(ONLY IN_THREAD)
	{
		me.draw_vertical(IN_THREAD, di,
				 coord_t::truncate(top_pad
						   +me.calculated_border_height
						   ),
				 bottom_pad);
	}

	void draw_left_pad(ONLY IN_THREAD)
	{
		me.draw_horizontal(IN_THREAD,
				   di, 0, left_pad);
	}

	void draw_right_pad(ONLY IN_THREAD)
	{
		me.draw_horizontal(IN_THREAD, di,
				   coord_t::truncate(left_pad
						     +me.calculated_border_width
						     ),
				   right_pad);
	}

	// Now, draw the corresponding line to the center of the border.

	void draw_top_stub(ONLY IN_THREAD)
	{
		me.draw_vertical(IN_THREAD, di, y, top_half_height);
	}

	void draw_bottom_stub(ONLY IN_THREAD)
	{
		me.draw_vertical(IN_THREAD, di, center_y, bottom_half_height);
	}

	void draw_left_stub(ONLY IN_THREAD)
	{
		me.draw_horizontal(IN_THREAD, di, 0, left_half_width);
	}

	void draw_right_stub(ONLY IN_THREAD)
	{
		me.draw_horizontal(IN_THREAD, di, center_x, right_half_width);
	}

	// Fill the corner from that corner's element's background color

	// Used only when drawing a straight corner.

	void topleft_background_fill(ONLY IN_THREAD,
				     const surrounding_elements_info &elements)
		const
	{
		if (!elements.topleft)
			return;

		auto &e_draw_info=elements.topleft->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=e_draw_info.background_xy_to(di.area_x,
						     di.area_y,
						     x, y);

		di.area_picture->impl->composite(e_draw_info.window_background,
						 xy.first,
						 xy.second,
						 x, y,
						 left_half_width,
						 top_half_height);
	}

	void topright_background_fill(ONLY IN_THREAD,
				      const surrounding_elements_info &elements)
		const
	{
		if (!elements.topright)
			return;

		auto &e_draw_info=elements.topright->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto new_x=coord_t::truncate(x+left_half_width);

		auto xy=e_draw_info.background_xy_to(di.area_x,
						     di.area_y,
						     new_x, y);

		di.area_picture->impl->composite(e_draw_info.window_background,
						 xy.first,
						 xy.second,
						 new_x, y,
						 right_half_width,
						 top_half_height);
	}

	void bottomleft_background_fill(ONLY IN_THREAD,
					const surrounding_elements_info
					&elements) const
	{
		if (!elements.bottomleft)
			return;

		auto &e_draw_info=elements.bottomleft->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto new_y=coord_t::truncate(y+top_half_height);

		auto xy=e_draw_info.background_xy_to(di.area_x,
						     di.area_y,
						     x, new_y);

		di.area_picture->impl->composite(e_draw_info.window_background,
						 xy.first,
						 xy.second,
						 x, new_y,
						 left_half_width,
						 bottom_half_height);
	}

	void bottomright_background_fill(ONLY IN_THREAD,
					 const surrounding_elements_info
					 &elements) const
	{
		if (!elements.bottomright)
			return;

		auto &e_draw_info=elements.bottomright->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto new_x=coord_t::truncate(x+left_half_width);
		auto new_y=coord_t::truncate(y+top_half_height);

		auto xy=e_draw_info.background_xy_to(di.area_x,
						     di.area_y,
						     new_x, new_y);

		di.area_picture->impl->composite(e_draw_info.window_background,
						 xy.first,
						 xy.second,
						 new_x, new_y,
						 right_half_width,
						 bottom_half_height);
	}

	//! Invoke the appropriate background_fill based on which_corners.

	void background_fill(ONLY IN_THREAD,
			     int which_corners,
			     const surrounding_elements_info &elements)
		const
	{
		if (which_corners & border_impl::base::cornertl())
			topleft_background_fill(IN_THREAD, elements);

		if (which_corners & border_impl::base::cornertr())
			topright_background_fill(IN_THREAD, elements);

		if (which_corners & border_impl::base::cornerbl())
			bottomleft_background_fill(IN_THREAD, elements);

		if (which_corners & border_impl::base::cornerbr())
			bottomright_background_fill(IN_THREAD, elements);
	}
};

border_implObj::corner_draw_info::corner_draw_info(const draw_info &di,
						   const border_implObj &me)

	// Calculate any padding due to extra dimensions.

	: di(di),
	  me(me),
	  left_pad((di.area_rectangle.width - me.calculated_border_width)/2),
	  right_pad(di.area_rectangle.width - left_pad - me.calculated_border_width),
	  top_pad((di.area_rectangle.height - me.calculated_border_height)/2),
	  bottom_pad(di.area_rectangle.height - top_pad - me.calculated_border_height),

	  // Half the calculated size.
	  left_half_width(me.calculated_border_width/2),
	  right_half_width(me.calculated_border_width-left_half_width),

	  top_half_height(me.calculated_border_height/2),
	  bottom_half_height(me.calculated_border_height-top_half_height),

	  // Starting x/y coordinates for the border.
	  x(coord_t::truncate(left_pad)),
	  y(coord_t::truncate(top_pad)),

	  // And the center point.
	  center_x(coord_t::truncate(x+left_half_width)),
	  center_y(coord_t::truncate(y+top_half_height))
{
}

border_impl border_implObj::clone() const
{
	const border_info &me=*this;

	return border_impl::create(me);
}

void border_implObj::calculate()
{
	calculated_border_width=dim_t::truncate(width + inner_hradius()*2);
	calculated_border_height=dim_t::truncate(height + inner_vradius()*2);

	calculated_dashes_sum=0;

	for (const auto &dash:dashes)
		calculated_dashes_sum=dim_t::truncate(calculated_dashes_sum
						      + dash);

	if (dashes.size() % 2)
		calculated_dashes_sum=dim_t::truncate(calculated_dashes_sum*2);

	if (calculated_dashes_sum == 0)
		// Some wise guy gave me a dash
		// pattern that added up to 65536.
		calculated_dashes_sum=1;

	for (const auto &dash:dashes)
		if (dash == 0)
			throw EXCEPTION("Dash size specified as 0 pixels");
}

/////////////////////////////////////////////////////////////////////////////


// No border is drawn when:

bool border_implObj::no_corner_border(const draw_info &di) const
{
	// width or height is 0, or
	//
	// no colors, or
	//
	// The area to draw the border is too narrow. This shouldn't happen,
	// but can be a temporary condition when the theme changes.

	return border_info::no_border()
		|| (di.area_rectangle.width < calculated_border_width)
		|| (di.area_rectangle.height < calculated_border_height);
}

bool border_implObj::no_horizontal_border(const draw_info &di) const
{
	// width or height is 0, or
	//
	// no colors, or
	//
	// The area to draw the border is too narrow. This shouldn't happen,
	// but can be a temporary condition when the theme changes.

	return border_info::no_border()
		|| (di.area_rectangle.height < calculated_border_height);
}

bool border_implObj::no_vertical_border(const draw_info &di) const
{
	// width or height is 0, or
	//
	// no colors, or
	//
	// The area to draw the border is too narrow. This shouldn't happen,
	// but can be a temporary condition when the theme changes.

	return border_info::no_border()
		|| (di.area_rectangle.width < calculated_border_width);
}

void border_implObj::draw_horizontal(ONLY IN_THREAD,
				     const draw_info &di) const
{
	if (no_horizontal_border(di))
		return;

	draw_horizontal(IN_THREAD, di, 0, di.area_rectangle.width);
}

void border_implObj::draw_horizontal(ONLY IN_THREAD,
				     const draw_info &di,
				     coord_t x,
				     dim_t length) const
{
	if (length == 0)
		return; // Corner case.

	// The border's top/left coordinate is (x, y) in area_picture, whose
	// top/left coordinate is (area_rectangle.x, area_rectangle.y), of
	// an element position at (area_x, area_y) in its container.
	//
	// Therefore, absolute_x/y is going to be the border's absolute
	// starting position.

	coord_t absolute_x=coord_t::truncate
		(coord_t::truncate(di.area_rectangle.x+x)+di.area_x);

	// Clear up the 1-bit masking pixmap, for the drawn border.

	mask_gc_clear(di);

	auto ycenter=compute_ycenter(di);

	// Draw a horizontal line (0, ycenter)-(width, ycenter),
	// that's "height" pixels thick.

	auto x2=coord_t::truncate(x+length);

	mask_segment(di, absolute_x, height,
		     x, ycenter,
		     x2, ycenter);

	composite_line(IN_THREAD, di, color1);
	if (color2)
	{
		mask_segment_xor(di, height, x, ycenter, x2, ycenter);
		composite_line(IN_THREAD, di, color2);
	}
}

// Same logic as draw_horizontal, but in the other direction.

void border_implObj::draw_vertical(ONLY IN_THREAD,
				   const draw_info &di) const
{
	if (no_vertical_border(di))
		return;

	draw_vertical(IN_THREAD, di, 0, di.area_rectangle.height);
}

void border_implObj::draw_vertical(ONLY IN_THREAD, const draw_info &di,
				   coord_t y,
				   dim_t length) const
{
	if (length == 0)
		return;

	coord_t absolute_y=
		coord_t::truncate(coord_t::truncate(di.area_rectangle.y+y)
				  +di.area_y);

	mask_gc_clear(di);

	auto xcenter=compute_xcenter(di);
	auto y2=coord_t::truncate(y+length);

	mask_segment(di, absolute_y, width,
		     xcenter, y,
		     xcenter, y2);

	composite_line(IN_THREAD, di, color1);
	if (color2)
	{
		mask_segment_xor(di, width, xcenter, y, xcenter, y2);
		composite_line(IN_THREAD, di, color2);
	}
}

void border_implObj::draw_corner(ONLY IN_THREAD,
				 const draw_info &di, int which_corners,
				 const surrounding_elements_info &elements
				 ) const
{
	corner_draw_info cdi{di, *this};

	if (no_corner_border(di))
	{
		cdi.background_fill(IN_THREAD, which_corners, elements);
		return;
	}

	bool tl=which_corners & border_impl::base::cornertl() ? true:false;
	bool tr=which_corners & border_impl::base::cornertr() ? true:false;
	bool bl=which_corners & border_impl::base::cornerbl() ? true:false;
	bool br=which_corners & border_impl::base::cornerbr() ? true:false;

	if (tl || tr)
		cdi.draw_top_pad(IN_THREAD);

	if (bl || br)
		cdi.draw_bottom_pad(IN_THREAD);

	if (tl || bl)
		cdi.draw_left_pad(IN_THREAD);

	if (tr || br)
		cdi.draw_right_pad(IN_THREAD);

	// If which_corners is a single corner, we have special code for that.

	// Before we draw the corner, though, make sure, that all other
	// corners' background colors are filled in from the corners'
	// element.

	switch (which_corners) {
	case border_impl::base::cornertl():
	case border_impl::base::cornertr():
	case border_impl::base::cornerbl():
	case border_impl::base::cornerbr():

		int other_corners = (border_impl::base::cornertl() |
				     border_impl::base::cornertr() |
				     border_impl::base::cornerbl() |
				     border_impl::base::cornerbr()
				     ) ^ which_corners;
		cdi.background_fill(IN_THREAD, other_corners, elements);
		break;
	}

	switch (which_corners) {
	case border_impl::base::cornertl():
		draw_cornertl(IN_THREAD, cdi, elements);
		return;
	case border_impl::base::cornertr():
		draw_cornertr(IN_THREAD, cdi, elements);
		return;
	case border_impl::base::cornerbl():
		draw_cornerbl(IN_THREAD, cdi, elements);
		return;
	case border_impl::base::cornerbr():
		draw_cornerbr(IN_THREAD, cdi, elements);
		return;
	}

	// Zero or multiple corners. We will draw a straight border around
	// each one, but start by filling each requested corner with the
	// corresponding element's background color.
	//
	// We need to do this explicitly instead of passing the appropriate
	// elements to draw_<name>_(). We want to fill in the background
	// color before any of the stubs get drawn.

	cdi.background_fill(IN_THREAD, which_corners, elements);

	bool drew_top=false, drew_bottom=false,
		drew_left=false, drew_right=false;

	// If there are two adjacent corners, find the straggler in the
	// middle, and draw it.

	if ( (tl && tr) && !bl && !br)
	{
		cdi.draw_top_stub(IN_THREAD);
		drew_top=true;
	}

	if ( (bl && br) && !tl && !tr)
	{
		cdi.draw_bottom_stub(IN_THREAD);
		drew_bottom=true;
	}

	if ( (tl && bl) && !tr && !br)
	{
		cdi.draw_left_stub(IN_THREAD);
		drew_left=true;
	}

	if ( (tr && br) && !tl && !bl)
	{
		cdi.draw_right_stub(IN_THREAD);
		drew_right=true;
	}

	// And now we can draw the full stroke.

	if ( (tl || tr) && !drew_top)
	{
		draw_vertical(IN_THREAD,
			      di, cdi.y, cdi.top_half_height);
	}

	if ( (bl || br) && !drew_bottom)
	{
		draw_vertical(IN_THREAD,
			      di, cdi.center_y, cdi.bottom_half_height);

	}

	if ( (tl || bl) && !drew_left)
	{
		draw_horizontal(IN_THREAD, di, 0, cdi.left_half_width);
	}

	if ( (tr || br) && !drew_right)
	{
		draw_horizontal(IN_THREAD, di, cdi.center_x,
				cdi.right_half_width);
	}
}

void border_implObj::draw_cornertl(ONLY IN_THREAD,
				   const corner_draw_info &cdi,
				   const surrounding_elements_info &elements)
	const
{
	if (hradius != 0 && vradius != 0)
	{
		draw_round_corner(IN_THREAD,
				  cdi.di,
				  cdi.x, cdi.y,
				  true, true,
				  elements.topleft);
		return;
	}
	cdi.topleft_background_fill(IN_THREAD, elements);
	draw_square_corner(IN_THREAD, cdi.di, cdi.x, cdi.y);
}

void border_implObj::draw_cornertr(ONLY IN_THREAD,
				   const corner_draw_info &cdi,
				   const surrounding_elements_info &elements)
	const
{
	if (hradius != 0 && vradius != 0)
	{
		draw_round_corner(IN_THREAD,
				  cdi.di,
				  cdi.x, cdi.y,
				  false, true,
				  elements.topright);
		return;
	}
	cdi.topright_background_fill(IN_THREAD, elements);
	draw_square_corner(IN_THREAD, cdi.di, cdi.x, cdi.y);
}

void border_implObj::draw_cornerbl(ONLY IN_THREAD,
				   const corner_draw_info &cdi,
				   const surrounding_elements_info &elements)
	const
{
	if (hradius != 0 && vradius != 0)
	{
		draw_round_corner(IN_THREAD,
				  cdi.di,
				  cdi.x, cdi.y,
				  true, false,
				  elements.bottomleft);
		return;
	}
	cdi.bottomleft_background_fill(IN_THREAD, elements);
	draw_square_corner(IN_THREAD, cdi.di, cdi.x, cdi.y);
}

void border_implObj::draw_cornerbr(ONLY IN_THREAD,
				   const corner_draw_info &cdi,
				   const surrounding_elements_info &elements)
	const
{
	if (hradius != 0 && vradius != 0)
	{
		draw_round_corner(IN_THREAD,
				  cdi.di,
				  cdi.x, cdi.y,
				  false, false,
				  elements.bottomright);
		return;
	}
	cdi.bottomright_background_fill(IN_THREAD, elements);
	draw_square_corner(IN_THREAD, cdi.di, cdi.x, cdi.y);
}

void border_implObj::draw_square_corner(ONLY IN_THREAD,
					const draw_info &di,
					coord_t x,
					coord_t y) const
{
	mask_gc_clear(di);

	gc::base::properties props;

	props.foreground(1);
	props.background(1);
	props.function(gc::base::function::SET);
	di.mask_gc->fill_rectangle(x, y,
				   calculated_border_width,
				   calculated_border_height, props);
	composite_line(IN_THREAD, di, color1);
}

void border_implObj::draw_round_corner(ONLY IN_THREAD,
				       const draw_info &di,
				       coord_t x,
				       coord_t y,
				       bool subtract_width,
				       bool subtract_height,
				       const grid_elementptr &element) const
{
	mask_gc_clear(di);

	gc::base::properties props;

	props.background(1);
	props.foreground(1);
	props.function(gc::base::function::SET);

	// A rounded border is created by simply drawing a circle or an
	// oval, and clipping the result.
	//
	// x & y are the starting coordinates for this border element, whose
	// size is calculated_border_width x calculated_border_height.
	//
	// We start by clipping everything to the actual border element.

	props.clipmask({{x, y, calculated_border_width,
					calculated_border_height}});

	// Now, all we have to do is compute the radius of the circles/ovals,
	// and draw them. If we draw at (x, y), we'll end up clipping the
	// top-right quarter of the circle/ovals, forming the top-right border.
	//
	// By adjusting the (x, y) coordinates, we arrange to have the
	// appropriate quarter of the circle/oval to fall into the clipped
	// area:

	coord_t circle_x=x;
	coord_t circle_y=y;

	if (subtract_width)
		circle_x=coord_t::truncate(circle_x-calculated_border_width);

	if (subtract_height)
		circle_y=coord_t::truncate(circle_y-calculated_border_height);

	// inner_hradius() and inner_vradius() translates to additional
	// padding around the border. The border is formed from, essentially:
	//
	//  radius area    border width/height  radius area
	//
	// So, we simply add the inner radius to the X coordinates to get
	// the top/right corner of the rectangle within which the circle/oval
	// gets drawn.

	circle_x=coord_t::truncate(circle_x+inner_hradius());
	circle_y=coord_t::truncate(circle_y+inner_vradius());

	// And there's an inner radius, we start by creating a mask for the
	// innermost circle/oval that comprises the inner area of the rounded
	// border. This will be filled in by the inner element's background
	// color.

	if (inner_hradius() > 0)
	{
		// Advance the (x, y) coordinate's by the border's size,
		// to compute the rectangle that encompasses the circle/oval
		// representing the inner side of the rounded border. The
		// size of the rectangle is simply inner_radius*2.
		//
		// Looks like rectangle dimensions for drawing an arc
		// enclose the actual rectangle, so we have to subtract 1.

		coord_t inner_x = coord_t::truncate(circle_x + width);
		coord_t inner_y = coord_t::truncate(circle_y + height);

		di.mask_gc->fill_arc(inner_x, inner_y,
				     inner_hradius()*2-1,
				     inner_vradius()*2-1,
				     0, 360*64, props);

		// Before clearing the inner border, use this mask to
		// fill the inner border with the element's background_color

		if (element)
		{
			auto &e_draw_info=element->grid_element->impl
				->get_draw_info(IN_THREAD);

			auto xy=e_draw_info.background_xy_to
				(coord_t::truncate(di.area_x+x),
				 coord_t::truncate(di.area_y+y));

			picture::base::clip_mask mask(di.area_picture,
						      di.mask_pixmap,
						      0, 0);

			di.area_picture->impl->composite
				(e_draw_info.window_background,
				 xy.first, xy.second,
				 0, 0,
				 di.area_rectangle.width,
				 di.area_rectangle.height,
				 render_pict_op::op_over);
		}
	}

	// Ok, the size of the circle/oval for the rounded border is
	// the inner radius plus border width times two. And we have to
	// subtract 1 to make X11's gods happy.

	dim_t outer_circle_w=
		dim_t::truncate(inner_hradius()+width+width+inner_hradius()-1);
	dim_t outer_circle_h=
		dim_t::truncate(inner_vradius()+height+height+inner_vradius()-1);

	// Use XOR to create a mask for the border itself. If there was
	// an inner radius, this is going to clear the mask for the inner
	// area of the border, leaving us with just the outline for the
	// rounded line.

	props.function(gc::base::function::XOR);

	di.mask_gc->fill_arc(circle_x, circle_y,
			     outer_circle_w, outer_circle_h,
			     0, 360*64, props);

#if 0
	// Redraw the outer border, to make sure there's at least a
	// 1-pixel rounded line.
	props.function(gc::base::function::SET);

	di.mask_gc->draw_arc(circle_x, circle_y, outer_circle_w, outer_circle_h,
			     0, 360*64, props);
#endif

	composite_line(IN_THREAD, di, color1);
}

void border_implObj::draw_stubs(ONLY IN_THREAD,
				const draw_info &di, int stubs) const
{
	if (no_corner_border(di))
		return;

	corner_draw_info cdi{di, *this};

	// We don't need to clear to each elements' background colors,
	// corner_border has already done that, on this code path.

	if (stubs & border_impl::base::top_stub())
	{
		cdi.draw_top_pad(IN_THREAD);
		cdi.draw_top_stub(IN_THREAD);
	}

	if (stubs & border_impl::base::bottom_stub())
	{
		cdi.draw_bottom_pad(IN_THREAD);
		cdi.draw_bottom_stub(IN_THREAD);
	}

	if (stubs & border_impl::base::left_stub())
	{
		cdi.draw_left_pad(IN_THREAD);
		cdi.draw_left_stub(IN_THREAD);
	}

	if (stubs & border_impl::base::right_stub())
	{
		cdi.draw_right_pad(IN_THREAD);
		cdi.draw_right_stub(IN_THREAD);
	}
}

void border_implObj::composite_line(ONLY IN_THREAD,
				    const draw_info &di,
				    const background_color &c) const
{
	// Now we'll use the pixmap as a mask to compose colors[n] into the
	// area.
	picture::base::clip_mask mask(di.area_picture,
				      di.mask_pixmap,
				      0, 0);

	di.area_picture->composite(c->get_current_color(IN_THREAD),

				   // source coordinate for colors[n].

				   di.area_rectangle.x,
				   di.area_rectangle.y,

				   // This is the top/left coordinates in
				   // area_picture where the border is drawn.
				   0, 0,

				   // We're drawing "width"-long horizontal
				   // border, that's "calculate_border_height"
				   // call.
				   di.area_rectangle.width,
				   di.area_rectangle.height,
				   render_pict_op::op_over);
}

// We need to calculate the centerline of a vertical border.
//
// To compute the centerline, two adjustments must be made.
//
// 1) the border is drawn inside a vertical swath that's
//    area_rectangle.width tall, that's at least
//    horizontal_border_width.
//
// 2) the actual width of the border is "width", and the "official"
//    border width is calculated_border_width, which is at least
//    "width".
//
// We're going to draw inside area_picture, so for adjustment #1,
// shift right by half the difference.
//
// Then, for adjustment #2, move right by half that difference, as well.

coord_t border_implObj::compute_xcenter(const draw_info &di) const
{
	return coord_t::truncate
		((di.area_rectangle.width - calculated_border_width)/2
		 +
		 (calculated_border_width - width)/2
		 // But then, we need to compute the centerline,
		 // 'cause that's how graphic contexts work.
		 + width/2);
}

std::tuple<coord_t, coord_t>
border_implObj::compute_xleftright(coord_t xcenter) const
{
	auto l=xcenter - width/2;

	auto r=l+width;

	return { coord_t::truncate(l), coord_t::truncate(r) };
}

// We need to calculate the centerline of a horizontal border.
//
// To compute the centerline, two adjustments must be made.
//
// 1) the border is drawn inside a horizontal swath that's
//    area_rectangle.height tall, that's at least
//    horizontal_border_height.
//
// 2) the actual height of the border is "height", and the "official"
//    border height is calculated_border_height, which is at least
//    "height".
//
// We're going to draw inside area_picture, so for adjustment #1,
// shift down by half the difference.
//
// Then, for adjustment #2, move down by half that difference, as well.

coord_t border_implObj::compute_ycenter(const draw_info &di) const
{
	return coord_t::truncate
		((di.area_rectangle.height - calculated_border_height)/2
		 +
		 (calculated_border_height - height)/2
		 // But then, we need to compute the centerline,
		 // 'cause that's how graphic contexts work.
		 + height / 2);
}

std::tuple<coord_t, coord_t>
border_implObj::compute_ytopbottom(coord_t ycenter) const
{
	auto t=ycenter - height/2;

	auto b=t+height;

	return { coord_t::truncate(t), coord_t::truncate(b) };
}

void border_implObj::mask_gc_clear(const draw_info &di)
{
	gc::base::properties props;

	props.background(0);
	props.function(gc::base::function::CLEAR);
	di.mask_gc->fill_rectangle(0, 0,
				   di.area_rectangle.width,
				   di.area_rectangle.height, props);
}

void border_implObj::mask_segment(const draw_info &di,
				  coord_t absolute_offset,
				  dim_t line_width,
				  coord_t x1, coord_t y1,
				  coord_t x2, coord_t y2) const
{
	gc::base::properties props;

	props.line_width((dim_t::value_type)line_width);
	props.foreground(1);

	if (dashes.empty())
	{
		props.line_style(gc::base::line_style::solid);
	}
	else
	{
		props.line_style(gc::base::line_style::on_off);
		props.dashes_offset=(dim_t::value_type)
			(coord_t::value_type(absolute_offset)
			 % dim_t::value_type(calculated_dashes_sum));
		props.dashes=dashes;
	}

	di.mask_gc->segment(x1, y1, x2, y2, props);
}

void border_implObj::mask_segment_xor(const draw_info &di,
				      dim_t line_width,
				      coord_t x1, coord_t y1,
				      coord_t x2, coord_t y2) const
{
	gc::base::properties props;

	props.line_width((dim_t::value_type)line_width);
	props.foreground(1);
	props.function(gc::base::function::XOR);
	di.mask_gc->segment(x1, y1, x2, y2, props);
}

LIBCXXW_NAMESPACE_END
