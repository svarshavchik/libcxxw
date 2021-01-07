/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_button_config_h
#define x_w_button_config_h

#include <x/w/buttonfwd.H>
#include <x/w/button_appearancefwd.H>

LIBCXXW_NAMESPACE_START

//! \ref button "Button" configuration.

struct button_config {

	//! Visual appearance

	const_button_appearance appearance;

	//! Default constructor
	button_config();

	//! Constructor
	button_config(const const_button_appearance &);

	//! Copy constructor
	button_config(const button_config &);

	//! Assignment operator

	button_config &operator=(const button_config &);

	//! Destructor
	~button_config();
};

//! Returns default appearance of a normal \ref button "button".
const button_config &normal_button();

//! Returns default appearance of a default \ref button "button".

//! Typically thicker border, to visually mark the default "Ok", or similar,
//! button, amongst its competitors.
const button_config &default_button();

LIBCXXW_NAMESPACE_END

#endif
