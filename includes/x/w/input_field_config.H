/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_config_h
#define x_w_input_field_config_h

#include <cstdint>

#include <x/w/types.H>
#include <x/w/alignment.H>
#include <x/w/text_param.H>
#include <x/w/input_field_configfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/list_appearance.H>
#include <x/w/scrollbar.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/w/factory.H>
#include <x/functionalrefptr.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! The settings related to a visual appearance of an \ref input_field_config "input field".

struct input_field_config_appearance {

	//! The input field's border.

	border_arg border="textedit_border";

	//! Inner border when the input field does not have input focus

	border_arg focusoff_border="texteditfocusoff_border";

	//! Inner border when the input field has input focus

	border_arg focuson_border="texteditfocuson_border";

	//! Foreground color
	color_arg foreground_color="textedit_foreground_color";

	//! Font for normal input fields
	font_arg regular_font=theme_font{"textedit"};

	//! Font for password input fields
	font_arg password_font=theme_font{"password"};

	//! Background color
	color_arg background_color="textedit_background_color";

	//! Background color when the input field is disabled.

	//! Typical disabled element shading is not sufficient to distingish
	//! the visibility change.

	color_arg disabled_background_color=
		"textedit_disabled_background_color";

	//! Optional hint

	//! If specified, this is displayed in the field when it's empty and
	//! and it does not have input focus.

	text_param hint;

	//! Hint's color
	color_arg hint_color="textedit_hint_color";

	//! Free-play before dragging operation commences

	//! How far the pointer must move before drag operation starts.
	//! Avoids overhead when a pointer button click slips

	dim_arg drag_horiz_start="drag_horiz_start";

	//! Free-play before dragging operation commences

	//! How far the pointer must move before drag operation starts.
	//! Avoids overhead when a pointer button click slips

	dim_arg drag_vert_start="drag_vert_start";

	//! Destructor
	~input_field_config_appearance();
};

//! The settings not related to a visual appearance of an \ref input_field_config "input field".

struct input_field_config_settings {

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

	//! Search callback

	//! Optional callback that enables concurrent searching. The callback
	//! gets invoked to search for potential values that could match the
	//! partially-entered text. The list of values gets presented in a
	//! combo-box style popup below or above the input field.

	functionptr<input_field_search_callback_t> input_field_search_callback;

	//! Appearance of the list search popup.

	const_list_appearance input_field_search_appearance=
		list_appearance::base::theme();

	//! Destructor

	~input_field_config_settings();
};

//! The configuration of an \ref input_field "input field".

//! Inherits from input_field_config_settings, so that the uniform
//! initialization syntax initializes it first.

struct input_field_config : public input_field_config_settings,
			    public input_field_config_appearance {

	~input_field_config();
};

//! Parameter to the search callback.

//! The search callback gets invoked in a separate execution thread.
//! search_string is the string to search, and upon a successfull search,
//! the callback prior to returning should invoke results() with two vectors.
//!
//! The first vector contains the found search results. The 2nd vector lists
//! what should be shown in the search results popup.
//!
//! At this time both vectors must always be the same size. A helper overload
//! takes a single vector of std::u32string, and duplicates it as the 2nd
//! vector.

struct input_field_search_info {

	//! Constructor

	input_field_search_info(const std::u32string &,
				std::vector<std::u32string> &search_results,
				std::vector<list_item_param> &search_items);

	//! Original search string.

	const std::u32string &search_string;

	//! Search result text

	//! What gets placed into the input field, if selected.
	std::vector<std::u32string> &search_results;

	//! Search result list

	//! What goes into the popup list.
	std::vector<list_item_param> &search_items;

	//! Found results
	void results(const std::vector<std::u32string> &text) const;

	//! Returns the mcguffin that indicates an aborted search

	//! If the search gets aborted (additional text gets typed into the
	//! search box while the search is running), the search execution
	//! thread can use this mcguffin to signal it should abort execution.
	//!
	//! The new search does not get started until the callback returns.
	virtual ref<obj> get_abort_mcguffin() const=0;
};

LIBCXXW_NAMESPACE_END

#endif
