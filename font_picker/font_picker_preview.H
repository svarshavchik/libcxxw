/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_picker_preview_h
#define x_w_font_picker_preview_h

#include "font_picker/font_picker_previewfwd.H"
#include "font_picker/font_picker_previewobj.H"
#include "x/w/factoryfwd.H"
#include "x/w/font_picker_configfwd.H"
#include "x/w/containerfwd.H"
#include <x/ref.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Create the font preview widget.

//! Used internal by the font picker popup.
//!
//! Recycled in cxxwcreator.

std::tuple<container, font_picker_preview>
create_font_picker_preview(const factory &f,
			   const font_picker_config &config)
	LIBCXX_PUBLIC;

LIBCXXW_NAMESPACE_END

#endif
