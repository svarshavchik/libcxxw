/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_cellimage.H"
#include "richtext/richtext_draw_boundaries.H"
#include "icon.H"
#include "icon_images_vector.H"
#include "pixmap_with_picture.H"
#include "x/w/pixmap.H"
#include "x/w/picture.H"

LIBCXXW_NAMESPACE_START

list_cellimageObj::list_cellimageObj(const std::vector<icon> &images,
					     halign alignment)
	: icon_images_vector(images), alignment(alignment)
{
}

std::pair<metrics::axis, metrics::axis>
list_cellimageObj::cell_get_metrics(ONLY IN_THREAD, dim_t preferred_width)
{
	// Derive the metrics from the largest icon we have.

	dim_t w, h;

	for (const auto &image:icon_images(IN_THREAD))
	{
		auto pm=image->image;

		auto iw=pm->get_width();
		auto ih=pm->get_height();

		if (iw > w)
			w=iw;

		if (ih > h)
			h=ih;
	}

	return { {w, w, w}, {h, h, h} };
}

void list_cellimageObj::cell_redraw(ONLY IN_THREAD,
				    element_drawObj &draw,
				    const draw_info &di,
				    clip_region_set &clipped,
				    bool draw_as_disabled,
				    richtext_draw_boundaries &boundaries)
{
	clipped.draw_as_disabled=draw_as_disabled;

	draw.draw_using_scratch_buffer
		(IN_THREAD,
		 [&, this]
		 (const picture &area_picture,
		  const pixmap &area_pixmap,
		  const gc &area_gc)
		 {
			 auto image=this->icon_images(IN_THREAD)
				 .at(*mpobj<size_t>::lock{this->n});

			 auto pm=image->image;

			 auto w=pm->get_width();
			 auto h=pm->get_height();
			 dim_t x=0;
			 dim_t y=0;

			 if (boundaries.position.width > w)
				 switch (alignment) {
				 case halign::right:
					 x=boundaries.position.width-w;
					 break;
				 case halign::center:
					 x=(boundaries.position.width-w)/2;
					 break;
				 default:
					 break;
				 }

			 if (boundaries.position.height > h)
				 y=(boundaries.position.height-h)/2;

			 area_picture->composite(image->image->
						 icon_picture,
						 0, 0,
						 {coord_t::truncate(x),
						  coord_t::truncate(y),
						  w, h},
						 render_pict_op::op_over
						 );
		 },
		 boundaries.position,
		 di,
		 di,
		 clipped);
}

void list_cellimageObj::cell_initialize(ONLY IN_THREAD,
					    const defaulttheme &initial_theme)
{
	initialize(IN_THREAD);
}

void list_cellimageObj::cell_theme_updated(ONLY IN_THREAD,
					       const defaulttheme &new_theme)
{
	theme_updated(IN_THREAD, new_theme);
}

bool list_cellimageObj::cell_is_separator()
{
	return false;
}

LIBCXXW_NAMESPACE_END
