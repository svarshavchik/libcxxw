/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_file_dialog_config_h
#define x_w_file_dialog_config_h

#include <x/w/file_dialog_configfwd.H>
#include <x/w/file_dialogfwd.H>
#include <x/w/dialogfwd.H>
#include <x/w/file_dialog_appearancefwd.H>
#include <x/w/text_param.H>
#include <x/w/busy.H>
#include <x/functionalrefptr.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Sets the behavior of a \ref file_dialog "file dialog" after selecting a file.

enum class file_dialog_type {

	//! The selected file must exist and must be readable.
	//!
	//! Otherwise an error message gets shown.

	existing_file,

	//! If the selected file exists, it must be writable; if the
	//! selected file does not exist its directory must be writable.
	//!
	//! Otherwise an error message gets shown.

	write_file,

	//! The selected file's directory must be writable, the
	//! permissions of any existing file are ignored.
	//!
	//! Otherwise an error message gets shown.
	create_file };

//! The configuration of a file dialog.

//! Parameter to the
//! \ref main_window "INSERT_LIBX_NAMESPACE::w::main_window"'s
//! create_fialog_dialog() method.

struct file_dialog_config {

	//! A particular file was selected by the dialog.

	//! The file dialog remains open. It's up to the callback
	//! to hide() it.

	functionref<void (THREAD_CALLBACK,
			  const file_dialog &,
			  const std::string &, const busy &)> ok_action;

	//! The dialog was canceled.

	//! The cancel button takes care of hiding the dialog, before
	//! executing this callback.
	ok_cancel_dialog_callback_t cancel_action;

	//! File dialog's semantics.
	file_dialog_type type=file_dialog_type::existing_file;

	//! Constructor
	file_dialog_config(const functionref<void (THREAD_CALLBACK,
						   const file_dialog &d,
						   const std::string &,
						   const busy &)> &ok_action,
			   const ok_cancel_dialog_callback_t &cancel_action,
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

	//! Apperance of this dialog
	const_file_dialog_appearance appearance;

	//! Destructor
	~file_dialog_config();

	//! Copy constructor
	file_dialog_config(const file_dialog_config &);

	//! Assignment operator.
	file_dialog_config &operator=(const file_dialog_config &);
};

LIBCXXW_NAMESPACE_END

#endif
