/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_h
#define editor_h

#include "editorfwd.H"
#include "editorobj.H"
#include "editor_peephole_impl.H"
#include "x/w/containerobj.H"
#include "x/w/factoryfwd.H"
#include "x/w/text_paramfwd.H"
#include "x/w/input_field_config.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Create the editor display element.

//! \internal
//!
//! Constructs the \ref editor "editor display element" inside an
//! \ref editor_container "editor peephole container".

editor create_editor(const ref<editor_peephole_implObj> &parent_container,
		     const text_param &initial_contents,
		     const input_field_config &config) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
