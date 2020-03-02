/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_button_appearance_h
#define x_w_image_button_appearance_h

#include <x/w/image_button_appearancefwd.H>
#include <x/w/image_button_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref image_button_appearance "image_button appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::image_button_appearance::base.

class image_button_appearance_base : public ptrref_base {

public:

	//! Default theme.

	//! Returns cached theme-based appearance of checkbox image buttons.
	static const_image_button_appearance checkbox_theme();

	//! Default theme.

	//! Returns cached theme-based appearance of radio image buttons.
	static const_image_button_appearance radio_theme();


	//! Default theme

	//! Returns cached theme-based appearance of close button for items
	//! managed by the item layout manager.

	static const_image_button_appearance item_theme();

	//! Default theme

	//! Returns cached theme-based appearance of the scroll-left button
	//! that the book layout manager uses.
	static const_image_button_appearance book_left_theme();

	//! Default theme

	//! Returns cached theme-based appearance of the scroll-right button
	//! that the book layout manager uses.

	static const_image_button_appearance book_right_theme();

	//! Default theme

	//! Returns cached theme-based appearance of the scroll-left button
	//! that the date input popup uses.
	static const_image_button_appearance date_popup_left_theme();

	//! Default theme

	//! Returns cached theme-based appearance of the scroll-left button
	//! that the date input popup uses.
	static const_image_button_appearance date_popup_right_theme();
};

LIBCXXW_NAMESPACE_END

#endif
