/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_listcontainer_impl.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/impl/theme_font_element.H"
#include "x/w/impl/themedim.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/container.H"

#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

peepholed_listcontainerObj::implObj::implObj()=default;

peepholed_listcontainerObj::implObj::~implObj()=default;

void peepholed_listcontainerObj::implObj::initialize(ONLY IN_THREAD)
{
	update_peephole_metrics(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::horizvert_updated(ONLY IN_THREAD)
{
	update_peephole_metrics(IN_THREAD);
}

dim_t peepholed_listcontainerObj::implObj::rowsize(ONLY IN_THREAD) const
{
	return get_pseudo_impl().rowsize(IN_THREAD).with_padding;
}

size_t peepholed_listcontainerObj::implObj::rows(ONLY IN_THREAD) const
{
	return get_pseudo_impl().rows(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::theme_updated(ONLY IN_THREAD,
		const defaulttheme &new_theme)
{
	update_peephole_metrics(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::update_peephole_metrics(ONLY IN_THREAD)
{
}

LIBCXXW_NAMESPACE_END
