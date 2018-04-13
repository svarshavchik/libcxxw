/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "corner_border.H"
#include "grid_element.H"
#include "current_border_impl.H"
#include "border_impl.H"
#include "straight_border.H"
#include "x/w/pictformat.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/impl/scratch_buffer.H"
#include "x/w/impl/scratch_and_mask_buffer_draw.H"
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
::implObj(const container_impl &container)
	: implObj(container, container->get_window_handler())
{
}


corner_borderObj::implObj
::implObj(const container_impl &container,
	  generic_windowObj::handlerObj &h)
	: scratch_and_mask_buffer_draw<child_elementObj>
	("cornermask@libcxx.com",
	 h.get_width()/20+1,
	 h.get_height()/20+1, container,
	 child_element_init_params{"corner@libcxx.com"})
{
}

corner_borderObj::implObj::~implObj()=default;

void corner_borderObj::implObj::updated(ONLY IN_THREAD)
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
	schedule_redraw(IN_THREAD);
}

void corner_borderObj::implObj::initialize(ONLY IN_THREAD)
{
	compute_metrics(IN_THREAD);
	superclass_t::initialize(IN_THREAD);
}

void corner_borderObj::implObj::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	compute_metrics(IN_THREAD);
	superclass_t::theme_updated(IN_THREAD, new_theme);
}

void corner_borderObj::implObj::compute_metrics(ONLY IN_THREAD)
{
	auto &elements=surrounding_elements(IN_THREAD);

	dim_t width;
	dim_t height;

	if (elements.fromtop_border)
	{
		auto b=elements.fromtop_border->impl->best_border(IN_THREAD);

		if (b)
			width=b->border(IN_THREAD)->calculated_border_width;
	}

	if (elements.frombottom_border)
	{
		auto b=elements.frombottom_border->impl->best_border(IN_THREAD);

		if (b)
		{
			auto width2=b->border(IN_THREAD)
				->calculated_border_width;

			if (width2 > width)
				width=width2;
		}
	}

	if (elements.fromleft_border)
	{
		auto b=elements.fromleft_border->impl->best_border(IN_THREAD);

		if (b)
			height=b->border(IN_THREAD)->calculated_border_height;
	}

	if (elements.fromright_border)
	{
		auto b=elements.fromright_border->impl->best_border(IN_THREAD);

		if (b)
		{
			auto height2=b->border(IN_THREAD)
				->calculated_border_height;

			if (height2 > height)
				height=height2;
		}
	}

	metrics::axis h{width, width, width};
	metrics::axis v{height, height, height};

	get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD, h, v);

	// cached_draw_info needs to be recomputed now.
	cached_draw_info.reset();
}

void corner_borderObj::implObj::do_draw(ONLY IN_THREAD,
					const draw_info &di,
					const picture &area_picture,
					const pixmap &area_pixmap,
					const gc &area_gc,
					const picture &mask_picture,
					const pixmap &mask_pixmap,
					const gc &mask_gc,
					const clip_region_set &clipped,
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

	auto &info=get_cached_draw_info(IN_THREAD);

	if (info.same_border_everywhere)
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

	dim_t leftwidth=area_entire_rect.width/2;
	dim_t rightwidth=area_entire_rect.width - leftwidth;
	dim_t topheight=area_entire_rect.height/2;
	dim_t bottomheight=area_entire_rect.height - topheight;

	// Now, for the corners where we won't call draw_corner(), take care
	// of filling that corner with the correposnding element's background
	// color ourselves, here.

	if ((info.all_corners & border_impl::base::cornertl())
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

	if ((info.all_corners & border_impl::base::cornertr())
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


	if ((info.all_corners & border_impl::base::cornerbl())
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

	if ((info.all_corners & border_impl::base::cornerbr())
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

	for (const auto &b:info.stubs)
	{
#ifdef DRAW_STUB_BORDER
		DRAW_STUB_BORDER();
#endif
		std::get<0>(b)->draw_stubs(IN_THREAD,
					   border_draw_info, std::get<1>(b));
	}

	for (const auto &b:info.corners)
	{
		std::get<0>(b)->draw_corner(IN_THREAD,
					    border_draw_info, std::get<1>(b),
					    elements);
	}
}

corner_borderObj::implObj::cached_draw_info_s
&corner_borderObj::implObj::get_cached_draw_info(ONLY IN_THREAD)
{
	if (cached_draw_info)
		return *cached_draw_info;

	auto &info=cached_draw_info.emplace();

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
		info.same_border_everywhere=true;
		return info;
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

	auto &corners=info.corners;
	auto &stubs=info.stubs;

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

#if 0
	struct debug_info {
		const char *name;
		straight_borderptr b;
	} d[]={ {"top", elements.fromtop_border},
		{"bottom", elements.frombottom_border},
		{"left", elements.fromleft_border},
		{"right", elements.fromright_border}};

	for (const auto &dd:d)
	{
		if (!dd.b)
			continue;

		const auto &b=dd.b->impl->best_border(IN_THREAD);

		if (!b)
			continue;

		const border_info &bb=*b->border(IN_THREAD);

		std::cout << dd.name << ": "
			  << bb.width << "x" << bb.height
			  << ", hradius=" << bb.hradius
			  << ", vradius=" << bb.vradius
			  << std::endl;
	}

	std::cout << "SIZE: " << stubs.size() << std::endl;
#endif

	/*
	** Plan B for when one border enters a corner area, part 2.
	**
	** If we have a lonely stub, and no corners, just extend the stub
	** to the other side, in case there's something "solid" there.
	**
	** If there's another stub, from any other edge, they'll meet in the
	** middle. If there's another corner joining the two of the other
	** edges, it's something weird, so we'll just keep on truckin'.
	*/

	if (stubs.size() == 1 && corners.empty())
	{
		auto first=*stubs.begin();

		int &val=std::get<int>(first);

		if (val & (left|right))
			val ^= (left|right);
		else
			val ^= (top|bottom);

		stubs.push_back(first);
	}

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

	info.all_corners=border_impl::base::cornertl() |
		border_impl::base::cornertr() |
		border_impl::base::cornerbl() |
		border_impl::base::cornerbr();

	for (const auto &b:corners)
		info.all_corners &= ~std::get<1>(b);

	// Draw corners according to their relative "importantness".
	std::sort(stubs.begin(), stubs.end(), sorter);
	std::sort(corners.begin(), corners.end(), sorter);

	return info;
}

//////////////////////////////////////////////////////////////////////////////
//
// Examine two borders that meet in a corner. If they're the same border,
// they can be drawn as a corner.

void corner_borderObj::implObj::surrounding_elements_and_borders
::get_same_border(ONLY IN_THREAD,
		  from_b border1,
		  from_b border2,
		  int which_corners,
		  int &flags,
		  int mask,
		  std::vector<std::tuple<const_border_impl, int>> &borders)
{
	auto &b1=(*this).*border1;
	auto &b2=(*this).*border2;

	// Do we have two borders here?
	if (!b1 || !b2)
		return;

	auto best1=b1->impl->best_border(IN_THREAD);
	auto best2=b2->impl->best_border(IN_THREAD);

	// Are they the same?
	if (best1 != best2)
		return; // Different borders.

	if (!best1)
		return; // Same borders, but both of them are zippo.

	borders.emplace_back(best1->border(IN_THREAD), which_corners);
	flags &= mask;
}

void corner_borderObj::implObj::surrounding_elements_and_borders
::pick_border(ONLY IN_THREAD,
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
