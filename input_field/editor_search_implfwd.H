/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editor_search_implfwd_h
#define x_w_editor_search_implfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN editor_search_implObj;

/*! Implement searching in editorObj::implObj

A subclass of editorObj::implObj that gets constructed if a search callback
was specified for the input field.

Multiply-inherits from input_field_search_threadObj to implement the search
interface.

*/

typedef ref<editor_search_implObj> editor_search_impl;

//! A constant \ref editor_search_impl "editor implementation with searching".

//! \see editor_search_impl

typedef const_ref<editor_search_implObj> const_editor_search_impl;

//! A nullable pointer reference to a \ref editor_search_impl "editor implementation with searching".

//! \see editor_search_impl

typedef ptr<editor_search_implObj> editor_search_implptr;

//! A nullable pointer reference to a const \ref editor_search_impl "editor implementation with searching".

//! \see editor_search_impl

typedef const_ptr<editor_search_implObj> const_editor_search_implptr;

LIBCXXW_NAMESPACE_END

#endif
