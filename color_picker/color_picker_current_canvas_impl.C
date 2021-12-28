/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_current_canvas_impl.H"
#include "screen_positions_impl.H"
#include "generic_window_handler.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

color_picker_current_canvasObj::implObj
::implObj(const container_impl &container,
	  const std::string name,
	  const color_pickerObj::implObj::official_color &initial_color,
	  const canvas_init_params &params)
	: canvasObj::implObj{container, params},
	  current_color_thread_only{initial_color->official_color.get()},
	  name{name},
	  current_official_color{initial_color}
{
}

color_picker_current_canvasObj::implObj::~implObj()=default;

void color_picker_current_canvasObj::implObj
::cleared_to_background_color(ONLY IN_THREAD,
			      const picture &pic,
			      const pixmap &pix,
			      const gc &context,
			      const draw_info &di,
			      const rectangle &r)
{
	auto c=current_color(IN_THREAD);

	// Pre-multiply the RGB components, before composing it using
	// fill_rectangle.
	for (int i=0; i<3; ++i)
	{
		rgb_component_squared_t v=
			static_cast<rgb_component_squared_t>(c.*(rgb_fields[i]))
			* c.a;

		rgb_component_t sv=v / rgb::maximum;

		if ( v % rgb::maximum >= rgb::maximum/2)
		{
			++sv;
			if (sv == 0)
				--sv;
		}
		c.*(rgb_fields[i]) = sv;
	}

	pic->fill_rectangle({0, 0, r.width, r.height}, c,
			    render_pict_op::op_atop);
}

void color_picker_current_canvasObj::implObj
::save(ONLY IN_THREAD,
       const screen_positions &pos)
{
	if (name.empty())
		return;

	std::vector<std::string> hierarchy;

	get_window_handler().window_id_hierarchy(hierarchy);

	auto writelock=pos->impl->create_writelock_for_saving(
		hierarchy, libcxx_uri, "color", name);

	auto color=current_official_color->official_color.get();

	for (size_t i=0; i<4; ++i)
	{
		writelock->create_child()
			->element({rgb_channels[i]})
			->text(color.*(rgb_fields[i]))
			->parent()->parent();
	}
}

LIBCXXW_NAMESPACE_END
