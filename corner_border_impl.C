/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "corner_border.H"
#include "grid_element.H"
#include "scratch_buffer.H"
#include "current_border_impl.H"
#include "border_impl.H"
#include "straight_border.H"
#include "scratch_and_mask_buffer_draw.H"
#include "x/w/pictformat.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include <algorithm>

LIBCXXW_NAMESPACE_START

surrounding_elements_info::surrounding_elements_info()=default;

surrounding_elements_info::~surrounding_elements_info()=default;

surrounding_elements_info
::surrounding_elements_info(const surrounding_elements_info &)=default;

corner_borderObj::implObj::surrounding_elements_and_borders
::surrounding_elements_and_borders()=default;

corner_borderObj::implObj::surrounding_elements_and_borders
::~surrounding_elements_and_borders()=default;

corner_borderObj::implObj::surrounding_elements_and_borders
::surrounding_elements_and_borders(const surrounding_elements_and_borders &)=default;

bool corner_borderObj::implObj::surrounding_elements_and_borders
::operator==(const surrounding_elements_and_borders &o) const
{
	return topleft == o.topleft &&
		topright == o.topright &&
		bottomleft == o.bottomleft &&
		bottomright == o.bottomright &&
		fromtop_border == o.fromtop_border &&
		frombottom_border == o.frombottom_border &&
		fromleft_border == o.fromleft_border &&
		fromright_border == o.fromright_border;
}

corner_borderObj::implObj
::implObj(const ref<containerObj::implObj> &container)
	: implObj(container, container->get_window_handler())
{
}

corner_borderObj::implObj
::implObj(const ref<containerObj::implObj> &container,
	  generic_windowObj::handlerObj &h)
	: scratch_and_mask_buffer_draw<child_elementObj>
	("cornermask@libcxx",
	 h.get_width()/20+1,
	 h.get_height()/20+1, container,
	 ({
		 metrics::horizvert_axi m;

		 m.horizontal_alignment=halign::fill;
		 m.vertical_alignment=valign::fill;

		 m;
	 }), "corner@libcxx")
{
}

corner_borderObj::implObj::~implObj()=default;

void corner_borderObj::implObj::updated(IN_THREAD_ONLY)
{
	bool unchanged=surrounding_elements(IN_THREAD) ==
		old_surrounding_elements(IN_THREAD);

	// Make sure and drop references to old elements.
	old_surrounding_elements(IN_THREAD)=surrounding_elements_and_borders();

	if (unchanged)
		return;

#ifdef CHANGED_CORNER_BORDERS
	CHANGED_CORNER_BORDERS();
#endif

	compute_metrics(IN_THREAD);
	schedule_redraw_if_visible(IN_THREAD);
}

void corner_borderObj::implObj::initialize(IN_THREAD_ONLY)
{
	compute_metrics(IN_THREAD);
}

void corner_borderObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	compute_metrics(IN_THREAD);
}

void corner_borderObj::implObj::compute_metrics(IN_THREAD_ONLY)
{
	auto &elements=surrounding_elements(IN_THREAD);

	// Compile a list of all the straight borders we have.

	std::vector<straight_border> all_borders;

	all_borders.reserve(4);

	if (elements.fromtop_border)
		all_borders.push_back(elements.fromtop_border);

	if (elements.frombottom_border)
		all_borders.push_back(elements.frombottom_border);

	if (elements.fromleft_border)
		all_borders.push_back(elements.fromleft_border);

	if (elements.fromright_border)
		all_borders.push_back(elements.fromright_border);

	// Our metrics consist of the largest width+height of all borders
	// together.
	dim_t width;
	dim_t height;

	for (const auto &b:all_borders)
	{
		const auto &current_border=b->impl->best_border(IN_THREAD)
			->border(IN_THREAD);

		if (current_border->calculated_border_width > width)
			width=current_border->calculated_border_width;

		if (current_border->calculated_border_height > height)
			height=current_border->calculated_border_height;
	}

	get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      {width, width, width},
				      {height, height, height});
}

void corner_borderObj::implObj::do_draw(IN_THREAD_ONLY,
					const draw_info &di,
					const picture &area_picture,
					const pixmap &area_pixmap,
					const gc &area_gc,
					const picture &mask_picture,
					const pixmap &mask_pixmap,
					const gc &mask_gc,
					const rectangle &area_entire_rect)
{
	border_implObj::draw_info border_draw_info{
				 area_picture,
					 area_entire_rect,
					 area_pixmap,
					 mask_picture,
					 mask_pixmap,
					 mask_gc,
					 di.absolute_location.x,
					 di.absolute_location.y};

	auto &elements=surrounding_elements(IN_THREAD);

	// Start handling all possibilities...

	// Possibility #1: same border everywhere.

	if (elements.fromtop_border ==
	    elements.frombottom_border &&
	    elements.fromtop_border ==
	    elements.fromleft_border &&
	    elements.fromtop_border ==
	    elements.fromright_border)
	{
		if (!elements.fromtop_border)
			return; // The "same border" is no border.

		const auto &b=elements.fromtop_border->impl
			->best_border(IN_THREAD);

		if (!b)
			return; // Still no border.

		const auto &border_impl=b->border(IN_THREAD);

		// We'll pass the elements to draw_corner(), it'll take care
		// of filling in any gaps with each corner's background color.

		border_impl->draw_corner(IN_THREAD,
					 border_draw_info,
					 border_impl::base::cornertl() |
					 border_impl::base::cornertr() |
					 border_impl::base::cornerbl() |
					 border_impl::base::cornerbr(),
					 elements);
		return;
	}

	// Compare adjacent borders, if they're the same we can use that
	// border to draw that particular corner.
	//
	// We can't draw the corner border just yet. We must first draw
	// the stubs for any non-matching borders (borders that don't
	// match any of its two adjacent borders.
	//
	// Start with the full list of left+right+top_bottom stubs needed,
	// and get_same_border() removes each one, if it determines that the
	// particular corner that involves two adjacent borders can be drawn.
	//
	// What would be left would be the stubs to draw.

	std::vector<std::tuple<const_border_impl, int>> corners;
	std::vector<std::tuple<const_border_impl, int>> stubs;
	corners.reserve(3);
	stubs.reserve(3);

	const int left=border_impl::base::left_stub();
	const int right=border_impl::base::right_stub();
	const int top=border_impl::base::top_stub();
	const int bottom=border_impl::base::bottom_stub();

	int need=left|right|top|bottom;

	elements.get_same_border(IN_THREAD,
				 &surrounding_elements_and_borders::fromleft_border,
				 &surrounding_elements_and_borders::fromtop_border,
				 border_impl::base::cornertl(),
				 need,
				 ~(left | top),
				 corners);

	elements.get_same_border(IN_THREAD,
				 &surrounding_elements_and_borders::fromtop_border,
				 &surrounding_elements_and_borders::fromright_border,
				 border_impl::base::cornertr(),
				 need,
				 ~(right | top),
				 corners);

	elements.get_same_border(IN_THREAD,
				 &surrounding_elements_and_borders::frombottom_border,
				 &surrounding_elements_and_borders::fromright_border,
				 border_impl::base::cornerbr(),
				 need,
				 ~(right | bottom),
				 corners);

	elements.get_same_border(IN_THREAD,
				 &surrounding_elements_and_borders::frombottom_border,
				 &surrounding_elements_and_borders::fromleft_border,
				 border_impl::base::cornerbl(),
				 need,
				 ~(left | bottom),
				 corners);

	if (need & left)
		elements.pick_border(IN_THREAD,
				     &surrounding_elements_and_borders::fromleft_border,
				     left,
				     stubs);

	if (need & right)
		elements.pick_border(IN_THREAD,
				     &surrounding_elements_and_borders::fromright_border,
				     right,
				     stubs);

	if (need & top)
		elements.pick_border(IN_THREAD,
				     &surrounding_elements_and_borders::fromtop_border,
				     top,
				     stubs);

	if (need & bottom)
		elements.pick_border(IN_THREAD,
				     &surrounding_elements_and_borders::frombottom_border,
				     bottom,
				     stubs);

	auto sorter=[]
		(const auto &a, const auto &b)
		{
			return std::get<0>(a)->compare(*std::get<0>(b));
		};

	// Based on the detected corners, figure out which quadrants will
	// not have a border drawn; hence we will need to clear them manually
	// to the element's background_color, ourselves.
	//
	// We'll start with all four corners, then remove whatever we find
	// in the corners vector. That corner's background color will get
	// taken care of by draw_corner(), when we call it below.

	int all_corners=border_impl::base::cornertl() |
		border_impl::base::cornertr() |
		border_impl::base::cornerbl() |
		border_impl::base::cornerbr();

	for (const auto &b:corners)
		all_corners &= ~std::get<1>(b);

	dim_t leftwidth=area_entire_rect.width/2;
	dim_t rightwidth=area_entire_rect.width - leftwidth;
	dim_t topheight=area_entire_rect.height/2;
	dim_t bottomheight=area_entire_rect.height - topheight;

	// Now, for the corners where we won't call draw_corner(), take care
	// of filling that corner with the correposnding element's background
	// color ourselves, here.

	if ((all_corners & border_impl::base::cornertl())
	    && elements.topleft)
	{
		auto &e_draw_info=elements.topleft->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=e_draw_info.background_xy_to(di);

		area_picture->impl->composite(e_draw_info.window_background,
					      xy.first,
					      xy.second,
					      0, 0,
					      leftwidth,
					      topheight);
	}

	if ((all_corners & border_impl::base::cornertr())
	    && elements.topright)
	{
		auto &e_draw_info=elements.topright->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=e_draw_info.background_xy_to(di,
						     coord_t::truncate(leftwidth),
						     0);

		area_picture->impl->composite(e_draw_info.window_background,
					      xy.first,
					      xy.second,
					      coord_t::truncate(leftwidth), 0,
					      rightwidth,
					      topheight);
	}


	if ((all_corners & border_impl::base::cornerbl())
	    && elements.bottomleft)
	{
		auto &e_draw_info=elements.bottomleft->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=e_draw_info.background_xy_to(di,
						     0,
						     coord_t::truncate(topheight));

		area_picture->impl->composite(e_draw_info.window_background,
					      xy.first,
					      xy.second,
					      0,
					      coord_t::truncate(topheight),
					      leftwidth,
					      bottomheight);
	}

	if ((all_corners & border_impl::base::cornerbr())
	    && elements.bottomright)
	{
		auto &e_draw_info=elements.bottomright->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=e_draw_info.background_xy_to(di,
						     coord_t::truncate(leftwidth),
						     coord_t::truncate(topheight));

		area_picture->impl->composite(e_draw_info.window_background,
					      xy.first,
					      xy.second,
					      coord_t::truncate(leftwidth),
					      coord_t::truncate(topheight),
					      rightwidth,
					      bottomheight);
	}

	// Draw corners according to their relative "importantness".
	std::sort(stubs.begin(), stubs.end(), sorter);
	std::sort(corners.begin(), corners.end(), sorter);

	for (const auto &b:stubs)
	{
#ifdef DRAW_STUB_BORDER
		DRAW_STUB_BORDER();
#endif
		std::get<0>(b)->draw_stubs(border_draw_info, std::get<1>(b));
	}

	for (const auto &b:corners)
	{
		std::get<0>(b)->draw_corner(IN_THREAD,
					    border_draw_info, std::get<1>(b),
					    elements);
	}
}

//////////////////////////////////////////////////////////////////////////////

void corner_borderObj::implObj::surrounding_elements_and_borders
::get_same_border(IN_THREAD_ONLY,
		  from_b border1,
		  from_b border2,
		  int which_corners,
		  int &flags,
		  int mask,
		  std::vector<std::tuple<const_border_impl, int>> &borders)
{
	auto &b1=(*this).*border1;
	auto &b2=(*this).*border2;

	if (b1 || b2)
	{
		if (!b1 || !b2)
			return; // Different borders.

		if (b1->impl->best_border(IN_THREAD) !=
		    b2->impl->best_border(IN_THREAD))
			return; // Different borders.

		pick_border(IN_THREAD, border1,
			    which_corners,
			    borders);
	}

	flags &= mask;
}

void corner_borderObj::implObj::surrounding_elements_and_borders
::pick_border(IN_THREAD_ONLY,
	      from_b which_border,
	      int flag,
	      std::vector<std::tuple<const_border_impl, int>> &borders)
{
	const auto &from_border= (*this).*which_border;

	if (!from_border)
		return;

	const auto &b=from_border->impl->best_border(IN_THREAD);

	if (!b)
		return;

	const auto &border=b->border(IN_THREAD);

	borders.emplace_back(border, flag);
}

LIBCXXW_NAMESPACE_END
