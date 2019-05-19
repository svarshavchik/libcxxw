/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_images_appearancefwd_h
#define x_w_scrollbar_images_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC scrollbar_images_appearanceObj;
class scrollbar_images_appearance_base;

/*! A set of images that draws the various parts of the scrollbar.

Specifies individual images that get used to draw a scrollbar.

\code
INSERT_LIBX_NAMESPACE::w::scrollbar_images_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_scrollbar_images_appearance custom=INSERT_LIBX_NAMESPACE::w::scrollbar_images_appearance::base::horizontal1()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::scrollbar_images_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default images1 are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::scrollbar_images_appearance::base::horizontal1()
and
\c INSERT_LIBX_NAMESPACE::w::scrollbar_images_appearance::base::vertical1()
return the default images for the horizontal and vertical scrollbar.
\c INSERT_LIBX_NAMESPACE::w::scrollbar_images_appearance::base::horizontal2()
and
\c INSERT_LIBX_NAMESPACE::w::scrollbar_images_appearance::base::vertical2()
return the default images for the horizontal and vertical scrollbar when
the corresponding portion of the scrollbar is selected (via the pointer or
the keyboard).

*/

typedef ref<scrollbar_images_appearanceObj,
	    scrollbar_images_appearance_base> scrollbar_images_appearance;

//! Constant \ref scrollbar_images_appearance "WHATITIS".

//! \see scrollbar_images_appearance

typedef const_ref<scrollbar_images_appearanceObj,
		  scrollbar_images_appearance_base> const_scrollbar_images_appearance;

//! A nullable pointer reference to a \ref scrollbar_images_appearance "WHATITIS".

//! \see scrollbar_images_appearance

typedef ptr<scrollbar_images_appearanceObj,
	    scrollbar_images_appearance_base> scrollbar_images_appearanceptr;

//! A nullable pointer reference to a const \ref scrollbar_images_appearance "WHATITIS".

//! \see scrollbar_images_appearance

typedef const_ptr<scrollbar_images_appearanceObj,
		  scrollbar_images_appearance_base> const_scrollbar_images_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
