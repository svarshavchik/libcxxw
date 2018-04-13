/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pagetabgridcontainer_implobj_h
#define pagetabgridcontainer_implobj_h

#include "booklayoutmanager/pagetabgridcontainer_implobj.H"
#include "always_visiblefwd.H"
#include "container_elementfwd.H"
#include "x/w/impl/themedim_elementfwd.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/dim_arg.H"
#include "x/w/containerobj.H"
#include "x/w/elementfwd.H"

LIBCXXW_NAMESPACE_START

struct tab_inactive_color_tag;
struct tab_active_color_tag;
struct tab_h_padding_tag;
struct tab_v_padding_tag;

//! The superclass of a \ref pagetabgridcontainer_implObj "pagetabgridcontainer_implObj".

//! This container mixes up.
//!
//! - alwaysvisibleObj
//!
//! - themedim_elementObj - the padding for the individual tags.
//!
//! - background_color_elementObj - the active and inactive "cold" colors
//!   for the individual hotspots that make up the booklayoutmanager's tab.
//!
//! - container_elementObj - a container.

typedef always_visibleObj<
	themedim_elementObj<
		background_color_elementObj<
			container_elementObj<child_elementObj>,
			tab_inactive_color_tag,
			tab_active_color_tag>,
		tab_h_padding_tag,
		tab_v_padding_tag>> pagetabgridcontainer_impl_superclass_t;

//! The container with the \ref pagetabgridlayoutmanager "pagetabgridlayoutmanager"

class LIBCXX_HIDDEN pagetabgridcontainer_implObj
	: public pagetabgridcontainer_impl_superclass_t {

	//! Alias.

	typedef pagetabgridcontainer_impl_superclass_t superclass_t;

 public:
	//! Constructor

	pagetabgridcontainer_implObj
		(const container_impl &parent_container,
		 const dim_arg &h_padding,
		 const dim_arg &v_padding,
		 const color_arg &inactive_color,
		 const color_arg &active_color);

	//! Destructor
	~pagetabgridcontainer_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
