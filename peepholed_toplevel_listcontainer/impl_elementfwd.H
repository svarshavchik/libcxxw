/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_impl_elementfwd_h
#define peepholed_toplevel_listcontainer_impl_elementfwd_h

#include "peepholed_toplevel_listcontainer/impl.H"

LIBCXXW_NAMESPACE_START

//! Combine the peepholed_toplevel_listcontainer_implObj mixin.

//! The mixin used by the listcontainer implementation object that's in
//! a top-level popup peephole.
//!
//! Inherits from peepholed_toplevel_listcontainer_implObj, and the
//! template parameter.
//!
//! All constructor parameters get forwarded to the superclass specified
//! by the template parameter.

template<typename element_impl>
class LIBCXX_HIDDEN peepholed_toplevel_listcontainer_impl_elementObj :
	public element_impl,
	public peepholed_toplevel_listcontainer_implObj
{
 public:

	using element_impl::element_impl;

	//! Destructor
	~peepholed_toplevel_listcontainer_impl_elementObj();

	//! Implement listcontainer_element().

	listcontainer_pseudo_implObj &listcontainer_element() override;
};

LIBCXXW_NAMESPACE_END

#endif
