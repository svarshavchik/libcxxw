/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_images_set_elementfwd_h
#define icon_images_set_elementfwd_h

#include "x/w/impl/iconfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"
#include <x/ptrfwd.H>
#include <utility>

LIBCXXW_NAMESPACE_START

template<typename>
class LIBCXX_HIDDEN icon_1tag;

//! One of the base classes inherited by an icon_images_set_element_impl.

template<typename>
class icon_1tag {

	//! Stored icon.

	icon tagged_icon_thread_only;
 public:

	THREAD_DATA_ONLY(tagged_icon);

	icon_1tag(const icon &i);
	void initialize(ONLY IN_THREAD);
	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme);
	void resize(ONLY IN_THREAD, dim_t w, dim_t h, icon_scale scale);
};

//! icon_images_set_elementObj implementation.

template<typename element_impl, typename ...Tags>
struct LIBCXX_HIDDEN icon_images_set_element_implObj;

//! Specialization

//! All icon_images_set_elementObj variadic types have been processed.
//! Inherit from the remaining template parameter.

template<typename element_impl>
struct icon_images_set_element_implObj<element_impl>
	: public element_impl {

	using element_impl::element_impl;
};

//! Specialization

//! Multiply-inherit from one icon_1tag.

template<typename element_impl, typename Tag1, typename ...Tags>
struct icon_images_set_element_implObj<element_impl, Tag1,
						     Tags...>
	: icon_images_set_element_implObj<element_impl, Tags...>,
	icon_1tag<Tag1> {

	//! Main superclass alias.

	typedef icon_images_set_element_implObj<element_impl, Tags...
						> superclass_t;

	template<typename ...Args>
		inline icon_images_set_element_implObj(const icon &i,
						       Args && ...args);

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override;
};

//! A tagged set alternative to an icon_image_vector

//! Implements a mixin for attaching a set of icons to an element.
//! The first template parameter is a subclass of elementObj::implObj.
//!
//! The remaining template parameters is a variadic list of types. The
//! resulting class multiply inherits from the first parameter and
//! \c icon_1tag<T> for every type in the variadic type list. The types are used
//! only for tagging and creating a unique \c icon_1tag<T> superclass.
//!
//! The variadic type list defaults to a single type \c void.
//!
//! The first N elements to the constructor, where N is the number of
//! types in the variadic list, are the initial list of icons. The remaining
//! constructor parameters get forwarded to the main superclass.

template<typename element_impl, typename Tag1=void, typename ...Tags>
using icon_images_set_elementObj=icon_images_set_element_implObj<
	element_impl, Tag1, Tags...>;

LIBCXXW_NAMESPACE_END

#endif
