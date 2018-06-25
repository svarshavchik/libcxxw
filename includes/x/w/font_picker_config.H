/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_picker_config_h
#define x_w_font_picker_config_h

#include <x/w/font_picker_configfwd.H>
#include <x/w/font_pickerfwd.H>
#include <x/w/font.H>
#include <x/w/element_popup_config.H>
#include <x/functionalrefptr.H>

#include <optional>

LIBCXXW_NAMESPACE_START

//! Initial font picker configuration

//! An optional parameter to a \ref factory "factory"'s
//! create_font_picker() setting the initial value and appearance of a
//! \ref font_picker "INSERT_LIBX_NAMESPACE::w::font_picker".

struct font_picker_config {

	//! Most recently used font families

	//! These will be shown first, in the dropdown. The "foundry_required"
	//! setting must remain the same, each time the font picker gets
	//! created, in order for the most_recently_used list to work.
	//!
	//! Any group ids that no longer exist get automatically
	//! ignored.
	std::vector<font_picker_group_id> most_recently_used;

	//! The initial callback

	//! The callback gets invoked upon installation with
	//! \ref initial "initial" callback trigger indication, and the
	//! initial value of the font family group.

	functionptr<font_picker_callback_t> callback;

	//! Initial picker font.

	//! The default library font is used if not specified.

	std::optional<font> initial_font;

	//! Whether a font family+group must be specified

	//! If not specified, "Select a font family/size" options are
	//! included in the font family+size fields, and the font
	//! defaults to the system default font, if they're selected.

	bool selection_required=true;

	//! Choose fonts by foundry

	//! Only matters if the same font family is provided by multiple
	//! foundries. If set, the font family combo-box popup shows each
	//! foundry separately. The foundry's name follows the family name,
	//! in parenthesis.

	bool select_by_foundry=false;

	//! Whether the picked font must specify a foundry

	//! current_font() normally returns the font's foundry only if
	//! there are multiple foundries with the same font family.
	//!
	//! This makes current_font() (and the callback) always receive the
	//! font's foundry.

	bool foundry_required=false;

	//! Popup configuration
	element_popup_config popup_config;
};

LIBCXXW_NAMESPACE_END

#endif
