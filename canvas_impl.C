/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/canvas.H"
#include "x/w/impl/themedim_axis_element.H"
#include "connection_thread.H"
#include "defaulttheme.H"
#include "xid_t.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

static metrics::axis
initial_axis(const defaulttheme &theme,
	     themedimaxis wh,
	     const dim_axis_arg &arg)
{
	auto min=theme->get_theme_dim_t(arg.minimum, wh);
	auto pref=theme->get_theme_dim_t(arg.preferred, wh);
	auto max=theme->get_theme_dim_t(arg.maximum, wh);

	if (pref < min)
		pref=min;

	if (max < pref)
		max=pref;

	return {min, pref, max};
}

child_element_init_params
canvasObj::implObj
::create_child_element_params(const container_impl &container,
			      const canvas_init_params &params)
{
	auto theme=container->container_element_impl().get_screen()
		->impl->current_theme.get();

	return {
		params.scratch_buffer_id.empty() ?
			std::string{"background@libcxx.com"} :
		params.scratch_buffer_id,
		{
			initial_axis(theme,
				     themedimaxis::width,
				     params.width),
			initial_axis(theme,
				     themedimaxis::height,
				     params.height),
		},
		params.background_color};
}

canvasObj::implObj::implObj(const container_impl &container,
			    const canvas_init_params &init_params)
	: implObj{container, init_params,
		create_child_element_params(container, init_params)}
{
}

canvasObj::implObj::implObj(const container_impl &container,
			    const canvas_init_params &init_params,
			    const child_element_init_params
			    &child_element_params)

	: superclass_t{init_params.width, init_params.height,
		container, child_element_params}
{
}

canvasObj::implObj::~implObj()=default;

void canvasObj::implObj::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	recalculate(IN_THREAD);
}

void canvasObj::implObj::theme_updated(ONLY IN_THREAD,
				       const defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	recalculate(IN_THREAD);
}

void canvasObj::implObj::recalculate(ONLY IN_THREAD)
{
	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 get_width_axis(IN_THREAD),
		 get_height_axis(IN_THREAD));
}


LIBCXXW_NAMESPACE_END
