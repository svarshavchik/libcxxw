/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/bordercontainer_impl.H"
#include "x/w/impl/container.H"
#include "x/w/impl/element.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/scratch_buffer.H"
#include "screen.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

bordercontainer_implObj
::bordercontainer_implObj(const container_impl &parent_container)
	: bordercontainer_implObj{parent_container,
				  parent_container->get_window_handler()}
{
}

bordercontainer_implObj
::bordercontainer_implObj(const container_impl &parent_container,
			  generic_windowObj::handlerObj &handler)
	: bordercontainer_implObj{parent_container,
				  handler.drawable_pictformat,
				  handler.get_screen()
				  ->find_alpha_pictformat_by_depth(1),
				  handler.get_screen()}
{
}

bordercontainer_implObj
::bordercontainer_implObj(const container_impl &parent_container,
			  const const_pictformat &my_pictformat,
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

LIBCXXW_NAMESPACE_END