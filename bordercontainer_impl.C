/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/bordercontainer_impl.H"
#include "x/w/impl/container.H"
#include "x/w/impl/element.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/scratch_buffer.H"
#include "screen.H"
#include "defaulttheme.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

bordercontainer_implObj
::bordercontainer_implObj(generic_windowObj::handlerObj &handler)
	: bordercontainer_implObj{handler.drawable_pictformat,
				  handler.get_screen()
				  ->find_alpha_pictformat_by_depth(1),
				  handler.get_screen()}
{
}

bordercontainer_implObj
::bordercontainer_implObj(const const_pictformat &my_pictformat,
			  const const_pictformat &mask_pictformat,
			  const screen &s)
	// Same IDs as in straight_border_impl.C
	: corner_scratch_buffer{s->impl->create_scratch_buffer
				(s, "corner@libcxx.com",
				 my_pictformat)},
	  h_scratch_buffer{s->impl->create_scratch_buffer
			   (s, "horiz-border@libcxx.com",
			    my_pictformat)},
	  v_scratch_buffer{s->impl->create_scratch_buffer
			   (s, "vert-border@libcxx.com",
			    my_pictformat)},
	  corner_mask_buffer{s->impl->create_scratch_buffer
			     (s, "corner@libcxx.com",
			      mask_pictformat)},
	  h_mask_buffer{s->impl->create_scratch_buffer
			(s, "horiz-border@libcxx.com",
			 mask_pictformat)},
	  v_mask_buffer{s->impl->create_scratch_buffer
			(s, "vert-border@libcxx.com",
			 mask_pictformat)}
{
}

bordercontainer_implObj::~bordercontainer_implObj()=default;
void bordercontainer_implObj::initialize(ONLY IN_THREAD)
{
	auto title=get_title(IN_THREAD);

	if (!title)
		return;

	title->theme_updated(IN_THREAD,
			     get_container_impl().container_element_impl()
			     .get_screen()->impl->current_theme.get());
}
//! Implement set_border().

//! Inherited from bordercontainer_implObj.

void bordercontainer_implObj::set_border(ONLY IN_THREAD,
					 const layoutmanager &layout,
					 const border_arg &new_left_border,
					 const border_arg &new_right_border,
					 const border_arg &new_top_border,
					 const border_arg &new_bottom_border)
{
	if (do_set_border(IN_THREAD,
			  new_left_border,
			  new_right_border,
			  new_top_border,
			  new_bottom_border))
	{
		// Make sure that recalculate() gets called to clear
		// the cached border info.
		layout->set_modified();
	}
}

LIBCXXW_NAMESPACE_END
