/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "straight_border_impl.H"
#include "generic_window_handler.H"
#include "x/w/impl/draw_info.H"
#include "x/w/impl/scratch_and_mask_buffer_draw.H"
#include "grid_element.H"
#include "screen.H"
#include "x/w/scratch_buffer.H"
#include "x/w/pictformat.H"

LIBCXXW_NAMESPACE_START

border_source::border_source(const grid_elementptr &e) : e(e)
{
}

current_border_implptr border_source::get_border(ONLY IN_THREAD,
						 current_border_implptr
						 grid_elementObj::*which_one)
	const
{
	return e ? (*e).*which_one:current_border_implptr();
}

horizontal_straight_borderObj
::horizontal_straight_borderObj(const container_impl &c,
				const current_border_implptr &border_default)
	: horizontal_straight_borderObj(c, {}, {}, {}, {}, border_default)
{
}

horizontal_straight_borderObj
::horizontal_straight_borderObj(const container_impl &c,
				const grid_elementptr &element_1,
				const current_border_implptr &border1,
				const grid_elementptr &element_2,
				const current_border_implptr &border2,
				const current_border_implptr &border_default)
	: horizontal_impl(c,
			  metrics::horizvert_axi(),
			  // Same ID as in bordercontainer_impl.C
			  "horiz-border@libcxx.com",
			  element_1,
			  border1,
			  element_2,
			  border2,
			  border_default)
{
}

horizontal_straight_borderObj::~horizontal_straight_borderObj()=default;

straight_border straight_borderBase
::create_horizontal_border(ONLY IN_THREAD,
			   const container_impl &container_impl,
			   const border_source &element_above,
			   const border_source &element_below,
			   const current_border_implptr &default_border)
{
#ifdef CREATE_STRAIGHT_BORDER
	CREATE_STRAIGHT_BORDER();
#endif

	auto impl=ref<horizontal_straight_borderObj>
		::create(container_impl,

			 element_above.e,
			 element_above
			 .get_border(IN_THREAD,
				     &grid_elementObj::bottom_border),

			 element_below.e,
			 element_below
			 .get_border(IN_THREAD,
				     &grid_elementObj::top_border),

			 default_border);

	return ptrref_base::objfactory<straight_border>::create(impl);
}

vertical_straight_borderObj
::vertical_straight_borderObj(const container_impl &c,
			      const current_border_implptr &border_default)
	: vertical_straight_borderObj(c, {}, {}, {}, {}, border_default)
{
}

vertical_straight_borderObj
::vertical_straight_borderObj(const container_impl &c,
			      const grid_elementptr &element_1,
			      const current_border_implptr &border1,
			      const grid_elementptr &element_2,
			      const current_border_implptr &border2,
			      const current_border_implptr &border_default)
	: vertical_impl(c,
			metrics::horizvert_axi(),
			// Same ID as in bordercontainer_impl.C
			"vert-border@libcxx.com",
			element_1,
			border1,
			element_2,
			border2,
			border_default)
{
}

vertical_straight_borderObj::~vertical_straight_borderObj()=default;

straight_border straight_borderBase
::create_vertical_border(ONLY IN_THREAD,
			 const container_impl &container_impl,
			 const border_source &element_onleft,
			 const border_source &element_onright,
			 const current_border_implptr &default_border)
{
#ifdef CREATE_STRAIGHT_BORDER
	CREATE_STRAIGHT_BORDER();
#endif
	auto impl=ref<vertical_straight_borderObj>
		::create(container_impl,

			 element_onleft.e,
			 element_onleft
			 .get_border(IN_THREAD, &grid_elementObj::right_border),

			 element_onright.e,
			 element_onright
			 .get_border(IN_THREAD, &grid_elementObj::left_border),

			 default_border);

	return ptrref_base::objfactory<straight_border>::create(impl);
}

// We have an existing horizontal or a vertical border that lives where
// we are requesting an existence of a particular border. Check to see
// if the existing  border matches the requirements; if so return the
// existing border, otherwise create a new one.

straight_border straight_borderBase
::update_horizontal_border(ONLY IN_THREAD,
			   const straight_border &existing_border,
			   const border_source &element_above,
			   const border_source &element_below,
			   const current_border_implptr &default_border)
{
	const auto &b=existing_border->impl->borders(IN_THREAD);

	if (b.element_1 == element_above.e &&
	    b.element_2 == element_below.e &&
	    b.border_default == default_border)
	{
		// We also want to compare the actual borders, they must be
		// the same.
		if (element_above.get_border(IN_THREAD,
					     &grid_elementObj::bottom_border)
		    == b.border_1 &&
		    element_below.get_border(IN_THREAD,
					     &grid_elementObj::top_border)
		    == b.border_2)
			return existing_border;
	}
	return create_horizontal_border(IN_THREAD,
					existing_border->impl->child_container,
					element_above,
					element_below,
					default_border);
}

straight_border straight_borderBase
::update_vertical_border(ONLY IN_THREAD,
			 const straight_border &existing_border,
			 const border_source &element_onleft,
			 const border_source &element_onright,
			 const current_border_implptr &default_border)
{
	const auto &b=existing_border->impl->borders(IN_THREAD);

	if (b.element_1 == element_onleft.e &&
	    b.element_2 == element_onright.e &&
	    b.border_default == default_border)
	{
		// We also want to compare the actual borders, they must be
		// the same.
		if (element_onleft.get_border(IN_THREAD,
					      &grid_elementObj::right_border)
		    == b.border_1
		    &&
		    element_onright.get_border(IN_THREAD,
					       &grid_elementObj::left_border)
		    == b.border_2)
		return existing_border;
	}
	return create_vertical_border(IN_THREAD,
				      existing_border->impl->child_container,
				      element_onleft,
				      element_onright,
				      default_border);
}

straight_borderObj::implObj
::implObj(const container_impl &container,
	  const metrics::horizvert_axi &initial_metrics,
	  const char *scratch_buffer_label,
	  const grid_elementptr &element_1,
	  const current_border_implptr &border1,
	  const grid_elementptr &element_2,
	  const current_border_implptr &border2,
	  const current_border_implptr &border_default)
	: implObj(container, initial_metrics,
		  scratch_buffer_label,
		  container->get_window_handler(),
		  element_1, border1, element_2, border2, border_default)
{
}

straight_borderObj::implObj
::implObj(const container_impl &container,
	  const metrics::horizvert_axi &initial_metrics,
	  const char *scratch_buffer_label,
	  generic_windowObj::handlerObj &h,
	  const grid_elementptr &element_1,
	  const current_border_implptr &border1,
	  const grid_elementptr &element_2,
	  const current_border_implptr &border2,
	  const current_border_implptr &border_default)
	: scratch_and_mask_buffer_draw<child_elementObj>
	(std::string("mask-")+scratch_buffer_label, container,
	 child_element_init_params{std::string("area-")+scratch_buffer_label,
			initial_metrics}),
	borders_thread_only{element_1, border1, element_2, border2,
		border_default}
{
}

straight_borderObj::implObj::~implObj()=default;

const current_border_implptr &straight_borderObj::implObj
::best_border(ONLY IN_THREAD) const
{
	const auto &b=borders(IN_THREAD);

	if (b.border_1 && b.border_2)
	{
		return b.border_1->border(IN_THREAD)
			->compare(*b.border_2->border(IN_THREAD))
			? b.border_2:b.border_1;
	}

	if (b.border_1)
		return b.border_1;
	if (b.border_2)
		return b.border_2;

	return b.border_default;
}

void straight_borderObj::implObj::do_draw(ONLY IN_THREAD,
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
	border_implObj::draw_info
		border_draw_info{area_picture,
			area_entire_rect,
			area_pixmap,
			mask_picture,
			mask_pixmap,
			mask_gc,
			di.absolute_location.x,
			di.absolute_location.y};

	draw_horizvert(IN_THREAD, border_draw_info);
}

void straight_borderObj::implObj
::background_horizontal(ONLY IN_THREAD,
			const border_implObj::draw_info &bg) const
{
	elementptr element_1;
	elementptr element_2;

	if (borders(IN_THREAD).element_1)
	{
		element_1=borders(IN_THREAD).element_1->grid_element;
	}

	if (borders(IN_THREAD).element_2)
	{
		element_2=borders(IN_THREAD).element_2->grid_element;
	}

	bg.background_horizontal(IN_THREAD, element_1, element_2);
}

void straight_borderObj::implObj
::background_vertical(ONLY IN_THREAD,
		      const border_implObj::draw_info &bg) const
{
	elementptr element_1;
	elementptr element_2;

	if (borders(IN_THREAD).element_1)
	{
		element_1=borders(IN_THREAD).element_1->grid_element;
	}

	if (borders(IN_THREAD).element_2)
	{
		element_2=borders(IN_THREAD).element_2->grid_element;
	}

	bg.background_vertical(IN_THREAD, element_1, element_2);
}

LIBCXXW_NAMESPACE_END
