/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_images_appearance_h
#define x_w_scrollbar_images_appearance_h

#include <x/w/scrollbar_images_appearancefwd.H>
#include <x/w/scrollbar_images_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref scrollbar_images_appearance "scrollbar_images appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::scrollbar_images_appearance::base.

class scrollbar_images_appearance_base : public ptrref_base {

public:

	//! Horizontal scrollbar images
	static const_scrollbar_images_appearance horizontal1();

	//! Horizontal scrollbar clicked images
	static const_scrollbar_images_appearance horizontal2();

	//! Vertical scrollbar images
	static const_scrollbar_images_appearance vertical1();

	//! Vertical scrollbar clicked image
	static const_scrollbar_images_appearance vertical2();
};

LIBCXXW_NAMESPACE_END

#endif
