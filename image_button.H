/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_button_h
#define image_button_h

#include "x/w/image_button.H"
#include "x/w/alignment.H"
#include "x/w/factoryfwd.H"
#include "x/w/border_arg.H"
#include "x/w/rgb.H"
#include "image_button_internalfwd.H"
#include "image_button_internalobj.H"

#include <optional>
#include <string_view>

LIBCXXW_NAMESPACE_START

typedef ref<image_button_internalObj::implObj>
image_button_internal_factory_t(const container_impl &);

//! Parameters used when creating an image button.

struct LIBCXX_HIDDEN create_image_button_info;

//! Fixed parameters to create_image_button()

struct create_image_button_info {
	//! The parent container

	//! The parent_container_impl must be returned by some factory's
	//! get_container_impl(). create_image_button()'s called is
	//! responsible for notifying the factory that the returned
	//! image button was created_internally().
	container_impl parent_container_impl;

	//! Whether the image button is always_visible

	//! By default true, when this image button is used to create another
	//! display element. This employs the always_visible_elementObj mixin.
	const bool always_visible;

	//! Visual appearance details
	const const_image_button_appearance &appearance;

	//! Whether non-recursive visiblity is enabled.

	//! Typically set to true when this image button is used to create
	//! another display element, inside it.
	bool disable_recursive_visibility=false;

	//! Whether clicking anywhere activates the image button.

	//! true: call label_for() on the entire image button element.
	//!
	//! false: call label_for() only on the internal button. Clicking on
	//! any label portion of the image button display element has no effect.

	bool click_anywhere=true;

	//! Button's background color.

	std::optional<color_arg> button_background_color;

	~create_image_button_info();
};

//! Type-erased create_image_button() and create_image_button_with_label_factory().

image_button
do_create_image_button(const create_image_button_info &info,
		       const function<image_button_internal_factory_t>
		       &img_impl_factory,
		       const functionref<void (const factory &)> &label_factory)
	LIBCXX_HIDDEN;

///////////////////////////////////////////////////////////////////////////


//! Creates an image_button

//! \see create_image_button

template<typename functor1>
image_button
create_image_button_with_label_factory(const create_image_button_info &info,
				       functor1 &&creator,
				       const functionref<void (const factory &)>
				       &label_factory)
	LIBCXX_HIDDEN;

template<typename functor1>
image_button
create_image_button_with_label_factory(const create_image_button_info &info,
				       functor1 &&creator,
				       const functionref<void (const factory &)>
				       &label_factory)
{
	return do_create_image_button(info,
				      make_function
				      <image_button_internal_factory_t>
				      (std::forward<functor1>(creator)),
				      label_factory);
}

//! Creates an image_button

//! An image_button is a container with a grid layout manager containing a
//! focus frame, and an image_button_internal.
//!
//! create_image_button()'s creator parameter is a closure that takes
//! a containerObj::implObj and constructs an image_button_internalObj::implObj
//! subclass, returning it. create_image_button() takes care of show()ing it.
//!
//! Image buttons have optional labels. The label_creator parameter must
//! be a closure that takes a factory parameter. If no label is needed, then
//! the closure should return without doing anything. Otherwise the closure
//! can create a single element that represents the image button's label,
//! that appears next to it.
//!
//! create_image_button_with_label_factory is an alternate entry point with
//! the label creator parameter being a type-erased
//! INSERT_LIBX_NAMESPACE::function.
//!
//! The factory where the image_button gets created gets passed in as the
//! factoryObj parameter. The specified valign parameter sets the vertical
//! alignment of the focus frame if the image_button_internal, together with
//! its focusframe, is smaller than the image_button's alloted space (it
//! ends up in a grid layout manager cell that's vertically-filled).

template<typename functor1, typename functor2>
image_button
create_image_button(const create_image_button_info &info,
		    functor1 &&creator,
		    functor2 &&label_creator)
	LIBCXX_HIDDEN;

template<typename functor1, typename functor2>
image_button
create_image_button(const create_image_button_info &info,
		    functor1 &&creator,
		    functor2 &&label_creator)
{
	return create_image_button_with_label_factory
		(info,
		 std::forward<functor1>(creator),
		 make_function<void (const factory &)>
		 (std::forward<functor2>(label_creator)));
}

//! Factory for a typical scroll button.

//! This is used to implement the creator() parameter of create_image_button().
//!
//! Creates an image button of specific height, using the given images.

ref<image_button_internalObj::implObj>
scroll_imagebutton_specific_height(const container_impl
				   &parent_container,
				   const std::vector<std::string> &images,
				   const dim_arg &height) LIBCXX_HIDDEN;

//! Implementation object for the image button display element.

class LIBCXX_HIDDEN image_buttonObj::implObj : virtual public obj {

 public:

	//! The internal focusable button.
	const image_button_internal button;

	//! Constructor
	implObj(const image_button_internal &button);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
