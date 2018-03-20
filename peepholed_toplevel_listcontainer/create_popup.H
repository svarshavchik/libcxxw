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
#include "always_visible.H"
#include "peepholed_toplevel_listcontainer/impl_element.H"
#include "peepholed_toplevel_listcontainer/element.H"
#include "peepholed_toplevel_listcontainer/layoutmanager_impl.H"
#include "peephole/peepholed_toplevel_element.H"
#include "element.H"
#include "shared_handler_data.H"
#include "x/w/border_arg.H"
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Typedef alias.

//! The class used by create_peepholed_toplevel_listcontainer_popup() for
//! the list layout manager's container's implementation object.

typedef always_visibleObj<
	peepholed_toplevel_listcontainer_impl_elementObj<
		listcontainer_dim_elementObj<list_container_implObj>>
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
create_p_t_l_t(const ref<containerObj::implObj> &,
	       const popup_attachedto_info &);

//! The type that specifies what kind of a popup it is.

typedef std::tuple<
	ref<obj> (shared_handler_dataObj::*)(IN_THREAD_ONLY,
					   const ref<popupObj::handlerObj> &),

	void (shared_handler_dataObj::*)(IN_THREAD_ONLY,
					 const popupObj::handlerObj &)
	> popup_type_t;

popup_type_t create_exclusive_popup() LIBCXX_HIDDEN;

popup_type_t create_menu_popup() LIBCXX_HIDDEN;

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

	//! This is set to either \c create_exclusive_popup or
	//! \c create_menu_popup.

	popup_type_t (*popup_type)();

	//! The style of the list container.
	const new_listlayoutmanager &list_style;

	//! Popup's background color when it's above/to the left of its element
	color_arg topleft_color;

	//! Popup's background color when it's below/to the right of its element
	color_arg bottomright_color;

};

//! Return value from create_peepholed_toplevel_listcontainer_popup().

typedef std::tuple<popup, popup_attachedto_handler> create_p_t_l_popup_ret_t;

template<typename factory_t>
create_p_t_l_popup_ret_t
create_peepholed_toplevel_listcontainer_popup
(const create_peepholed_toplevel_listcontainer_popup_args &args,
 factory_t &&factory)
	LIBCXX_HIDDEN;

//! Type-erased create_peepholed_toplevel_listcontainer_popup().

create_p_t_l_popup_ret_t
do_create_peepholed_toplevel_listcontainer_popup
(const create_peepholed_toplevel_listcontainer_popup_args &args,
 const function<create_p_t_l_t> &) LIBCXX_HIDDEN;

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

template<typename factory_t>
create_p_t_l_popup_ret_t
create_peepholed_toplevel_listcontainer_popup
(const create_peepholed_toplevel_listcontainer_popup_args &args,
 factory_t &&factory)
{
	return do_create_peepholed_toplevel_listcontainer_popup
		(args,
		 make_function<create_p_t_l_t>
		 (std::forward<factory_t>(factory)));
}

LIBCXXW_NAMESPACE_END

#endif
