/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_config_h
#define x_w_color_picker_config_h

#include <x/w/color_picker_configfwd.H>
#include <x/w/color_pickerfwd.H>
#include <x/w/color_picker_appearancefwd.H>
#include <x/w/rgb.H>
#include <x/w/screen_positionsfwd.H>
#include <x/functionalrefptr.H>

#include <string>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Wrapper for input field appearance.

//! Separately inherited by \ref color_picker_config "color_picker_config"
//! to avoid the need to fully declare \ref color_picker_appearance "appearance"
//! and pull in all of its header file dependencies.

struct color_picker_config_appearance : const_color_picker_appearance {

	//! Constructor
	color_picker_config_appearance();

	//! Constructor

	using const_color_picker_appearance::const_color_picker_appearance;

	//! Destructor
	~color_picker_config_appearance();

	//! Copy constructor
	color_picker_config_appearance(const color_picker_config_appearance &);

	//! Assignment operator
	color_picker_config_appearance &operator=
	(const color_picker_config_appearance &);

	using const_color_picker_appearance::operator=;

	using const_color_picker_appearance::operator*;

	using const_color_picker_appearance::operator->;
};


//! Initial color picker configuration

//! An optional parameter to a \ref factory "factory"'s
//! create_color_picker() setting the initial value and apperance of a
//! \ref color_picker "INSERT_LIBX_NAMESPACE::w::color_picker".

struct color_picker_config {

	//! Initial callback
	functionptr<color_picker_callback_t> initial_callback;

	//! Initial color

	rgb initial_color=white;

	//! This color picker's label

	//! Set by restore().
	std::string name;

	//! Enable alpha channel picker

	bool enable_alpha_channel=false;

	//! Restore previously-saved color picker configuration.

	//! This resets initial_color

	void restore(const const_screen_positions &,
		     const std::string_view &);

	//! Visual appearance
	color_picker_config_appearance appearance;
};

LIBCXXW_NAMESPACE_END

#endif
