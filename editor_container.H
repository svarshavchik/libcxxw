/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_container_h
#define editor_container_h

#include "editor_containerfwd.H"
#include "editor_containerobj.H"
#include "x/w/factoryfwd.H"
#include "x/w/text_paramfwd.H"
#include "x/w/input_field_config.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Create a \ref editor_container "pepehole container with an editor element".

//! This creates the peephole container with an editor element inside it.
//! The peephole will scroll the editor element to keep its cursor in view.

editor_container
create_editor_container(const factory &in_factory,
			const text_param &initial_contents,
			const input_field_config &config) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
