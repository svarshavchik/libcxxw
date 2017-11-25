/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_button_h
#define image_button_h

#include "x/w/image_button.H"
#include "x/w/alignment.H"
#include "x/w/factoryfwd.H"
#include "image_button_internalfwd.H"
#include "image_button_internalobj.H"

LIBCXXW_NAMESPACE_START

typedef ref<image_button_internalObj::implObj>
image_button_internal_factory_t(const ref<containerObj::implObj>);

//! Type-erased create_image_button() and create_image_button_with_label_factory().

image_button
do_create_image_button(bool disable_recursive_visibility,
		       const function<image_button_internal_factory_t>
		       &img_impl_factory,
		       factoryObj &f,
		       valign alignment,

		       const function<void (const factory &)> &label_factory)
	LIBCXX_HIDDEN;

///////////////////////////////////////////////////////////////////////////


//! Creates an image_button

//! \see create_image_button

template<typename functor1>
image_button
create_image_button_with_label_factory(bool disable_recursive_visibility,
				       functor1 &&creator,
				       factoryObj &f,
				       valign alignment,
				       const function<void (const factory &)>
				       &label_factory)
	LIBCXX_HIDDEN;

template<typename functor1>
image_button
create_image_button_with_label_factory(bool disable_recursive_visibility,
				       functor1 &&creator,
				       factoryObj &f,
				       valign alignment,
				       const function<void (const factory &)>
				       &label_factory)
{
	return do_create_image_button(disable_recursive_visibility,
				      make_function
				      <image_button_internal_factory_t>
				      (std::forward<functor1>(creator)),
				      f, alignment,
				      label_factory);
}

//! Creates an image_button

//! An image_button is a container with a grid layout manager containing a
//! focus frame, and an image_button_internal.
//!
//! The ignore_visibility parameter disables recursive visibility changes.
//!
//! create_image_button()'s creator parameter is a closure that takes
//! a containerObj::implObj and constructs an image_button_internalObj::implObj
//! subclass, returning it. create_image_button() takes care of show()ing it.
//!
//! Image buttons have optional labels. The label_creator parameter must
//! be a closure that takes a factory parameter. If no label is needed, then
//! the closure should return without doing anything.
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
create_image_button(bool disable_recursive_visibility,
		    functor1 &&creator,
		    factoryObj &f,
		    valign alignment,
		    functor2 &&label_creator)
	LIBCXX_HIDDEN;

template<typename functor1, typename functor2>
image_button
create_image_button(bool disable_recursive_visibility,
		    functor1 &&creator,
		    factoryObj &f,
		    valign alignment,
		    functor2 &&label_creator)
{
	return create_image_button_with_label_factory
		(disable_recursive_visibility,
		 std::forward<functor1>(creator), f,
		 alignment,
		 make_function<void (const factory &)>
		 (std::forward<functor2>(label_creator)));
}

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
