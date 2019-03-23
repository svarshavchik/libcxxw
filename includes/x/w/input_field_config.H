/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_config_h
#define x_w_input_field_config_h

#include <cstdint>

#include <x/w/types.H>
#include <x/w/alignment.H>
#include <x/w/input_field_configfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/input_field_appearance.H>
#include <x/w/text_param.H>
#include <x/w/scrollbar.H>
#include <x/w/factory.H>
#include <x/functionalrefptr.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! The configuration of an \ref input_field "input field".

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

	//! Optional hint

	//! If specified, this is displayed in the field when it's empty and
	//! and it does not have input focus.

	text_param hint;

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

	//! The input field's appearance
	const_input_field_appearance appearance=
		input_field_appearance::base::theme();

	//! Destructor

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
