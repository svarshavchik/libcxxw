/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/scratch_buffer.H"
#include "x/w/impl/container.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/border_impl.H"
#include "corner_borderfwd.H"

LIBCXXW_NAMESPACE_START

borderlayoutmanagerObj::implObj::implObj(const container_impl &container_impl,
					 const ref<bordercontainer_implObj>
					 &bordercontainer_impl,
					 const element &initial_element,
					 halign element_halign,
					 valign element_valign)
	: singletonlayoutmanagerObj::implObj{container_impl, initial_element,
					     element_halign,
					     element_valign},
	  bordercontainer_impl{bordercontainer_impl}
{
}

borderlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager borderlayoutmanagerObj::implObj::create_public_object()
{
	return borderlayoutmanager::create(ref(this));
}

dim_t borderlayoutmanagerObj::implObj::get_left_padding(ONLY IN_THREAD)
{
	return dim_t::truncate(get_all_borders(IN_THREAD).left_pad +
			       bordercontainer_impl->get_border_hpad(IN_THREAD))
		;
}

dim_t borderlayoutmanagerObj::implObj::get_right_padding(ONLY IN_THREAD)
{
	return dim_t::truncate(get_all_borders(IN_THREAD).right_pad +
			       bordercontainer_impl->get_border_hpad(IN_THREAD))
		;
}

dim_t borderlayoutmanagerObj::implObj::get_top_padding(ONLY IN_THREAD)
{
	return dim_t::truncate(get_all_borders(IN_THREAD).top_pad +
			       bordercontainer_impl->get_border_vpad(IN_THREAD))
		;
}

dim_t borderlayoutmanagerObj::implObj::get_bottom_padding(ONLY IN_THREAD)
{
	return dim_t::truncate(get_all_borders(IN_THREAD).bottom_pad +
			       bordercontainer_impl->get_border_vpad(IN_THREAD))
		;
}

const borderlayoutmanagerObj::implObj::border_info &
borderlayoutmanagerObj::implObj::get_all_borders(ONLY IN_THREAD)
{
	if (cached_border_info)
		return *cached_border_info;

	auto lb=bordercontainer_impl->get_left_border(IN_THREAD)
		->border(IN_THREAD);
	auto rb=bordercontainer_impl->get_right_border(IN_THREAD)
		->border(IN_THREAD);
	auto tb=bordercontainer_impl->get_top_border(IN_THREAD)
		->border(IN_THREAD);
	auto bb=bordercontainer_impl->get_bottom_border(IN_THREAD)
		->border(IN_THREAD);

	// Which border is "better" for each corner.

	auto tl=lb->compare(*tb) ? tb:lb;
	auto tr=rb->compare(*tb) ? tb:rb;
	auto bl=lb->compare(*bb) ? bb:lb;
	auto br=rb->compare(*bb) ? bb:rb;

	// Compute each side's padding by choosing the "better" of the two
	// corner borders.

	dim_t left_pad=(tl->compare(*bl) ? bl:tl)->calculated_border_width;
	dim_t right_pad=(tr->compare(*br) ? br:tr)->calculated_border_width;

	dim_t top_pad=(tl->compare(*tr) ? tr:tl)->calculated_border_height;
	dim_t bottom_pad=(bl->compare(*br) ? br:bl)->calculated_border_height;

	// HOWEVER: if the given side's border is all wet, there is no padding.

	if (lb->no_border())
		left_pad=0;

	if (rb->no_border())
		right_pad=0;

	if (tb->no_border())
		top_pad=0;

	if (bb->no_border())
		bottom_pad=0;

	cached_border_info=
		{
		 lb, rb, tb, bb, tl, tr, bl, br, left_pad, right_pad, top_pad,
		 bottom_pad
		};

	return *cached_border_info;
}

rectangle borderlayoutmanagerObj::implObj
::padded_position(ONLY IN_THREAD, const element_impl &e)
{
	const auto &info=get_all_borders(IN_THREAD);

	dim_t hpad2=dim_t::truncate(info.left_pad+info.right_pad);
	dim_t vpad2=dim_t::truncate(info.top_pad+info.bottom_pad);

	auto &current_position=bordercontainer_impl->get_container_impl()
		.container_element_impl().data(IN_THREAD).current_position;

	dim_t padded_width=
		current_position.width > hpad2 ? current_position.width-hpad2:dim_t{};
	dim_t padded_height=
		current_position.height > vpad2 ? current_position.height-vpad2:dim_t{};

	return {coord_t::truncate(info.left_pad),
		coord_t::truncate(info.top_pad),
		padded_width, padded_height};
}

void borderlayoutmanagerObj::implObj
::child_background_color_changed(ONLY IN_THREAD,
				 const element_impl &child)
{
	// If the child has a background color we need to redraw the padding.

	if (child->has_own_background_color(IN_THREAD))
	{
		bordercontainer_impl->get_container_impl()
			.container_element_impl().schedule_redraw(IN_THREAD);
	}
}

void borderlayoutmanagerObj::implObj::theme_updated(ONLY IN_THREAD,
						    const defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);

	needs_recalculation(IN_THREAD);
}

void borderlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	// If the border changed, we need to redraw even if recalculate()
	// will do nothing, and we always need to clear the cached_border_info, in
	// order for it to be computed from scratch.

	if (current_left_border != bordercontainer_impl->get_left_border(IN_THREAD)
	    ->border(IN_THREAD) ||
	    current_right_border != bordercontainer_impl->get_right_border(IN_THREAD)
	    ->border(IN_THREAD) ||
	    current_top_border != bordercontainer_impl->get_top_border(IN_THREAD)
	    ->border(IN_THREAD) ||
	    current_bottom_border != bordercontainer_impl->get_bottom_border(IN_THREAD)
	    ->border(IN_THREAD))
	{
		cached_border_info.reset();

		const auto &info=get_all_borders(IN_THREAD);

		current_left_border=info.lb;
		current_right_border=info.rb;
		current_top_border=info.tb;
		current_bottom_border=info.bb;

		bordercontainer_impl->get_container_impl()
			.container_element_impl().schedule_redraw(IN_THREAD);
	}

	superclass_t::recalculate(IN_THREAD);
}

void borderlayoutmanagerObj::implObj::do_draw(ONLY IN_THREAD,
					      const draw_info &di,
					      clip_region_set &clip,
					      rectarea &drawn_areas)
{
	superclass_t::do_draw(IN_THREAD, di, clip, drawn_areas);

	drawn_areas.reserve(drawn_areas.size()+8);

	const auto &info=get_all_borders(IN_THREAD);

	auto &e=bordercontainer_impl->get_container_impl().container_element_impl();

	element my_element=get();

	// For each one of the four corners, compute:
	//
	// - The corner's coordinates.
	//
	// - border_impl::base::cornerXX for this corner.
	//
	// - Which field in surrounding_elements_info our element should be
	// installed into, to simulate this corner's environment; i.e., for
	// a top-left corner the element is the bottomright element.

	struct {
		const const_border_impl &which_border;
		rectangle r;
		decltype(border_impl::base::cornertl()) which_corner;
		element_implptr surrounding_elements_info::*this_corner;
	} four_corners[4]=
		  {
		   {
		    info.tl,
		    {0, 0, info.left_pad, info.top_pad},
		    // Specifies which corner border_implObj will draw.
		    border_impl::base::cornerbr(),

		    // Specifies where our element is positioned, in the
		    // surrounding_elements_info parameter.
		    //
		    // draw_corner() looks at which corner it is asked to
		    // draw, then sees if in the simulated grid, there's
		    // an element in the given corner. We'll indicate that
		    // our singleton element is in the same corner.

		    &surrounding_elements_info::bottomright,
		   },
		   {
		    info.tr,
		    {0, 0, info.right_pad, info.top_pad},
		    border_impl::base::cornerbl(),
		    &surrounding_elements_info::bottomleft,
		   },
		   {
		    info.bl,
		    {0, 0, info.left_pad, info.bottom_pad},
		    border_impl::base::cornertr(),
		    &surrounding_elements_info::topright,
		   },
		   {
		    info.br,
		    {0, 0, info.right_pad, info.bottom_pad},
		    border_impl::base::cornertl(),
		    &surrounding_elements_info::topleft,
		   }
		  };

	// The starting X coordinate for the two corners on the right.
	four_corners[1].r.x=four_corners[3].r.x=
		coord_t::truncate(e.data(IN_THREAD).current_position.width-
				  info.right_pad);

	// The starting Y coordinate for the two corners at the bottom.

	four_corners[2].r.y=four_corners[3].r.y=
		coord_t::truncate(e.data(IN_THREAD).current_position.height-
				  info.bottom_pad);

	for (const auto &corner_info:four_corners)
	{
		const auto &r=corner_info.r;

		drawn_areas.push_back(r); // Inform the caller we drew this.

		// Need to tell border_implObj which element allegedly
		// exists at this corner.
		surrounding_elements_info at_this_corner;

		at_this_corner.*(corner_info.this_corner)=my_element->impl;

		// We now acquire the scratch buffers.

		e.draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const picture &p,
			  const pixmap &pm,
			  const gc &c)
			 {
				 bordercontainer_impl->corner_mask_buffer->get
					 (r.width,
					  r.height,
					  [&, this]
					  (const picture &scratch_p,
					   const pixmap &scratch_pm,
					   const gc &scratch_gc)
					  {
						  // Prepare the border draw
						  // info object, that tells
						  // corner_draw() what to do.

						  border_implObj::draw_info bdi
							  {
							   p,
							   {0, 0, r.width,
							    r.height},
							   pm,
							   scratch_p,
							   scratch_pm,
							   scratch_gc,
							   coord_t::truncate
							   (di.absolute_location
							    .x + r.x),
							   coord_t::truncate
							   (di.absolute_location
							    .y + r.y)};

						  if (corner_info.which_border
						      ->no_corner_border(bdi))
							  return;

						  corner_info.which_border
							  ->draw_corner
							  (IN_THREAD,
							   bdi,
							   corner_info
							   .which_corner,
							   at_this_corner);
					  });
			 },
			 r,
			 di, di, clip,
			 bordercontainer_impl->corner_scratch_buffer);
	}

	// Now, draw the lines, first the horizontal border lines,
	// above and below.
	//
	// For each call to background_horizontal() and draw_horizontal() we
	// specify:
	//
	// - the starting Y coordinate.
	//
	// - either the element above or below the border.

	struct {
		coord_t starting_coord;
		dim_t thickness;
		const_border_impl border;
		elementptr first_e;
		elementptr second_e;
	} lines[2]={
		    {0, info.top_pad, info.tb, {}, my_element},
		    {0, info.bottom_pad, info.bb, my_element, {}}
	};

	// Compute the Y coordinate for the bottom border.

	lines[1].starting_coord=
		coord_t::truncate(coord_t::truncate(e.data(IN_THREAD)
						    .current_position.height)
				  -info.bottom_pad);

	// Compute the length of the horizontal border.
	dim_t length=dim_t::truncate(coord_t::truncate(e.data(IN_THREAD)
						       .current_position.width)
				     - (info.left_pad+info.right_pad));

	for (const auto &line_info:lines)
	{
		// Calculate where the drawn border sohuld go.
		rectangle r{coord_t::truncate(info.left_pad),
			    line_info.starting_coord,
			    length,
			    line_info.thickness};
		drawn_areas.push_back(r); // We are drawing this rectangle.

		// Acquire the scratch buffers, and create the draw_info
		// that tells border_implObj what to do.
		//
		// We will call background_horizontal() to draw any part of the
		// border which uses the nearby element's background color,
		// then draw_horizontal() to draw the horizontal border.

		e.draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const picture &p,
			  const pixmap &pm,
			  const gc &c)
			 {
				 bordercontainer_impl->h_mask_buffer->get
					 (r.width,
					  r.height,
					  [&, this]
					  (const picture &scratch_p,
					   const pixmap &scratch_pm,
					   const gc &scratch_gc)
					  {
						  border_implObj::draw_info bdi
							  {
							   p,
							   {0, 0, r.width,
							    r.height},
							   pm,
							   scratch_p,
							   scratch_pm,
							   scratch_gc,
							   coord_t::truncate
							   (di.absolute_location
							    .x + r.x),
							   coord_t::truncate
							   (di.absolute_location
							    .y + r.y)};

						  if (line_info.border
						      ->no_horizontal_border(bdi)
						      )
							  return;

						  bdi.background_horizontal
							  (IN_THREAD,
							   line_info.first_e,
							   line_info.second_e);

						  line_info.border
							  ->draw_horizontal
							  (IN_THREAD, bdi);

					  });
			 },
			 r,
			 di, di, clip,
			 bordercontainer_impl->h_scratch_buffer);
	}

	// And now for the two vertical borders, which are drawn similary.
	//
	// Compute the starting X coordinate of the right border, and
	// the height of both vertical borders.

	lines[0].thickness=info.left_pad;
	lines[1].thickness=info.right_pad;
	lines[0].border=info.lb;
	lines[1].border=info.rb;

	lines[1].starting_coord=
		coord_t::truncate(coord_t::truncate(e.data(IN_THREAD)
						    .current_position.width)
				  -info.right_pad);

	length=dim_t::truncate(coord_t::truncate(e.data(IN_THREAD)
						 .current_position.height)
			       - (info.top_pad+info.bottom_pad));

	// And now do similar things for vertical borders; using
	// background_vertical() and draw_vertical().

	for (const auto &line_info:lines)
	{
		rectangle r{line_info.starting_coord,
			    coord_t::truncate(info.top_pad),
			    line_info.thickness,
			    length};

		drawn_areas.push_back(r);

		e.draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const picture &p,
			  const pixmap &pm,
			  const gc &c)
			 {
				 bordercontainer_impl->h_mask_buffer
					 ->get
					 (r.width,
					  r.height,
					  [&, this]
					  (const picture &scratch_p,
					   const pixmap &scratch_pm,
					   const gc &scratch_gc)
					  {
						  border_implObj::draw_info bdi
							  {
							   p,
							   {0, 0, r.width,
							    r.height},
							   pm,
							   scratch_p,
							   scratch_pm,
							   scratch_gc,
							   coord_t::truncate
							   (di.absolute_location
							    .x + r.x),
							   coord_t::truncate
							   (di.absolute_location
							    .y + r.y)};

						  if (line_info.border
						      ->no_vertical_border(bdi)
						      )
							  return;

						  bdi.background_vertical
							  (IN_THREAD,
							   line_info.first_e,
							   line_info.second_e);

						  line_info.border->draw_vertical
							  (IN_THREAD, bdi);

					  });
			 },
			 r,
			 di, di, clip,
			 bordercontainer_impl->h_scratch_buffer);
	}
}

LIBCXXW_NAMESPACE_END
