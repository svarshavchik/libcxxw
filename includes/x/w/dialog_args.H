/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialog_args_h
#define x_w_dialog_args_h

#include <x/w/dialogfwd.H>
#include <x/w/screen_positionsfwd.H>
#include <x/w/main_window_appearance.H>
#include <x/w/new_layoutmanager.H>
#include <string>
#include <string_view>
#include <optional>
#include <functional>
#include <variant>

LIBCXXW_NAMESPACE_START

//! Parameters for a new dialog.

//! Passed to \ref main_window "main window"'s methods that create new
//! dialogs, specifies the new dialog's configuration and parameters.

struct standard_dialog_args {

	//! Unique dialog ID.

	//! Cannot be set to a temporary object, this is a \c std::string_view.
	std::string_view dialog_id;

	//! Whether the new dialog is a modal dialog.
	bool modal=false;

	//! Window name

	//! Used when saving/loading screen_positions.
	std::string name;

	//! Default dialog position
	std::variant<dialog_position, rectangle> position=
		dialog_position::default_position;

	//! Set the urgent flag on the dialog.

	//! Some window managers draw attention, in some way, to urgent
	//! dialogs. This sets the urgent flag on the new dialog.
	bool urgent=false;

	//! Whether the new dialog grabs input focus when it opens.
	bool grab_input_focus=true;

	//! Default background color
	color_arg dialog_background="dialog_background";

	//! Top level appearance

	const_main_window_appearance appearance=
		main_window_appearance::base::theme();

	//! Default dialog position.
	void restore(dialog_position) LIBCXX_PUBLIC;

	//! Default dialog position.
	void restore(dialog_position, const std::string_view &) LIBCXX_PUBLIC;

	//! Restore the dialog's previous screen position.
	void restore(const const_screen_positions &,
		     const std::string_view &) LIBCXX_PUBLIC;
};

//! An additional parameter for create_dialog() and create_custom_dialog().

//! Passed to \ref main_window "main window"'s create_dialog() or
//! create_custom_dialog(), setting the new dialog's parameters.
//!
//! A subclass of \ref standard_dialog_args "INSERT_LIBX_NAMESPACE::w::standard_dialog_args"
//! that adds an optional \c dialog_layout, the layout manager for the new
//! dialog.

struct create_dialog_args : public standard_dialog_args {

	//! New dialog's layout manager.

	//! \code
	//! INSERT_LIBX_NAMESPACE::w::create_dialog_args args{"example@example.com"};
	//!
	//! INSERT_LIBX_NAMESPACE::w::new_toolboxlayoutmanager ntlm;
	//!
	//! args.dialog_layout=ntlm;
	//! \endcode
	//!
	//! The new layout manager object cannot be a temporary object, and
	//! the object must exist until create_dialog() or
	//! create_custom_dialog() returns.

	std::optional<std::reference_wrapper<const new_layoutmanager>
		      > dialog_layout;
};

LIBCXXW_NAMESPACE_END

#endif
