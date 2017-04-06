/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editorfwd_h
#define editorfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class editorObj;

//! A text editor.

//! This is an internal editing element. It implements just the editing
//! functionality. Additional scaffolding is used to create an externally
//! visible input field: it gets place inside a
//! \ref focusframecontainer "focus frame container", and that goes inside
//! the actual input field, that also supplies the optional scrollbars,
//! if the input field is specified as using them.

typedef ref<editorObj> editor;

//! A nullable pointer reference to an editor.

//! \see editor

typedef ptr<editorObj> editorptr;

//! A reference to a constant editor object.

//! \see editor

typedef const_ref<editorObj> const_editor;

//! A nullable pointer reference to a constant editor object.

//! \see editor

typedef const_ptr<editorObj> const_editorptr;

LIBCXXW_NAMESPACE_END

#endif
