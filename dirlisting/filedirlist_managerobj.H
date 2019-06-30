/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedirlist_managerobj_h
#define x_w_filedirlist_managerobj_h

#include "dirlisting/filedirlist_managerfwd.H"
#include "x/w/uielementsfwd.H"
#include "x/w/file_dialog_configfwd.H"
#include <x/pcrefwd.H>
#include <x/functionalrefptrfwd.H>
#include <x/obj.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see filedirlist_manager

class LIBCXX_HIDDEN filedirlist_managerObj : virtual public obj {

 public:
	class implObj;

	//! Internal implementation details.
	const ref<implObj> impl;

	//! Constructor

	//! Constructor takes the theme-generated elements containing the
	//! pane container, with the directory and file list containers,
	//! the initial directory for browsing, and R_OK or W_OK, indicating
	//! whether this is to open an existing file or write/create a file.

	filedirlist_managerObj(const uielements &tmpl,
			       const std::string &initial_directory,
			       const file_dialog_config &config);

	//! Destructor
	~filedirlist_managerObj();

	//! Install a new callback that gets invoked when a selection is made.

	void set_selected_callback(const functionref
				   <filedirlist_selected_callback_t> &);

	//! Return the filename at the given position.

	filedirlist_entry at(const filedirlist_entry_id &id);

	//! Switch the display element to a different directory.

	void chdir(const std::string &directory);

	//! Change filename filters.
	void chfilter(const pcre &filter);

	//! Return the current directory
	std::string pwd() const;
};

LIBCXXW_NAMESPACE_END

#endif
