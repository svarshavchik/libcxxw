/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "file_dialog/file_dialog_impl.H"
#include "x/w/file_dialog_config.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

/////////////////////////////////////////////////////////////////////////////

file_dialog_config
::file_dialog_config(const functionref<void (THREAD_CALLBACK,
					     const file_dialog &,
					     const std::string &,
					     const busy &)> &ok_action,
		     const functionref<void (THREAD_CALLBACK,
					     const busy &)
		     > &cancel_action,
		     file_dialog_type type)
	: ok_action{ok_action},
	  cancel_action{cancel_action},
	  type{type},
	  initial_directory{"."},
	  filename_filters{ { _("All files"), "." }},
	  access_denied_message(type == file_dialog_type::existing_file
				? _("\"%1%\" does not exist, or you do not "
				    "have permission to access this file")
				: _("You do not have permissions to create "
				    "\"%1%\"")
				),
	  access_denied_title(_("Unable to open"))
{
}

file_dialog_config::~file_dialog_config()=default;

LIBCXXW_NAMESPACE_END
