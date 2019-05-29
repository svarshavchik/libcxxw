/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_window_appearance_h
#define x_w_main_window_appearance_h

#include <x/w/main_window_appearancefwd.H>
#include <x/w/main_window_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref main_window_appearance "main_window appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::main_window_appearance::base.

class main_window_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_main_window_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
