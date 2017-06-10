/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image.H"
#include "icon.H"
#include "icon_image.H"

#include "x/w/pixmap.H"
#include "x/w/picture.H"

LIBCXXW_NAMESPACE_START

imageObj::implObj::implObj(const ref<containerObj::implObj> &container,
		 const icon &initial_icon)
	: implObj(container, initial_icon,
		  initial_icon->image->icon_pixmap->get_width(),
		  initial_icon->image->icon_pixmap->get_height(),
		  "image@libcxx")
{
}

imageObj::implObj::implObj(const ref<containerObj::implObj> &container,
		 const icon &initial_icon,
		 dim_t icon_width,
		 dim_t icon_height,
		 const std::string &scratch_bufer_id)
	: child_elementObj(container,
			   {"image@libcxx",
				   { {icon_width, icon_width, icon_width},
					   {icon_height, icon_height,
							   icon_height}}}),
	  current_icon(initial_icon)
{
}

void imageObj::implObj::initialize(IN_THREAD_ONLY)
{
	// Theme might've been changed. Resynchronize.

	current_icon=current_icon->initialize(IN_THREAD);
}

void imageObj::implObj::do_draw(IN_THREAD_ONLY,
				const draw_info &di,
				const rectangle_set &areas)
{
	// We ignore areas for now, and just composite the entire icon picture.

	auto w=current_icon->image->icon_pixmap->get_width();
	auto h=current_icon->image->icon_pixmap->get_height();

	clip_region_set clipped{IN_THREAD, di};

	draw_using_scratch_buffer(IN_THREAD,
				  [&, this]
				  (const auto &pic,
				   const auto &,
				   const auto &)
				  {
					  pic->composite(current_icon->image
							 ->icon_picture,
							 0, 0,
							 0, 0,
							 w, h,
							 render_pict_op
							 ::op_atop);
				  },
				  {0, 0, w, h},
				  di, di, clipped);
}

void imageObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	auto new_icon=current_icon->theme_updated(IN_THREAD);

	if (new_icon == current_icon)
		return;

	set_icon(IN_THREAD, new_icon);
}

void imageObj::implObj::set_icon(IN_THREAD_ONLY, const icon &new_icon)
{
	current_icon=new_icon->initialize(IN_THREAD);

	// Update the metrics to reflect the new icon.

	auto w=current_icon->image->icon_pixmap->get_width();
	auto h=current_icon->image->icon_pixmap->get_height();

	get_horizvert(IN_THREAD)->set_element_metrics(IN_THREAD,
						      {w, w, w}, {h, h, h});
	schedule_redraw(IN_THREAD);
}


LIBCXXW_NAMESPACE_END
