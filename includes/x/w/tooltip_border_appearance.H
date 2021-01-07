/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_border_appearance_h
#define x_w_tooltip_border_appearance_h

#include <x/w/tooltip_border_appearancefwd.H>
#include <x/w/tooltip_border_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref tooltip_border_appearance "tooltip_border appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::tooltip_border_appearance::base.

class tooltip_border_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_tooltip_border_appearance theme();

	//! Default theme.

	static const_tooltip_border_appearance nonalpha_theme();
};

LIBCXXW_NAMESPACE_END

#endif
