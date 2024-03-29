/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/scratch_buffer.H"
#include "x/w/impl/container.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/border_impl.H"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtext_alteration_config.H"
#include "richtext/richtext_draw_info.H"
#include "richtext/richtext_draw_boundaries.H"
#include "corner_borderfwd.H"

LIBCXXW_NAMESPACE_START

borderlayoutmanagerObj::implObj::implObj(const container_impl &container_impl,
					 const ref<bordercontainer_implObj>
					 &bordercontainer_impl,
					 const std::optional<color_arg>
					 &frame_background,
					 const element &initial_element,
					 halign element_halign,
					 valign element_valign)
	: singletonlayoutmanagerObj::implObj{container_impl, initial_element,
					     element_halign,
					     element_valign},
	  bordercontainer_impl{bordercontainer_impl},
	  frame_background{frame_background}
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

	auto title=bordercontainer_impl->get_title(IN_THREAD);

	// If there's a title, make sure the top border is tall enough for it.

	if (title)
	{
		auto [wm, hm]=title->get_metrics(0);

		dim_t text_height=hm.preferred();

		if (text_height > top_pad)
			top_pad=text_height;
	}

	// HOWEVER: if the given side's border is all wet, there is no padding.

	if (lb->no_border())
		left_pad=0;

	if (rb->no_border())
		right_pad=0;

	if (tb->no_border() && !title)
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
	bordercontainer_impl->get_container_impl()
		.container_element_impl()
		.schedule_full_redraw(IN_THREAD);
}

void borderlayoutmanagerObj::implObj::theme_updated(ONLY IN_THREAD,
						    const const_defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);

	needs_recalculation(IN_THREAD);
}

void borderlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	auto title=bordercontainer_impl->get_title(IN_THREAD);

	auto previous_title=current_title;

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
	    ->border(IN_THREAD) ||
	    current_title != title)
	{
		cached_border_info.reset();

		const auto &info=get_all_borders(IN_THREAD);

		current_left_border=info.lb;
		current_right_border=info.rb;
		current_top_border=info.tb;
		current_bottom_border=info.bb;
		current_title=title;

		bordercontainer_impl->get_container_impl()
			.container_element_impl()
			.schedule_full_redraw(IN_THREAD);
	}

	if ((previous_title ? 1:0)
	    ^ (current_title ? 1:0))
	{
		update_element_border(IN_THREAD, get_list_element(IN_THREAD));
	}

	superclass_t::recalculate(IN_THREAD);
}

void borderlayoutmanagerObj::implObj::created(ONLY IN_THREAD, const element &e)
{
	superclass_t::created(IN_THREAD, e);
	update_element_border(IN_THREAD, e);
}

void borderlayoutmanagerObj::implObj::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	update_element_border(IN_THREAD, get_list_element(IN_THREAD));
}

void borderlayoutmanagerObj::implObj::update_element_border(ONLY IN_THREAD,
							    const element &e)
{
	if (!frame_background)
		return;

	if (!current_title)
		e->set_background_color(IN_THREAD, *frame_background);
	else
		e->remove_background_color(IN_THREAD);
}

void borderlayoutmanagerObj::implObj::do_draw(ONLY IN_THREAD,
					      const draw_info &di,
					      clip_region_set &clip,
					      rectarea &drawn_areas)
{
	superclass_t::do_draw(IN_THREAD, di, clip, drawn_areas);

	drawn_areas.reserve(drawn_areas.size()+9);

	const auto &info=get_all_borders(IN_THREAD);

	auto &e=bordercontainer_impl->get_container_impl().container_element_impl();
	auto &current_position=e.data(IN_THREAD).current_position;

	if (current_position.width <= dim_t::truncate(info.left_pad
						      + info.right_pad) ||
	    current_position.height <= dim_t::truncate(info.top_pad
						       + info.bottom_pad))
		return; // To small to draw anything.

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
		coord_t::truncate(current_position.width-
				  info.right_pad);

	// The starting Y coordinate for the two corners at the bottom.

	four_corners[2].r.y=four_corners[3].r.y=
		coord_t::truncate(current_position.height-
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
	// Compute the length of the horizontal border.

	dim_t length=dim_t::truncate(coord_t::truncate(current_position.width)
				     - (info.left_pad+info.right_pad));

	struct {
		coord_t starting_coord;
		dim_t thickness;
		coord_t x;
		dim_t width;
		const_border_impl border;
		elementptr first_e;
		elementptr second_e;
	} hlines[3]={
		     {0, info.top_pad,
		      coord_t::truncate(info.left_pad),
		      length, info.tb, {}, my_element},
		     {0, info.bottom_pad,
		      hlines[0].x, length, info.bb, my_element, {}},
		     hlines[0] // Filler.
	};

	// Compute the Y coordinate for the bottom border.

	hlines[1].starting_coord=
		coord_t::truncate(coord_t::truncate(current_position.height)
				  -info.bottom_pad);

	size_t n=2;

	dim_t text_width;
	dim_t text_height;

	if (current_title)
	{
		auto [w, h]=current_title->get_metrics(0);

		auto text_width=w.preferred();
		auto text_height=h.preferred();

		auto title_indent=
			bordercontainer_impl->get_title_indent(IN_THREAD);

		if (title_indent > length)
			title_indent=length;

		if (text_width > length-title_indent)
		{
			text_width=length-title_indent;
		}

		if (text_width > 0)
		{
			dim_t width_and_indent
				{dim_t::truncate(text_width+title_indent)};

			// hlines[0] is the top border.
			//
			// Adjust the top border to not draw where the title
			// goes. But before we do that, copy it, and have the
			// copy draw the border in the title indent area.

			if (title_indent > 0)
			{
				hlines[2]=hlines[0];
				hlines[2].width=title_indent;
				++n;
			}

			hlines[0].x=coord_t::truncate(hlines[0].x+
						      width_and_indent);
			hlines[0].width -= width_and_indent;


			rectangle r{coord_t::truncate(info.left_pad+
						      title_indent), 0,
					text_width,
					info.top_pad};

			if (r.height > text_height)
			{
				r.y=coord_t::truncate((r.height-text_height)/2);
				r.height=text_height;
			}

			drawn_areas.push_back(r); // We are drawing this

			richtext_draw_boundaries bounds{di, di.entire_area()};

			bounds.position_at(r);

			richtext_alteration_config richtext_alteration;
			richtext_draw_info rdi{richtext_alteration};

			current_title->full_redraw(IN_THREAD, e, rdi, di,
						   clip,
						   bounds);
		}
	}

	for (size_t i=0; i<n; ++i)
	{
		const auto &line_info=hlines[i];

		// Calculate where the drawn border should go.
		rectangle r{line_info.x,
			    line_info.starting_coord,
			    line_info.width,
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

						  bdi.background_horizontal
							  (IN_THREAD,
							   line_info.first_e,
							   line_info.second_e);

						  if (line_info.border
						      ->no_horizontal_border(bdi)
						      )
							  return;

						  line_info.border
							  ->draw_horizontal
							  (IN_THREAD, bdi);

					  });
			 },
			 r,
			 di, di, clip,
			 bordercontainer_impl->h_scratch_buffer);
	}

	// And now for the two vertical borders, which are drawn similarly.
	//
	// Compute the starting X coordinate of the right border, and
	// the height of both vertical borders.


	struct {
		coord_t starting_coord;
		dim_t thickness;
		const_border_impl border;
		elementptr first_e;
		elementptr second_e;
	} vlines[2]={
		    {0, info.left_pad, info.lb, {}, my_element},
		    {0, info.right_pad, info.rb, my_element, {}},
	};

	vlines[1].starting_coord=
		coord_t::truncate(coord_t::truncate(current_position.width)
				  -info.right_pad);

	length=dim_t::truncate(coord_t::truncate(current_position.height)
			       - (info.top_pad+info.bottom_pad));

	// And now do similar things for vertical borders; using
	// background_vertical() and draw_vertical().

	for (const auto &line_info: vlines)
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
				 bordercontainer_impl->v_mask_buffer
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

void borderlayoutmanagerObj::implObj
::adjust_child_horiz_vert_metrics(ONLY IN_THREAD,
				  metrics::axis &child_horiz,
				  metrics::axis &child_vert)
{
	if (!current_title)
		return;

	auto [wm, hm]=current_title->get_metrics(0);

	dim_t title_indent=bordercontainer_impl
		->get_title_indent(IN_THREAD);

	dim_t double_title_indent=dim_t::truncate(title_indent + title_indent);
	dim_t minimum=dim_t::truncate(wm.preferred()+double_title_indent);

	if (minimum == dim_t::infinite())
		--minimum;

	if (minimum > child_horiz.minimum())
	{
		child_horiz=child_horiz
			.increase_minimum_by(minimum - child_horiz.minimum());
	}
}


LIBCXXW_NAMESPACE_END
