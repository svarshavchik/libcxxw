/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container_element.H"
#include "editor_container_impl.H"
#include "peephole_get_element.H"
#include "editor.H"
#include "editor_impl.H"
#include "nonrecursive_visibility.H"
#include "peephole_layoutmanager_impl.H"
#include "fonts/current_fontcollection.H"
#include "fonts/fontcollection.H"

LIBCXXW_NAMESPACE_START

editor_containerObj::implObj::~implObj()=default;

void editor_containerObj::implObj::initialize(IN_THREAD_ONLY)
{
	peepholeObj::implObj::initialize(IN_THREAD);
	recalculate(IN_THREAD);
}

void editor_containerObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	peepholeObj::implObj::theme_updated(IN_THREAD);
	recalculate(IN_THREAD);
}

void editor_containerObj::implObj::recalculate(IN_THREAD_ONLY)
{
	get_element([&, this]
		    (const editor &e)
		    {
			    const auto &font=e->impl->font->fc(IN_THREAD);
			    const auto &config=e->impl->config;

			    dim_t height=dim_t::truncate
				    (config.rows*dim_t::value_type
				     (font->height()));

			    dim_t width=dim_t::truncate
				    (config.columns*dim_t::value_type
				     (font->nominal_width()));

			    if (width == dim_t::infinite())
				    --width;

			    get_horizvert(IN_THREAD)->set_element_metrics
				    (IN_THREAD,
				     {width, width, width},
				     {height, height, height});
		    });
}

bool editor_containerObj::implObj::process_button_event(IN_THREAD_ONLY,
							int button,
							bool press,
							xcb_timestamp_t
							timestamp,
							const input_mask &mask)
{
	if ((button != 1 && button != 2) || !press)
		return false;

	invoke_layoutmanager
		([&]
		 (const ref<peepholeObj::layoutmanager_implObj> &lm)
		 {
			 editor e=lm->peephole_element;

			 e->impl->set_focus(IN_THREAD);
		 });
	return true;
}

LIBCXXW_NAMESPACE_END
