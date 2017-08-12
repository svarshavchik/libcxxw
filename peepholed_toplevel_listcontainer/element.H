/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_element_h
#define peepholed_toplevel_listcontainer_element_h

#include <x/ref.H>
#include "x/w/focusableownerobj.H"
#include "popup/popup_attachedto_info.H"
#include "peepholed_toplevel_listcontainer/elementfwd.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
peepholed_toplevel_listcontainer_elementObj<element_impl>
::peepholed_toplevel_listcontainer_elementObj
(const popup_attachedto_info &attachedto_info,
 const ref<peepholed_toplevel_listcontainer_implObj
 > &toplevel_listcontainer_impl,
 Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  list_focusable_owner(ref<focusableObj::ownerObj>
			       ::create(ref<focusableImplObj>
					(&toplevel_listcontainer_impl
					 ->listcontainer_element()))),
	  attachedto_info(attachedto_info),
	  toplevel_listcontainer_impl(toplevel_listcontainer_impl)
{
}

//! Destructor

template<typename element_impl>
peepholed_toplevel_listcontainer_elementObj<element_impl>
::~peepholed_toplevel_listcontainer_elementObj()=default;

//! Define max peephole size based on our attachedto_info.

template<typename element_impl>
void peepholed_toplevel_listcontainer_elementObj<element_impl>
::recalculate_peepholed_metrics(IN_THREAD_ONLY, const screen &s)
{
	max_width_value=attachedto_info->max_peephole_width(IN_THREAD, s);
	max_height_value=attachedto_info->max_peephole_height(IN_THREAD, s);

	// TODO: we shouldn't need horizontal increment, but default it to
	// something reasonable.

	{
		current_theme_t::lock lock{s->impl->current_theme};

		horizontal_increment_value=(*lock)->compute_width(5);
	}
}

//! Inherited from peepholed_toplevelObj

template<typename element_impl>
dim_t peepholed_toplevel_listcontainer_elementObj<element_impl>
::max_width(IN_THREAD_ONLY) const
{
	return max_width_value;
}

//! Inherited from peepholed_toplevelObj

template<typename element_impl>
dim_t peepholed_toplevel_listcontainer_elementObj<element_impl>
::max_height(IN_THREAD_ONLY) const
{
	return max_height_value;
}

//! Inherited from peepholed_toplevelObj

template<typename element_impl>
dim_t peepholed_toplevel_listcontainer_elementObj<element_impl>
::horizontal_increment(IN_THREAD_ONLY) const
{
	return horizontal_increment_value;
}

//! Inherited from peepholed_toplevelObj

template<typename element_impl>
dim_t peepholed_toplevel_listcontainer_elementObj<element_impl>
::vertical_increment(IN_THREAD_ONLY) const
{
	return toplevel_listcontainer_impl->tallest_row_height(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
