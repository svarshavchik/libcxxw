/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_h
#define editor_h

#include "editorfwd.H"
#include "editorobj.H"
#include "x/w/factoryfwd.H"
#include "x/w/text_paramfwd.H"
#include "x/w/input_field_config.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

editor create_editor(const factory &f,
		     const text_param &initial_contents,
		     const input_field_config &config);

LIBCXXW_NAMESPACE_END

#endif
