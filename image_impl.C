/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image.H"
#include "icon.H"

#include "x/w/pixmap.H"
#include "x/w/picture.H"
#include "pixmap_with_picture.H"

LIBCXXW_NAMESPACE_START

imageObj::implObj::implObj(const ref<containerObj::implObj> &container,
			   const icon &initial_icon)
	: implObj(container, initial_icon,
		  initial_icon->image->get_width(),
		  initial_icon->image->get_height())
{
}

imageObj::implObj::implObj(const ref<containerObj::implObj> &container,
			   const icon &initial_icon,
			   dim_t icon_width,
			   dim_t icon_height)
	: implObj(container, initial_icon,
		  {icon_width, icon_width, icon_width},
		  {icon_height, icon_height, icon_height})
{
}

imageObj::implObj::implObj(const ref<containerObj::implObj> &container,
			   const icon &initial_icon,
			   const metrics::axis &horiz_metrics,
			   const metrics::axis &vert_metrics)
	: child_elementObj{container, {"image@libcxx.com",
		{ horiz_metrics, vert_metrics }}},
	  current_icon_thread_only(initial_icon)
{
}

void imageObj::implObj::initialize(ONLY IN_THREAD)
{
	auto new_icon=current_icon(IN_THREAD)->initialize(IN_THREAD);

	if (new_icon == current_icon(IN_THREAD))
		return;

	set_icon(IN_THREAD, new_icon);
}

void imageObj::implObj::do_draw(ONLY IN_THREAD,
				const draw_info &di,
				const rectangle_set &areas)
{
	// We ignore areas for now, and just composite the entire icon picture.

	auto w=current_icon(IN_THREAD)->image->get_width();
	auto h=current_icon(IN_THREAD)->image->get_height();

	clip_region_set clipped{IN_THREAD, get_window_handler(), di};

	draw_using_scratch_buffer(IN_THREAD,
				  [&, this]
				  (const auto &pic,
				   const auto &,
				   const auto &)
				  {
					  pic->composite(this->current_icon
							 (IN_THREAD)->image
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

void imageObj::implObj::theme_updated(ONLY IN_THREAD,
				      const defaulttheme &new_theme)
{
	auto new_icon=current_icon(IN_THREAD)->theme_updated(IN_THREAD, new_theme);

	if (new_icon == current_icon(IN_THREAD))
		return;

	set_icon(IN_THREAD, new_icon);
}

void imageObj::implObj::set_icon(ONLY IN_THREAD, const icon &new_icon)
{
	current_icon(IN_THREAD)=new_icon->initialize(IN_THREAD);

	update_image_metrics(IN_THREAD);
	schedule_redraw(IN_THREAD);
}

void imageObj::implObj::update_image_metrics(ONLY IN_THREAD)
{
	auto w=current_icon(IN_THREAD)->image->get_width();
	auto h=current_icon(IN_THREAD)->image->get_height();

	get_horizvert(IN_THREAD)->set_element_metrics(IN_THREAD,
						      {w, w, w}, {h, h, h});
}

LIBCXXW_NAMESPACE_END
