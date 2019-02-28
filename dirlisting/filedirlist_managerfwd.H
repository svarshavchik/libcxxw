/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedirlist_managerfwd_h
#define x_w_filedirlist_managerfwd_h

#include "x/w/namespace.H"
#include "x/w/busyfwd.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/connection_threadfwd.H"
#include <x/ptrfwd.H>
#include <string>
#include <optional>
#include <sys/stat.h>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN filedirlist_managerObj;

/*! Directory contents manager display element.

Implements a display element for a
\ref filedircontents "directory contents monitor", that support the following
functionality:

- Keeps track of the contents of the directory being monitored, internally.

- Implements chdir() that changes the directory whose contents get monitored.

- Provides an at() method that a callback can invoke, that returns the
\ref filedirlist_entry "entry" that gives the full pathname to the selected
directory entry.

This is not a discrete display element, but an opaque object. The constructor
takes a \ref factory "factory" as a parameter, and uses it to construct a
single display element, that this object keeps an internal reference to.

*/

typedef ref<filedirlist_managerObj> filedirlist_manager;

//! A constant \ref filedirlist_manager "WHATITIS".

//! \see filedirlist_manager

typedef const_ref<filedirlist_managerObj> const_filedirlist_manager;

//! A nullable pointer reference to a \ref filedirlist_manager "WHATITIS".

//! \see filedirlist_manager

typedef ptr<filedirlist_managerObj> filedirlist_managerptr;

//! A nullable pointer reference to a const \ref filedirlist_manager "WHATITIS".

//! \see filedirlist_manager

typedef const_ptr<filedirlist_managerObj> const_filedirlist_managerptr;

struct LIBCXX_HIDDEN filedirlist_entry_id;
struct LIBCXX_HIDDEN filedirlist_selected;
struct LIBCXX_HIDDEN filedirlist_focus;
struct LIBCXX_HIDDEN filedirlist_selection;
struct LIBCXX_HIDDEN popup_visibility;
struct LIBCXX_HIDDEN filedirlist_mkdir;
struct LIBCXX_HIDDEN filedirlist_rmdir;
struct LIBCXX_HIDDEN filedirlist_rename;
struct LIBCXX_HIDDEN filedirlist_unlink;

//! Specifies an entry in the directory or the file list section.

struct filedirlist_entry_id {

	static constexpr int dir_section=0;

	static constexpr int file_section=1;

	int section;

	size_t n;
};

//! The parameter passed to a filedirlist_selected_callback_t

//! This parameter specifies that an entry was clicked on.

struct filedirlist_selected : public filedirlist_entry_id {

	const busy &mcguffin;
};

//! The parameter passed to a filedirlist_selected_callback_t

//! This parameter reports whether the directory or the file section gained
//! or lost focus.

struct filedirlist_focus {

	int section; //!< See filedirlits_entry_id

	bool in_focus;
};

//! The parameter passed to a filedirlist_selected_callback_t

//! This parameter reports whether which entry in the directory or the file
//! section is currently highlighted/under cursor.

struct filedirlist_current_list_item {

	int section; //!< See filedirlist_entry_id

	const std::optional<size_t> &n;
};

//! The parameter passed to a filedirlist_selected_callback_t

//! This parameter reports when one of the popups becomes visible, or is
//! hidden.

struct filedirlist_contextpopup {

	int section; //!< See filedirlist_entry_id

	bool visible; //!< Whether it's visible
};

//! The parameter passed to a filedirlist_selected_callback_t

//! This parameter specifies that "Create subdirectory" was selected.

struct filedirlist_mkdir { std::string filename; };

//! The parameter passed to a filedirlist_selected_callback_t

//! This parameter specifies that "Remove subdirectory" was selected.

struct filedirlist_rmdir { std::string filename; };

//! The parameter passed to a filedirlist_selected_callback_t

//! This parameter specifies that "Rename" was selected.

struct filedirlist_rename { std::string filename; };

//! The parameter passed to a filedirlist_selected_callback_t

//! This parameter specifies that "Remove subdirectory" was selected.

struct filedirlist_unlink { std::string filename; };

typedef std::variant<filedirlist_selected,
		     filedirlist_focus,
		     filedirlist_current_list_item,
		     filedirlist_contextpopup,
		     filedirlist_mkdir,
		     filedirlist_rmdir,
		     filedirlist_rename,
		     filedirlist_unlink
		     > filedirlist_selected_callback_arg_t;

//! Callback that gets invoked when a file directory entry is selected.

//! The file directory list is locked at this point, and the id of the
//! clicked on entry is passed here.
typedef void filedirlist_selected_callback_t
(THREAD_CALLBACK,
 const filedirlist_selected_callback_arg_t &,
 const callback_trigger_t &);

//! An entry in the directory being monitored.

struct LIBCXX_HIDDEN filedirlist_entry {

	//! The filename.

	//! This is just the filename component, when a vector of these things
	//! is stored internally. When this gets returned by
	//! \ref filedirlist_manager "at()", at() puts an absolute pathname
	//! here.

	std::string name;

	//! Directory entry's attributes.

	struct ::stat st;

	//! Whether stat() is available.

	//! If the stat() on the directory entry failed, the \c st structure
	//! is zero-initialized.

	constexpr bool stat_succeeded()
	{
		return st.st_nlink > 0;
	}
};


LIBCXXW_NAMESPACE_END

#endif
