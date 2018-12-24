/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_create_popupfwd_h
#define peepholed_toplevel_listcontainer_create_popupfwd_h

#include "x/w/containerfwd.H"
#include "x/w/containerobj.H"
#include "x/w/impl/always_visible.H"
#include "listlayoutmanager/listcontainer_pseudo_implfwd.H"
#include "peepholed_toplevel_listcontainer/impl_elementfwd.H"
#include "peepholed_toplevel_listcontainer/elementfwd.H"
#include "peepholed_toplevel_listcontainer/handlerfwd.H"
#include "peephole/peepholed_toplevel_element.H"
#include "peephole/peepholed_toplevelfwd.H"
#include "popup/popup_attachedto_infofwd.H"

#include <tuple>

LIBCXXW_NAMESPACE_START

//! Typedef alias.

//! The class used by create_peepholed_toplevel_listcontainer_popup() for
//! the list layout manager's container's implementation object.

typedef always_visibleObj<
	peepholed_toplevel_listcontainer_impl_elementObj<
		listcontainer_pseudo_implObj>
	> p_t_l_impl_t;

//! Typedef alias.

//! The class used by create_peepholed_toplevel_listcontainer_popup() for
//! the list layout manager's container.

typedef peepholed_toplevel_listcontainer_elementObj<
	peepholed_toplevel_elementObj<containerObj>> p_t_l_t;

//! Return value from the content factory passed to \ref create_peepholed_toplevel_listcontainer_popup "create_peepholed_toplevel_listcontainer_popup".

typedef std::tuple<container, peepholed_toplevel> create_popup_factory_ret_t;

//! Signature of the content factory passed to \ref create_peepholed_toplevel_listcontainer_popup "create_peepholed_toplevel_listcontainer_popup".

typedef create_popup_factory_ret_t
create_p_t_l_t(const container_impl &,
	       const popup_attachedto_info &);

//! Signature for the handler factory parameter to \ref create_peepholed_toplevel_listcontainer_popup "create_peepholed_toplevel_listcontainer_popup"().

typedef peepholed_toplevel_listcontainer_handler
create_p_t_l_handler_t(const peepholed_toplevel_listcontainer_handler_args &);

//! Default handler factory parameter for \ref create_peepholed_toplevel_listcontainer_popup "create_peepholed_toplevel_listcontainer_popup"().

peepholed_toplevel_listcontainer_handler
create_p_t_l_handler(const peepholed_toplevel_listcontainer_handler_args &)
	LIBCXX_HIDDEN;

struct LIBCXX_HIDDEN create_peepholed_toplevel_listcontainer_popup_args;

LIBCXXW_NAMESPACE_END
#endif
