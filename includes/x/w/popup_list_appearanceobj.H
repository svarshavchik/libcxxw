/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_popup_list_appearanceobj_h
#define x_w_popup_list_appearanceobj_h

#include <x/w/popup_list_appearancefwd.H>
#include <x/w/list_appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! All properties inherited by a \ref popup_list_appearance "popup list appearance object".

//! That's in addition to the ones that are inherited from the
//! \ref list_appearance "list appearance properties" object.

struct popup_list_appearance_properties {

	//! Constructor

	popup_list_appearance_properties();

	//! Destructor

	~popup_list_appearance_properties();

	//! Popup's border.

	border_arg popup_border;

	//! Popup's background color when it's above/to the left of its element
	color_arg topleft_color;

	//! Popup's background color when it's below/to the right of its element
	color_arg bottomright_color;

	//! Popup's shade color

	//! The color the popup gets shaded with, when it's blocked by
	//! a modal dialog (unlikely).
	color_arg modal_shade_color;
};

//! Implementation object.

//! \see popup_list_appearance

class popup_list_appearanceObj : public list_appearanceObj,
				 public popup_list_appearance_properties {

public:
	//! Constructor
	popup_list_appearanceObj();

	//! Destructor
	~popup_list_appearanceObj();

	//! Copy constructor
	popup_list_appearanceObj(const popup_list_appearanceObj &);

	//! Clone this object.
	popup_list_appearance clone() const;
};

LIBCXXW_NAMESPACE_END

#endif
