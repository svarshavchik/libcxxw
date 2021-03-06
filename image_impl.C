/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image.H"
#include "x/w/impl/icon.H"

#include "x/w/pixmap.H"
#include "x/w/picture.H"
#include "x/w/rectangle.H"
#include "x/w/impl/container.H"
#include "x/w/impl/metrics_horizvert.H"
#include "x/w/impl/pixmap_with_picture.H"

LIBCXXW_NAMESPACE_START

image_impl_init_params::image_impl_init_params(const container_impl &container,
					       const icon &initial_icon)
	: image_impl_init_params{container, initial_icon,
				 initial_icon->image->get_width(),
				 initial_icon->image->get_height()}
{
}

image_impl_init_params::image_impl_init_params(const container_impl &container,
					       const icon &initial_icon,
					       dim_t icon_width,
					       dim_t icon_height)
	: image_impl_init_params{container, initial_icon,
				 {icon_width, icon_width, icon_width},
				 {icon_height, icon_height, icon_height}}
{
}

image_impl_init_params
::image_impl_init_params(const container_impl &container,
			 const icon &initial_icon,
			 const metrics::axis &horiz_metrics,
			 const metrics::axis &vert_metrics)
	: child_element_init_params{"image@libcxx.com",
				    { horiz_metrics, vert_metrics }},
	  parent_container{container},
	  initial_icon{initial_icon}
{
}

image_impl_init_params::~image_impl_init_params()=default;

imageObj::implObj::implObj(const image_impl_init_params &init_params)
	: child_elementObj{init_params.parent_container, init_params},
	  current_icon_thread_only{init_params.initial_icon}
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
				const rectarea &areas)
{
	// We ignore areas for now, and just composite the entire icon picture.

	auto w=current_icon(IN_THREAD)->image->get_width();
	auto h=current_icon(IN_THREAD)->image->get_height();

	// If our layout manager gave us more room, center ourselves.

	auto aligned=metrics::align(di.absolute_location.width,
				    di.absolute_location.height,
				    w,
				    h,
				    halign::center,
				    valign::middle);

	// And clear the rest to our background color.

	auto extra_space=subtract({ {0, 0, di.absolute_location.width,
				     di.absolute_location.height}}, {aligned});

	if (!extra_space.empty())
		clear_to_color(IN_THREAD, di, extra_space);

	clip_region_set clipped{IN_THREAD, get_window_handler(), di};

	draw_using_scratch_buffer
		(IN_THREAD,
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
					aligned.width, aligned.height,
					render_pict_op::op_atop);
		 },
		 aligned,
		 di, di, clipped);
}

void imageObj::implObj::theme_updated(ONLY IN_THREAD,
				      const const_defaulttheme &new_theme)
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
	schedule_full_redraw(IN_THREAD);
}

void imageObj::implObj::update_image_metrics(ONLY IN_THREAD)
{
	auto w=current_icon(IN_THREAD)->image->get_width();
	auto h=current_icon(IN_THREAD)->image->get_height();

	get_horizvert(IN_THREAD)->set_element_metrics(IN_THREAD,
						      {w, w, w}, {h, h, h});
}

LIBCXXW_NAMESPACE_END
