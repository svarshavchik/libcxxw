/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/book_appearance.H"
#include "x/w/image_button_appearance.H"

LIBCXXW_NAMESPACE_START

book_appearance_properties::book_appearance_properties()
	: background_color{"page_background"},
	  border{"page_border"},
	  tabs_background_color{"book_tab_background_color"},
	  tab_border{"book_tab_border"},
	  scroll_button_height{"book_scroll_height"},
	  left_scroll_button{image_button_appearance::base::book_left_theme()},
	  right_scroll_button{image_button_appearance::base::book_right_theme()}
{
}

book_appearance_properties::~book_appearance_properties()=default;

book_appearanceObj::book_appearanceObj()=default;

book_appearanceObj::~book_appearanceObj()=default;

book_appearanceObj::book_appearanceObj(const book_appearanceObj &o)
	: book_appearance_properties{o}
{
}

book_appearance book_appearanceObj::clone() const
{
	return book_appearance::create(*this);
}

const const_book_appearance &book_appearance_base::theme()
{
	static const const_book_appearance config=
		const_book_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
