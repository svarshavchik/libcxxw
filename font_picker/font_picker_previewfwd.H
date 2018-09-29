/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_picker_previewfwd_h
#define x_w_font_picker_previewfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN font_picker_previewObj;

/*! Preview of the currently picked font.

A display element in the font picker popup that shows a sample of the
currently picked font and its options. This element is based on the label
element.

This element is placed inside a peephole in the popup, so if the preview
label grows too big, it doesn't blow up the popup.

*/

typedef ref<font_picker_previewObj> font_picker_preview;

//! A constant \ref font_picker_preview "font picker preview label".

//! \see font_picker_preview

typedef const_ref<font_picker_previewObj> const_font_picker_preview;

//! A nullable pointer reference to a \ref font_picker_preview "font picker preview label".

//! \see font_picker_preview

typedef ptr<font_picker_previewObj> font_picker_previewptr;

//! A nullable pointer reference to a const \ref font_picker_preview "font picker preview label".

//! \see font_picker_preview

typedef const_ptr<font_picker_previewObj> const_font_picker_previewptr;

LIBCXXW_NAMESPACE_END

#endif
