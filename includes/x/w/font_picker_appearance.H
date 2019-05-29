/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_picker_appearance_h
#define x_w_font_picker_appearance_h

#include <x/w/font_picker_appearancefwd.H>
#include <x/w/font_picker_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref font_picker_appearance "font_picker appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::font_picker_appearance::base.

class font_picker_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_font_picker_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
