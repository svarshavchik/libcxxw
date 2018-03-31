/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_config_h
#define x_w_input_field_config_h

#include <cstdint>

#include <x/w/types.H>
#include <x/w/alignment.H>
#include <x/w/input_field_configfwd.H>
#include <x/w/scrollbar.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/w/factory.H>
#include <x/functionalrefptrfwd.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! The configuration of an input field.

struct input_field_config {

	//! Number of columns
	size_t columns=40;

	//! Number of rows
	size_t rows=1;

	//! Auto-select when focus is gained.

	//! Whether the input field should automatically select its entire
	//! contents, when it gains input focus.

	bool autoselect=false;

	//! Deselect the contents of the input field on focus loss.

	//! This option is mainly enabled for editable combo-boxes.
	bool autodeselect=false;

	//! Whether clipboards should be updated.

	//! This is turned off by editable combo-boxes and password fields.
	bool update_clipboards=true;

	//! Whether this is a password field

	//! Setting this to non-zero visually replaces all characters with
	//! the given character.

	char32_t password_char=0;

	//! Set the typical settings for a password field.

	void set_password(char32_t c='*')
	{
		autoselect=true;
		autodeselect=true;
		update_clipboards=false;
		password_char=c;
		alignment=halign::center;
	}

	//! When to show the vertical scrollbar

	//! This is used only when rows > 1.
	scrollbar_visibility vertical_scrollbar=
		scrollbar_visibility::automatic_reserved;

	//! Inner alignment
	halign alignment=halign::left;

	//! Maximum number of unicode characters.

	//! \note
	//! Attempting to set() the contents of the field to a larger string
	//! gets quietly ignored.

	size_t maximum_size=(size_t)-1;

	//! Single line input field.
	constexpr bool oneline() const { return rows <= 1; }

	//! The input field's border.

	border_arg border="textedit_border";

	//! Background color

	color_arg background_color="textedit_background_color";

	//! Background color when the input field is disabled.

	//! Typical disabled element shading is not sufficient to distingish
	//! the visibility change.

	color_arg disabled_background_color=
		"textedit_disabled_background_color";


	//! Optional factories for the spin controls next to the input field.

	std::optional<std::tuple<functionref<void(const factory &)>,
				 functionref<void(const factory &)>>
		      > spin_control_factories;

	//! Custom spin buttons

	//! Enables two spin buttons next to the input field.
	//! The callbacks must use the their factory parameter to create one
	//! display element that shows an icon or a label for that spin
	//! button.

	void set_spin_control_factories(const functionref<void(const factory
							       &)> &,
					const functionref<void(const factory
							       &)> &);

	//! Enable spin buttons, with a default appearance.

	//! Enables two spin buttons next to the input field.
	void set_default_spin_control_factories();

	//! Destructor

	~input_field_config();
};

LIBCXXW_NAMESPACE_END

#endif
