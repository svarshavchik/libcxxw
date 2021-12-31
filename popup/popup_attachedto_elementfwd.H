/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_elementfwd_h
#define popup_attachedto_elementfwd_h

#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/nonrecursive_visibilityfwd.H"
#include "x/w/impl/child_elementobj.H"
#include "x/w/impl/container.H"
#include "x/w/impl/popup/popupfwd.H"

#include "x/w/elementfwd.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/factoryfwd.H"
#include "x/w/containerobj.H"
#include "x/w/border_arg.H"
#include "x/w/rgb.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/gridlayoutmanagerobj.H"
#include "x/w/element_popup_appearancefwd.H"

#include "popup/popup_attachedto_infofwd.H"
#include "popup/popup_impl.H"
#include "peephole/peephole_toplevel.H"
#include "peephole/peepholed_attachedto_containerobj.H"
#include "image_button_internalfwd.H"

#include <x/ptrfwd.H>
#include <x/functional.H>

#include <tuple>

LIBCXXW_NAMESPACE_START

//! Alias

//! \see popup_attachedto_element_impl

typedef nonrecursive_visibilityObj<container_elementObj
				  <child_elementObj>
				   > popup_attachedto_element_implObj;

//! Implementation class for an element with an attached popup

//! The implementation object for an element with a popup is the
//! \ref nonrecursive_visibilityObj "non recursive visibility mixin"
//! for a container implementation object. The container

typedef ref<popup_attachedto_element_implObj> popup_attachedto_element_impl;

//! A constant \ref popup_attachedto_element_impl "implementation object for an element with a popup".

//! \see popup_attachedto_element_impl

typedef const_ref<popup_attachedto_element_implObj
		  > const_popup_attachedto_element_impl;

//! A nullable pointer reference to an \ref popup_attachedto_element_impl "implementation object for an element with a popup".

//! \see popup_attachedto_element_impl

typedef ptr<popup_attachedto_element_implObj> popup_attachedto_element_implptr;

//! A nullable pointer reference to a const \ref popup_attachedto_element_impl "implementation object for an element with a popup".

//! \see popup_attachedto_element_impl

typedef const_ptr<popup_attachedto_element_implObj
		  > const_popup_attachedto_element_implptr;

//! What create_attachedto_element() returns.

typedef std::tuple<container_impl,
		   image_button_internal,
		   gridlayoutmanager,
		   popup
		   > create_attachedto_element_ret_t;

//! Signature of the factory for the popup contents container impl object.

typedef peepholed_attachedto_container_impl
create_popup_contents_impl_t(const container_impl &,
			     const child_element_init_params &);

//! Signature of the factory for the popup contents container public object.

typedef peepholed_attachedto_container
create_popup_contents_t(const popup_attachedto_info &,
			const ref<gridlayoutmanagerObj::implObj> &);

//! Create a display element with an attached popup.

//! Implements the framework for a display element with a button that
//! opens a popup.
//!
//! The first parameter is the parent container. The second parameter
//! is a popup_attachedto_info object constructed with default parameters.
//!
//! Receives the return value from a factory object's get_container_impl(),
//! an \ref element_popup_appearance "element_popup_appearance", and
//! three closures.
//!
//! The first closure receives two parameters: a
//! \ref container_impl "container implementation object", and a
//! \ref child_element_init_params "child_element_init_params". The first
//! closure needs to create a
//! \ref peepholed_attachedto_containerObj::implObj "peepholed_attachedto_containerObj::implObj"
//! object, constructed using the two parameters, and return it. This is
//! the implementation object for the contents of the attached popup.
//!
//! The second closure gets called next, with the forwarded
//! \ref popup_attachedto_info "popup_attachedto_info object",
//! and a \ref gridlayoutmanagerObj::implObj "gridlayoutmanagerObj::implObj".
//! The popup's contents use the grid layout manager. The second closure
//! must create and return a
//! \ref peepholed_attachedto_container "peepholed_attachedto_container"
//! that gets constructed using the object returned by the second closure
//! (the element's "internal implementation" object), and the third closure's
//! parameters.
//!
//! The returned peepholed container element gets show_all()ed automatically.
//!
//! The third closure gets called with a \ref factory "factory" parameter.
//! The closure uses the factory to construct something that shows the
//! "current value" of the new display element. The closure must show()
//! its created display element.
//!
//! create_popup_attachedto_element() returns the following values:
//!
//! - the new display element's \ref containerObj::implObj "container implementation object".
//!
//! - an \ref image_button_internal "image_button_internal" display element,
//! the button that opens the popup. This display element has already been
//! placed in the container, together with the "current value" element.
//!
//! - the \ref "gridlayoutmanager "gridlayoutmanager" of the new display
//! element.
//!
//! - the \ref "popup" object.
//!
//! The caller is responsible for creating the new "public" object representing
//! the display element.
//!
//! Since the popup has only weak references, the popup object needs to be
//! referenced by the new "public" object. The new public object must be
//! a subclass of a \ref focusable_container "focusable_container" and implement
//! get_impl() by, at least, forwarding it to the image button's get_impl().
//! The current value, created by the first closure, may not be focusable
//! but the popup button is, so at the very least the new display element
//! must front the focusable. It must be a container, and the returned
//! container implementation object, and the returned gridlayoutmanager's
//! implementation object gets used to construct the container superclass.

template<typename current_value_t,
	 typename popup_contents_impl_t,
	 typename popup_contents_t> create_attachedto_element_ret_t
create_popup_attachedto_element(
	const container_impl &parent_container,
	const popup_attachedto_info &attachedto_info,
	const const_element_popup_appearance &appearance,
	popup_contents_impl_t &&popup_contents_impl,
	popup_contents_t &&popup_contents,
	current_value_t &&current_value)
{
	return create_popup_attachedto_element_impl
		(parent_container, attachedto_info, appearance,
		 make_function<create_popup_contents_impl_t>
		 (std::forward<popup_contents_impl_t>(popup_contents_impl)),
		 make_function<create_popup_contents_t>
		 (std::forward<popup_contents_t>(popup_contents)),
		 make_function<void (const gridfactory &)>
		 (std::forward<current_value_t>(current_value)));

}

//! Type-erased created_popup_attachedto_element().

create_attachedto_element_ret_t
create_popup_attachedto_element_impl(
	const container_impl &parent_container,
	const popup_attachedto_info &attachedto_info,
	const const_element_popup_appearance &appearance,
	const function<create_popup_contents_impl_t
	> &popup_contents_impl_factory,
	const function<create_popup_contents_t> &popup_contents_factory,
	const function<void (const gridfactory &)> &current_value_factory)
	LIBCXX_HIDDEN;


LIBCXXW_NAMESPACE_END

#endif
