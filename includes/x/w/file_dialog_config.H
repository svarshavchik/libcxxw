/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_file_dialog_config_h
#define x_w_file_dialog_config_h

#include <x/w/file_dialog_configfwd.H>
#include <x/w/file_dialogfwd.H>
#include <x/w/text_param.H>
#include <x/w/busy.H>
#include <functional>
#include <tuple>

LIBCXXW_NAMESPACE_START

enum class file_dialog_type { new_file, existing_file };

//! The configuration of a file dialog.

struct file_dialog_config {

	//! A particular file was selected by the dialog.

	std::function<void (const file_dialog &,
			    const std::string &, const busy &)> ok_action;

	//! The dialog was canceled.
	std::function<void (const busy &)> cancel_action;

	//! File dialog's semantics.
	file_dialog_type type=file_dialog_type::existing_file;

	//! Constructor
	file_dialog_config(const std::function<void (const file_dialog &d,
						     const std::string &,
						     const busy &)> &ok_action,
			   const std::function<void (const busy &)
			   > &cancel_action,
			   file_dialog_type type=file_dialog_type::existing_file
			   );

	//! Initial directory displayed in the file dialog.

	std::string initial_directory;

	//! Filename filters.

	//! A vector of tuples: the label for the filename filter, and the
	//! filename filter itself. The filename filter is a regular
	//! expression.

	std::vector<std::tuple<text_param, std::string>> filename_filters;

	//! Initial filename filter index.

	size_t initial_filename_filter=0;

	//! Message shown when access to the file is denied.
	std::string access_denied_message;

	//! Title of the access denied dialog.
	std::string access_denied_title;

	//! Destructor
	~file_dialog_config();
};

LIBCXXW_NAMESPACE_END

#endif
