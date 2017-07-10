/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemlayoutmanager_impl.H"
#include "themedim.H"
#include "screen.H"
#include "element.H"
#include "child_element.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

listitemlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl,
	  const element &initial_element,
	  const themedim &left_padding,
	  const themedim &right_padding,
	  const themedim &v_padding)
	: singletonlayoutmanagerObj(container_impl, initial_element),
	left_padding(left_padding),
	right_padding(right_padding),
	v_padding(v_padding)
{
}

listitemlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager listitemlayoutmanagerObj::implObj::create_public_object()
{
	return listitemlayoutmanager::create(ref<implObj>(this));
}

dim_t listitemlayoutmanagerObj::implObj::get_left_padding(IN_THREAD_ONLY)
{
	return left_padding->pixels(IN_THREAD);
}

dim_t listitemlayoutmanagerObj::implObj::get_right_padding(IN_THREAD_ONLY)
{
	return right_padding->pixels(IN_THREAD);
}

dim_t listitemlayoutmanagerObj::implObj::get_top_padding(IN_THREAD_ONLY)
{
	return v_padding->pixels(IN_THREAD);
}

dim_t listitemlayoutmanagerObj::implObj::get_bottom_padding(IN_THREAD_ONLY)
{
	return v_padding->pixels(IN_THREAD);
}


void listitemlayoutmanagerObj::implObj
::theme_updated(IN_THREAD_ONLY,
		const defaulttheme &new_theme)
{
	// themedims are owned by the parent container, which will be recalced
	// first.

	recalculate(IN_THREAD);
}


LIBCXXW_NAMESPACE_END
