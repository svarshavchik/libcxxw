/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef cursor_pointer_elementfwd_H
#define cursor_pointer_elementfwd_H

#include "x/w/impl/cursor_pointerfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

template<typename>
class LIBCXX_HIDDEN cursor_pointer_1tag;

//! One of the base classes inherited by an cursor_pointer_element_implObj.

template<typename>
class cursor_pointer_1tag {

	//! Stored cursor_pointer.

	cursor_pointer tagged_cursor_pointer_thread_only;
 public:

	THREAD_DATA_ONLY(tagged_cursor_pointer);

	cursor_pointer_1tag(const cursor_pointer &c);
	void initialize(ONLY IN_THREAD);
	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme);
};

//! cursor_pointer_elementObj implementation.

template<typename element_impl, typename ...Tags>
struct LIBCXX_HIDDEN cursor_pointer_element_implObj;

//! Specialization

//! All cursor_pointer_elementObj variadic types have been processed.
//! Inherit from the remaining template parameter.

template<typename element_impl>
struct cursor_pointer_element_implObj<element_impl>
	: public element_impl {

	using element_impl::element_impl;
};

//! Specialization

//! Multiply-inherit from one cursor_pointer_1tag.

template<typename element_impl, typename Tag1, typename ...Tags>
struct cursor_pointer_element_implObj<element_impl, Tag1,
						     Tags...>
	: cursor_pointer_element_implObj<element_impl, Tags...>,
	cursor_pointer_1tag<Tag1> {

	//! Main superclass alias.

	typedef cursor_pointer_element_implObj<element_impl, Tags...
						> superclass_t;

	template<typename ...Args>
		inline cursor_pointer_element_implObj(const cursor_pointer &i,
						       Args && ...args);

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override;
};

//! A mixin for combing a cursor_pointer with an element.

//! Implements a mixin for attaching cursor_pointers to an element.
//! The first template parameter is a subclass of elementObj::implObj.
//!
//! The remaining template parameters is a variadic list of types. The
//! resulting class multiply inherits from the first parameter and
//! \c cursor_pointer_1tag<T> for every type in the variadic type list.
//! The types are used only for tagging and creating a unique
//! \c cursor_pointer_1tag<T> superclass.
//!
//! The variadic type list defaults to a single type \c void.
//!
//! The first N elements to the constructor, where N is the number of
//! types in the variadic list, are the initial list of cursor_pointers.
//! The remaining
//! constructor parameters get forwarded to the main superclass.

template<typename element_impl, typename Tag1=void, typename ...Tags>
using cursor_pointer_elementObj=cursor_pointer_element_implObj<
	element_impl, Tag1, Tags...>;

LIBCXXW_NAMESPACE_END

#endif
