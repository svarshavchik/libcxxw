/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "border_impl.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/gc.H"
#include "grid_element.H"
#include "element.H"
#include "draw_info.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

border_implObj::border_implObj()=default;

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

	// left_pad and right_pad are the starting (x,y) coordinates
	// for the real border.
	coord_t x, y;

	// Adding left_half_width and top_half_height to (x, y)
	// produces the center point.
	coord_t center_x, center_y;

	corner_draw_info(const draw_info &di,
			 const border_implObj &me);

	// Draw extra lines in the padding areas.

	void draw_top_pad(const grid_elementptr &left_element,
			  const grid_elementptr &right_element)
	{
		me.draw_vertical(di, 0, top_pad, left_element, right_element);
	}

	void draw_bottom_pad(const grid_elementptr &left_element,
			     const grid_elementptr &right_element)
	{
		me.draw_vertical(di,
				 coord_t::truncate(top_pad
						   +me.calculated_border_height
						   ),
				 bottom_pad,
				 left_element, right_element);
	}

	void draw_left_pad(const grid_elementptr &top_element,
			   const grid_elementptr &bottom_element)
	{
		me.draw_horizontal(di, 0, left_pad, top_element, bottom_element);
	}

	void draw_right_pad(const grid_elementptr &top_element,
			    const grid_elementptr &bottom_element)
	{
		me.draw_horizontal(di,
				   coord_t::truncate(left_pad
						     +me.calculated_border_width
						     ),
				   right_pad,
				   top_element, bottom_element);
	}

	// Now, draw the corresponding line to the center of the border.

	void draw_top_stub(const grid_elementptr &left_element,
			   const grid_elementptr &right_element)
	{
		me.draw_vertical(di, y, top_half_height,
				 left_element, right_element);
	}

	void draw_bottom_stub(const grid_elementptr &left_element,
			      const grid_elementptr &right_element)
	{
		me.draw_vertical(di, center_y, bottom_half_height,
				 left_element, right_element);
	}

	void draw_left_stub(const grid_elementptr &top_element,
			    const grid_elementptr &bottom_element)
	{
		me.draw_horizontal(di, 0, left_half_width,
				   top_element, bottom_element);
	}

	void draw_right_stub(const grid_elementptr &top_element,
			     const grid_elementptr &bottom_element)
	{
		me.draw_horizontal(di, center_x, right_half_width,
				   top_element, bottom_element);
	}

	// Fill the corner from that corner's element's background color

	// Used only when drawing a straight corner.

	void topleft_background_fill(IN_THREAD_ONLY,
				     const surrounding_elements_info &elements)
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

	void topright_background_fill(IN_THREAD_ONLY,
				      const surrounding_elements_info &elements)
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

	void bottomleft_background_fill(IN_THREAD_ONLY,
					const surrounding_elements_info
					&elements)
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

	void bottomright_background_fill(IN_THREAD_ONLY,
					 const surrounding_elements_info
					 &elements)
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

void border_implObj::draw_horizontal(const draw_info &di,
				     const grid_elementptr &top_element,
				     const grid_elementptr &bottom_element) const
{
	if (no_horizontal_border(di))
		return;

	draw_horizontal(di, 0, di.area_rectangle.width,
			top_element, bottom_element);
}

void border_implObj::draw_horizontal(const draw_info &di,
				     coord_t x,
				     dim_t length,
				     const grid_elementptr &top_element,
				     const grid_elementptr &bottom_element) const
{
	if (length == 0)
		return; // Corner case.
	// The border's top/left coordinate is (x, y) in area_picture, whose
	// top/left coordinate is (area_rectangle.x, area_rectangle.y) in
	// its container.
	//
	// Therefore, absolute_x/y is going to be the border's absolute
	// starting position.

	coord_t absolute_x=coord_t::truncate(di.area_rectangle.x+x);

	// Clear up the 1-bit masking pixmap, for the drawn border.

	mask_gc_clear(di);

	auto ycenter=compute_ycenter(di);

	// Draw a horizontal line (0, ycenter)-(width, ycenter),
	// that's "height" pixels thick.

	mask_segment(di, absolute_x, height,
		     x, ycenter,
		     coord_t::truncate(x+length), ycenter);

	composite_line(di, 0);
}

// Same logic as draw_horizontal, but in the other direction.

void border_implObj::draw_vertical(const draw_info &di,
				   const grid_elementptr &left_element,
				   const grid_elementptr &right_element) const
{
	if (no_vertical_border(di))
		return;

	draw_vertical(di, 0, di.area_rectangle.height,
		      left_element, right_element);
}

void border_implObj::draw_vertical(const draw_info &di,
				   coord_t y,
				   dim_t length,
				   const grid_elementptr &left_element,
				   const grid_elementptr &right_element) const
{
	if (length == 0)
		return;

	coord_t absolute_y=coord_t::truncate(di.area_rectangle.y+y);

	mask_gc_clear(di);

	auto xcenter=compute_xcenter(di);

	mask_segment(di, absolute_y, width,
		     xcenter, y,
		     xcenter, coord_t::truncate(y+length));

	composite_line(di, 0);
}

void border_implObj::draw_corner(IN_THREAD_ONLY,
				 const draw_info &di, int which_corners,
				 const surrounding_elements_info &elements
				 ) const
{
	if (no_corner_border(di))
		return;

	corner_draw_info cdi{di, *this};

	bool tl=which_corners & border_impl::base::cornertl() ? true:false;
	bool tr=which_corners & border_impl::base::cornertr() ? true:false;
	bool bl=which_corners & border_impl::base::cornerbl() ? true:false;
	bool br=which_corners & border_impl::base::cornerbr() ? true:false;

	if (tl || tr)
		cdi.draw_top_pad(elements.topleft, elements.topright);

	if (bl || br)
		cdi.draw_bottom_pad(elements.bottomleft, elements.bottomright);

	if (tl || bl)
		cdi.draw_left_pad(elements.topleft, elements.bottomleft);

	if (tr || br)
		cdi.draw_right_pad(elements.topright, elements.bottomright);

	// If which_corners is a single corner, we have special code for that.

	switch (which_corners) {
	case border_impl::base::cornertl():
		draw_cornertl(IN_THREAD, di, cdi.x, cdi.y, elements.topleft);
		return;
	case border_impl::base::cornertr():
		draw_cornertr(IN_THREAD, di, cdi.x, cdi.y, elements.topright);
		return;
	case border_impl::base::cornerbl():
		draw_cornerbl(IN_THREAD, di, cdi.x, cdi.y, elements.bottomleft);
		return;
	case border_impl::base::cornerbr():
		draw_cornerbr(IN_THREAD, di, cdi.x, cdi.y,
			      elements.bottomright);
		return;
	}

	// Zero or multiple corners. We will draw a straight border around
	// each one, but start by filling each requested corner with the
	// corresponding element's background color.

	if (which_corners & border_impl::base::cornertl())
		cdi.topleft_background_fill(IN_THREAD, elements);

	if (which_corners & border_impl::base::cornertr())
		cdi.topright_background_fill(IN_THREAD, elements);

	if (which_corners & border_impl::base::cornerbl())
		cdi.bottomleft_background_fill(IN_THREAD, elements);

	if (which_corners & border_impl::base::cornerbr())
		cdi.bottomright_background_fill(IN_THREAD, elements);

	bool drew_top=false, drew_bottom=false,
		drew_left=false, drew_right=false;

	// If there are two adjacent corners, find the straggler in the
	// middle, and draw it.

	grid_elementptr dummy;

	if ( (tl && tr) && !bl && !br)
	{
		cdi.draw_top_stub(dummy, dummy);
		drew_top=true;
	}

	if ( (bl && br) && !tl && !tr)
	{
		cdi.draw_bottom_stub(dummy, dummy);
		drew_bottom=true;
	}

	if ( (tl && bl) && !tr && !br)
	{
		cdi.draw_left_stub(dummy, dummy);
		drew_left=true;
	}

	if ( (tr && br) && !tl && !bl)
	{
		cdi.draw_right_stub(dummy, dummy);
		drew_right=true;
	}

	// And now we can draw the full stroke.

	if ( (tl || tr) && !drew_top)
	{
		draw_vertical(di, cdi.y, cdi.top_half_height, dummy, dummy);
	}

	if ( (bl || br) && !drew_bottom)
	{
		draw_vertical(di, cdi.center_y, cdi.bottom_half_height,
			      dummy, dummy);

	}

	if ( (tl || bl) && !drew_left)
	{
		draw_horizontal(di, 0, cdi.left_half_width,
				dummy, dummy);
	}

	if ( (tr || br) && !drew_right)
	{
		draw_horizontal(di, cdi.center_x, cdi.right_half_width,
				dummy, dummy);
	}
}

void border_implObj::draw_cornertl(IN_THREAD_ONLY,
				   const draw_info &di,
				   coord_t x,
				   coord_t y,
				   const grid_elementptr &element) const
{
	if (hradius != 0 && vradius != 0)
	{
		draw_round_corner(IN_THREAD,
				  di,
				  x, y,
				  false, false,
				  270 * 64, 90 * 64,
				  element);
		return;
	}
	draw_square_corner(di, x, y);
}

void border_implObj::draw_cornertr(IN_THREAD_ONLY,
				   const draw_info &di,
				   coord_t x,
				   coord_t y,
				   const grid_elementptr &element) const
{
	if (hradius != 0 && vradius != 0)
	{
		draw_round_corner(IN_THREAD,
				  di,
				  x, y,
				  true, false,
				  180 * 64, 90 * 64,
				  element);
		return;
	}
	draw_square_corner(di, x, y);
}

void border_implObj::draw_cornerbl(IN_THREAD_ONLY,
				   const draw_info &di,
				   coord_t x,
				   coord_t y,
				   const grid_elementptr &element) const
{
	if (hradius != 0 && vradius != 0)
	{
		draw_round_corner(IN_THREAD,
				  di,
				  x, y,
				  false, true,
				  90 * 64, -90 * 64,
				  element);
		return;
	}
	draw_square_corner(di, x, y);
}

void border_implObj::draw_cornerbr(IN_THREAD_ONLY,
				   const draw_info &di,
				   coord_t x,
				   coord_t y,
				   const grid_elementptr &element) const
{
	if (hradius != 0 && vradius != 0)
	{
		draw_round_corner(IN_THREAD,
				  di,
				  x, y,
				  true, true,
				  180 * 64, -90 * 64,
				  element);
		return;
	}
	draw_square_corner(di, x, y);
}

void border_implObj::draw_square_corner(const draw_info &di,
					coord_t x,
					coord_t y) const
{
	mask_gc_clear(di);

	gc::base::properties props;

	props.background(1);
	props.function(gc::base::function::SET);
	di.mask_gc->fill_rectangle(x, y,
				   calculated_border_width,
				   calculated_border_height, props);
	composite_line(di, 0);
}

// The x & y coordinates we use specify the center of the arc.
//
// X protocol requests specify the arc as a bounding rectangle.
//
// Calculate the X protocol arc given the x/y center coordinates, and the
// horizontal and vertical radius.

static gc::base::arc compute_arc(coord_t x,
				 coord_t y,
				 dim_squared_t hradius,
				 dim_squared_t vradius,
				 int16_t angle1,
				 int16_t angle2)
{
	x=coord_t::truncate(x-dim_squared_t::value_type(hradius));
	y=coord_t::truncate(y-dim_squared_t::value_type(vradius));

	// The bounding rectangle is twice the horizontal and vertical radius,
	// but X protocol uses an inclusive bounding rectangle, hence the need
	// to subtract 1.

	dim_t h=dim_t::truncate(hradius*2-1);
	dim_t v=dim_t::truncate(vradius*2-1);

	return {x, y, h, v, angle1, angle2};
}

void border_implObj::draw_round_corner(IN_THREAD_ONLY,
				       const draw_info &di,
				       coord_t x,
				       coord_t y,
				       bool add_width,
				       bool add_height,
				       int16_t angle1,
				       int16_t angle2,
				       const grid_elementptr &element) const
{
	if (add_width)
		x=coord_t::truncate(x+calculated_border_width);

	if (add_height)
		y=coord_t::truncate(y+calculated_border_height);

	mask_gc_clear(di);
	gc::base::properties props;

	props.background(1);
	props.function(gc::base::function::SET);

	// First, fill in the entire pie from the corner border.

	di.mask_gc->fill_arc(compute_arc(x, y,
					 inner_hradius() + width+1,
					 inner_vradius() + height+1,
					 angle1,
					 angle2), props);

	if (inner_hradius() > 0)
	{
		// Before clearing the inner border, use this mask to
		// fill the inner border with the element's background_color

		if (element)
		{
			auto &e_draw_info=element->grid_element->impl
				->get_draw_info(IN_THREAD);

			auto xy=e_draw_info.background_xy_to
				(di.area_rectangle.x,
				 di.area_rectangle.y);

			picture::base::clip_mask mask(di.area_picture,
						      di.mask_pixmap,
						      0, 0);

			di.area_picture->impl->composite
				(e_draw_info.window_background,
				 xy.first, xy.second,
				 0, 0,
				 di.area_rectangle.width,
				 di.area_rectangle.height);
		}

		// Now, clear the inner border.

		props.function(gc::base::function::CLEAR);

		di.mask_gc->fill_arc(compute_arc(x, y,
						 inner_hradius()+dim_t{1},
						 inner_vradius()+dim_t{1},
						 angle1,
						 angle2), props);

		// Redraw the outer border, to make sure there's at least a
		// 1-pixel rounded line.
		props.function(gc::base::function::SET);
		di.mask_gc->draw_arc(compute_arc(x, y,
						 inner_hradius() + width,
						 inner_vradius() + height,
						 angle1,
						 angle2), props);
	}

	composite_line(di, 0);
}

void border_implObj::draw_stubs(const draw_info &di, int stubs) const
{
	if (no_corner_border(di))
		return;

	corner_draw_info cdi{di, *this};

	// We don't need to clear to each elements' background colors,
	// corner_border has already done that, on this code path.

	grid_elementptr dummy;

	if (stubs & border_impl::base::top_stub())
	{
		cdi.draw_top_pad(dummy, dummy);
		cdi.draw_top_stub(dummy, dummy);
	}

	if (stubs & border_impl::base::bottom_stub())
	{
		cdi.draw_bottom_pad(dummy, dummy);
		cdi.draw_bottom_stub(dummy, dummy);
	}

	if (stubs & border_impl::base::left_stub())
	{
		cdi.draw_left_pad(dummy, dummy);
		cdi.draw_left_stub(dummy, dummy);
	}

	if (stubs & border_impl::base::right_stub())
	{
		cdi.draw_right_pad(dummy, dummy);
		cdi.draw_right_stub(dummy, dummy);
	}
}

void border_implObj::composite_line(const draw_info &di, size_t n) const
{
	// Now we'll use the pixmap as a mask to compose colors[n] into the
	// area.
	picture::base::clip_mask mask(di.area_picture,
				      di.mask_pixmap,
				      0, 0);

	di.area_picture->composite(colors.at(n),

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
				   di.area_rectangle.height);
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

LIBCXXW_NAMESPACE_END
