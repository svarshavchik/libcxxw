/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_button_appearance_h
#define x_w_button_appearance_h

#include <x/w/button_appearancefwd.H>
#include <x/w/button_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref button_appearance "button appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::button_appearance::base.

class button_appearance_base : public ptrref_base {

public:

	//! Cached appearance of a normal button.

	static const_button_appearance normal_theme();

	//! Cached appearance of a button that represents the "default" action.

	static const_button_appearance default_theme();
};

LIBCXXW_NAMESPACE_END

#endif
