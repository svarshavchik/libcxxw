/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_create_popup_h
#define peepholed_toplevel_listcontainer_create_popup_h

#include "listlayoutmanager/listcontainer_dim_element.H"
#include "popup/popup.H"
#include "popup/popup_attachedto_info.H"
#include "popup/popup_attachedto_handler.H"
#include "x/w/impl/always_visible.H"
#include "peepholed_toplevel_listcontainer/handler.H"
#include "peepholed_toplevel_listcontainer/impl_element.H"
#include "peepholed_toplevel_listcontainer/element.H"
#include "peepholed_toplevel_listcontainer/layoutmanager_impl.H"
#include "peephole/peepholed_toplevel_element.H"
#include "x/w/impl/element.H"
#include "shared_handler_data.H"
#include "x/w/border_arg.H"
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Typedef alias.

//! The class used by create_peepholed_toplevel_listcontainer_popup() for
//! the list layout manager's container's implementation object.

typedef always_visibleObj<
	peepholed_toplevel_listcontainer_impl_elementObj<
		listcontainer_dim_elementObj<listcontainer_pseudo_implObj>>
	> p_t_l_impl_t;

//! Typedef alias.

//! The class used by create_peepholed_toplevel_listcontainer_popup() for
//! the list layout manager's container.

typedef peepholed_toplevel_listcontainer_elementObj<
	peepholed_toplevel_elementObj<containerObj>> p_t_l_t;

//! Return value from the create_p_t_l_impl_t callback.

typedef std::tuple<ref<p_t_l_impl_t>,
		   ref<peepholed_toplevel_listcontainer_layoutmanager_implObj>
		   > create_p_t_l_impl_ret_t;

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

//! Non-factory parameters to create_peepholed_toplevel_listcontainer_popup().

struct LIBCXX_HIDDEN create_peepholed_toplevel_listcontainer_popup_args {

	//! Parent window.

	//! Not really the element the popup gets attached to, but the
	//! parent element of the element the popup gets attached to.
	//! The popup gets created first, then the actual element it
	//! gets attached to. This specified the top level generic window
	//! the element is in.

	ref<elementObj::implObj> parent_element;

	//! The popup's window type.
	const char *popup_window_type;

	//! The popup's window class instance.
	const char *popup_wm_class_instance;

	//! The popup's border.
	std::optional<border_arg> popup_border;

	//! The popup's additional nesting_level.

	//! This gets added to parent_element's nesting_level to set the
	//! popup's nesting_level, for optimal layout recalculations.
	unsigned extra_nesting_level;

	//! How the popup gets attached to the parent_element.
	attached_to attached_to_style;

	//! Popup type
	const popup_visibility_semantics &popup_type;

	//! The style of the list container.
	const new_listlayoutmanager &list_style;

	//! Popup's background color when it's above/to the left of its element
	color_arg topleft_color;

	//! Popup's background color when it's below/to the right of its element
	color_arg bottomright_color;

};

//! Return value from create_peepholed_toplevel_listcontainer_popup().

typedef std::tuple<popup, popup_attachedto_handler> create_p_t_l_popup_ret_t;

template<typename factory_t, typename handler_factory_t>
create_p_t_l_popup_ret_t
create_peepholed_toplevel_listcontainer_popup
(const create_peepholed_toplevel_listcontainer_popup_args &args,
 factory_t &&factory,
 handler_factory_t &&handler_factory)
	LIBCXX_HIDDEN;

//! Type-erased create_peepholed_toplevel_listcontainer_popup().

create_p_t_l_popup_ret_t
do_create_peepholed_toplevel_listcontainer_popup
(const create_peepholed_toplevel_listcontainer_popup_args &args,
 const function<create_p_t_l_t> &,
 const function<create_p_t_l_handler_t> &) LIBCXX_HIDDEN;

//! Construct an attached popup with a \ref listlayoutmanager "list layout manager".

//! Creates a \ref popup "popup" that's attached to a display element in the
//! main window. The popup's container will be, apparently, the
//! \ref listlayoutmanager "list layout manager".
//!
//! create_peepholed_toplevel_listcontainer_popup() takes care of managing
//! the popup's top-level peephole. The popup automatically grows to accomodate
//! the size of the list, but won't go off screen. The peephole gets limited
//! to the screen's size, and, if exceeded, scrolls the contents of the list
//! in the peephole.
//!
//! Two callbacks are used in addition to the fixed arguments:
//!
//! 1. The first callback receives the container implementation object
//! and a \ref popup_attachedto_info "popup_attachedto_info", and should
//! return the constructed container, and a
//! \ref peepholed_toplevel "peepholed_toplevel object".
//!
//! The callback typically constructs a \ref p_t_l_t "p_t_l_t" object
//! and returns it as both values, since it inherits from both
//! a container, and a \ref peepholed_toplevel "peepholed_toplevel" object.
//!
//! 2. The second callback constructs the handler object for the new popup.
//! The callback receives a \ref peepholed_toplevel_listcontainer_handler_args "peepholed_toplevel_listcontainer_handler_args",
//! the argument to
//! \ref peepholed_toplevel_listcontainer_handler "peepholed_toplevel_listcontainer_handler"'s
//! constructor, and should return the constructed object, or its subclass.
//! This factory may be specified as \ref create_p_t_l_handler "create_p_t_l_handler",
//! the default implementation.
//!
//! Returns the new popup, and a
//! \ref popup_attachedto_handlerObj "popup_attachedto_handler".
//! create_peepholed_toplevel_listcontainer_popup() gets called to construct
//! the popup before creating the element the popup gets attached to.
//! The element's implementation object inherits from
//! \ref popup_attachedto_handler_elementObj "popup_attachedto_handler_element",
//! and the returned
//! \ref popup_attachedto_handlerObj "popup_attachedto_handler" gets used
//! to construct the element's implementation object.
//!
//! A popup can only be attached to one element.

template<typename factory_t, typename handler_factory_t>
create_p_t_l_popup_ret_t
create_peepholed_toplevel_listcontainer_popup
(const create_peepholed_toplevel_listcontainer_popup_args &args,
 factory_t &&factory,
 handler_factory_t &&handler_factory)
{
	return do_create_peepholed_toplevel_listcontainer_popup
		(args,
		 make_function<create_p_t_l_t>
		 (std::forward<factory_t>(factory)),
		 make_function<create_p_t_l_handler_t>
		 (std::forward<handler_factory_t>(handler_factory)));
}

LIBCXXW_NAMESPACE_END

#endif
