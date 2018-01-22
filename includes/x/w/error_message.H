/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_error_message_H
#define x_w_error_message_H

#include <x/w/error_messagefwd.H>
#include <string>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Optional parameters for a \ref main_window "main window"'s error_message().

struct error_message_config {

	//! Callback when the dialog gets closed.

	//! error_message() returns immediately, and, by default, the modal
	//! dialog blocks keyboard and pointer events until the dialog
	//! gets closed. If set, acknowledged_callback() gets invoked when
	//! the dialog gets closed.

	std::function<void ()> acknowledged_callback;

	//! error_message()'s dialog's title.

	std::variant<std::string, std::u32string> title=default_title();

	//! Default value for the title.

	static std::string default_title() noexcept;

	//! Ok button's label.
	text_param ok_label=default_ok_label();

	//! Default value for ok_label.
	static text_param default_ok_label() noexcept;

	//! Whether the dialog is modal
	bool modal=true;

	//! Destructor
	~error_message_config();
};

LIBCXXW_NAMESPACE_END

#endif
