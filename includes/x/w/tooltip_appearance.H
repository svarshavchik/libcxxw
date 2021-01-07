/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_appearance_h
#define x_w_tooltip_appearance_h

#include <x/w/tooltip_appearancefwd.H>
#include <x/w/tooltip_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref tooltip_appearance "tooltip appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::tooltip_appearance::base.

class tooltip_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_tooltip_appearance tooltip_theme();

	//! Default theme.

	static const_tooltip_appearance static_tooltip_theme();
};

LIBCXXW_NAMESPACE_END

#endif
