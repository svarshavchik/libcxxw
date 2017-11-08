/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedirlist_managerobj_h
#define x_w_filedirlist_managerobj_h

#include "dirlisting/filedirlist_managerfwd.H"
#include "x/w/factory.H"
#include <x/pcrefwd.H>
#include <string>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see filedirlist_manager

class LIBCXX_HIDDEN filedirlist_managerObj : virtual public obj {

 public:
	class implObj;

	//! Internal implementation details.
	const ref<implObj> impl;

	//! Constructor

	//! Constructor takes a factory wheere it creates the display element,
	//! the initial directory for browsing, and R_OK or W_OK, indicating
	//! whether this is to open an existing file or write/create a file.

	filedirlist_managerObj(const factory &f,
			       const std::string &initial_directory,
			       int access_mode);

	//! Destructor
	~filedirlist_managerObj();

	//! Install a new callback that gets invoked when a selection is made.

	void set_selected_callback(const std::function
				   <filedirlist_selected_callback_t> &);

	//! Return the filename at the given position.

	filedirlist_entry at(size_t n);

	//! Switch the display element to a different directory.

	void chdir(const std::string &directory);

	//! Change filename filters.
	void chfilter(const pcre &filter);

	//! Return the current directory
	std::string pwd() const;
};

LIBCXXW_NAMESPACE_END

#endif
