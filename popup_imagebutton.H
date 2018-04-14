/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_imagebutton_H
#define popup_imagebutton_H

#include "x/w/border_arg.H"
#include "x/w/rgb.H"
#include "x/w/element.H"
#include "x/w/gridfactory.H"
#include "image_button_internal.H"
#include "focus/focusframecontainer_impl.H"
#include "focus/focusframecontainer_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/child_element.H"
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Configuration of a popup-showing image button.

//! \see create_popup_imagebutton

struct LIBCXX_HIDDEN popup_imagebutton_config {

	//! The border for the grid cell with the popup button.

	border_arg grid_cell_border;

	//! The popup button's grid cell background color
	color_arg grid_cell_background_color;

	//! Popup button image
	std::string image1;

	//! Popup button image
	std::string image2;

	//! Popup button's focus "off" border.
	border_arg focusoff_border;

	//! Popup button's focus "on" border.
	border_arg focuson_border;
};

//! Type of a factory for a popup image button's focus frame.

//! This constructs the implementation object for the image button focus frame
//! that gets added to the grid factory passed as the first parameter
//! to create_popup_imagebutton().
//!
//! The factory should construct a subclass of
//! \ref popup_imagebutton_focusframe_implObj "popup_imagebutton_focusframe_implObj",
//! the base mixin composition for the focus frame.
//!
//! The factory receives the arguments that are expected to be passed to the
//! base mixin composition.

typedef ref<focusframecontainer_implObj>
popup_imagebutton_focusframe_factory(const border_arg &,
				     const border_arg &,
				     const container_impl &,
				     const child_element_init_params &);

//! Superclass that's returned from the \ref popup_imagebutton_focusframe_factory "popup_imagebutton_focusframe_factory".

typedef focusframecontainer_elementObj<always_visibleObj<
					       container_elementObj
					       <child_elementObj>>
				       > popup_imagebutton_focusframe_implObj;


//! Type-erased create_popup_imagebutton().

image_button_internal
do_create_popup_imagebutton(const gridfactory &f,
			    const function<popup_imagebutton_focusframe_factory>
			    &ff_factory,
			    const ref<elementObj::implObj> &popup_element,
			    const popup_imagebutton_config &config)
	LIBCXX_HIDDEN;


//! Create a popup image button.

//! A button that shows or hides a popup. Used by combo-boxes and date input
//! fields.
//!
//! They are implemented as an internal container with the grid layout
//! manager. The combo-box element, or the date input field element, is in
//! cell (0, 0) and this creates a button that goes into the cell next to it.

template<typename focusframe_factory>
image_button_internal
create_popup_imagebutton(//! Factory for the grid cell with the popup button
			 const gridfactory &f,

			 //! A \ref popup_imagebutton_focusframe_factory "focus frame factory".
			 focusframe_factory &&ff,

			 //! Popup that gets shown or hidden.
			 const ref<elementObj::implObj> &popup_element,

			 //! Visual appearance of the button.
			 const popup_imagebutton_config &config)
{
	return do_create_popup_imagebutton
		(f, make_function<popup_imagebutton_focusframe_factory>
		 (std::forward<focusframe_factory>(ff)),
		 popup_element,
		 config);
}

LIBCXXW_NAMESPACE_END

#endif
