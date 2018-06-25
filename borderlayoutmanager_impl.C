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
					 const elementptr &initial_element)
	: singletonlayoutmanagerObj::implObj{container_impl, initial_element},
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
	return dim_t::truncate
		(bordercontainer_impl->get_border_hpad(IN_THREAD) +
		 bordercontainer_impl->get_border(IN_THREAD)->border(IN_THREAD)
		 ->calculated_border_width);
}

dim_t borderlayoutmanagerObj::implObj::get_right_padding(ONLY IN_THREAD)
{
	return get_left_padding(IN_THREAD);
}

dim_t borderlayoutmanagerObj::implObj::get_top_padding(ONLY IN_THREAD)
{
	return dim_t::truncate
		(bordercontainer_impl->get_border_vpad(IN_THREAD) +
		 bordercontainer_impl->get_border(IN_THREAD)->border(IN_THREAD)
		 ->calculated_border_height);
}

dim_t borderlayoutmanagerObj::implObj::get_bottom_padding(ONLY IN_THREAD)
{
	return get_top_padding(IN_THREAD);
}

rectangle borderlayoutmanagerObj::implObj
::padded_position(ONLY IN_THREAD, const element_impl &e)
{
	auto b=bordercontainer_impl->get_border(IN_THREAD)->border(IN_THREAD);

	dim_t hpad=b->calculated_border_width;
	dim_t vpad=b->calculated_border_height;

	dim_t hpad2=dim_t::truncate(hpad+hpad);
	dim_t vpad2=dim_t::truncate(vpad+vpad);

	auto &current_position=bordercontainer_impl->get_container_impl()
		.container_element_impl().data(IN_THREAD).current_position;

	dim_t padded_width=
		current_position.width > hpad2 ? current_position.width-hpad2:dim_t{};
	dim_t padded_height=
		current_position.height > vpad2 ? current_position.height-vpad2:dim_t{};

	return {coord_t::truncate(hpad), coord_t::truncate(vpad),
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

void borderlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	superclass_t::recalculate(IN_THREAD);

	// If the border changed, we need to redraw even if recalculate()
	// did nothing.
	if (current_border != bordercontainer_impl->get_border(IN_THREAD)
	    ->border(IN_THREAD))
		bordercontainer_impl->get_container_impl()
			.container_element_impl().schedule_redraw(IN_THREAD);
}

void borderlayoutmanagerObj::implObj::do_draw(ONLY IN_THREAD,
					      const draw_info &di,
					      clip_region_set &clip,
					      rectangle_set &drawn_areas)
{
	superclass_t::do_draw(IN_THREAD, di, clip, drawn_areas);

	auto b=bordercontainer_impl->get_border(IN_THREAD)->border(IN_THREAD);
	current_border=b;

	auto &e=bordercontainer_impl->get_container_impl()
		.container_element_impl();

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
		rectangle r;
		decltype(border_impl::base::cornertl()) which_corner;
		element_implptr surrounding_elements_info::*this_corner;
	} four_corners[4]=
		  {
		   {
		    //
		    // All four corners have calculated_border_width/height,
		    // and the coordinates vary only in the starting X and Y
		    // positions, which we will set below.
		    {0, 0, b->calculated_border_width,
		     b->calculated_border_height},

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
		    {0, 0, b->calculated_border_width,
		     b->calculated_border_height},
		    border_impl::base::cornerbl(),
		    &surrounding_elements_info::bottomleft,
		   },
		   {
		    {0, 0, b->calculated_border_width,
		     b->calculated_border_height},
		    border_impl::base::cornertr(),
		    &surrounding_elements_info::topright,
		   },
		   {
		    {0, 0, b->calculated_border_width,
		     b->calculated_border_height},
		    border_impl::base::cornertl(),
		    &surrounding_elements_info::topleft,
		   }
		  };

	// The starting X coordinate for the two corners on the right.
	four_corners[1].r.x=four_corners[3].r.x=
		coord_t::truncate(e.data(IN_THREAD).current_position.width-
				  b->calculated_border_width);

	// The starting Y coordinate for the two corners at the bottom.

	four_corners[2].r.y=four_corners[3].r.y=
		coord_t::truncate(e.data(IN_THREAD).current_position.height-
				  b->calculated_border_height);

	for (const auto &corner_info:four_corners)
	{
		const auto &r=corner_info.r;

		drawn_areas.insert(r); // Inform the caller we drew this.

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
				 bordercontainer_impl->corner_mask_buffer
					 ->get
					 (b->calculated_border_width,
					  b->calculated_border_height,
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

						  b->draw_corner
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
		elementptr first_e;
		elementptr second_e;
	} lines[2]={
		    {0, {}, my_element},
		    {0, my_element, {}}
	};

	// Compute the Y coordinate for the bottom border.

	lines[1].starting_coord=
		coord_t::truncate(coord_t::truncate(e.data(IN_THREAD)
						    .current_position.height)
				  -b->calculated_border_height);

	// Compute the length of the horizontal border.
	dim_t length=dim_t::truncate(coord_t::truncate(e.data(IN_THREAD)
						       .current_position.width)
				     - (b->calculated_border_width
					+b->calculated_border_width));

	for (const auto &line_info:lines)
	{
		// Calculate where the drawn border sohuld go.
		rectangle r{coord_t::truncate(b->calculated_border_width),
			    line_info.starting_coord,
			    length,
			    b->calculated_border_height};
		drawn_areas.insert(r); // We are drawing this rectangle.

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
				 bordercontainer_impl->h_mask_buffer
					 ->get
					 (r.width,
					  b->calculated_border_height,
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

						  bdi.background_horizontal
							  (IN_THREAD,
							   line_info.first_e,
							   line_info.second_e);

						  b->draw_horizontal(IN_THREAD,
								     bdi);

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
	lines[1].starting_coord=
		coord_t::truncate(coord_t::truncate(e.data(IN_THREAD)
						    .current_position.width)
				  -b->calculated_border_width);

	length=dim_t::truncate(coord_t::truncate(e.data(IN_THREAD)
						 .current_position.height)
			       - (b->calculated_border_height
				  +b->calculated_border_height));

	// And now do similar things for vertical borders; using
	// background_vertical() and draw_vertical().

	for (const auto &line_info:lines)
	{
		rectangle r{line_info.starting_coord,
			    coord_t::truncate(b->calculated_border_height),
			    b->calculated_border_width,
			    length};

		drawn_areas.insert(r);

		e.draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const picture &p,
			  const pixmap &pm,
			  const gc &c)
			 {
				 bordercontainer_impl->h_mask_buffer
					 ->get
					 (b->calculated_border_height,
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

						  bdi.background_vertical
							  (IN_THREAD,
							   line_info.first_e,
							   line_info.second_e);

						  b->draw_vertical(IN_THREAD,
								   bdi);

					  });
			 },
			 r,
			 di, di, clip,
			 bordercontainer_impl->h_scratch_buffer);
	}
}

LIBCXXW_NAMESPACE_END
