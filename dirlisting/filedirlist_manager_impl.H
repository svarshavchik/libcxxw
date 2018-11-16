/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedirlist_manager_impl_h
#define x_w_filedirlist_manager_impl_h

#include "dirlisting/filedirlist_manager.H"
#include "dirlisting/filedircontentsfwd.H"
#include "dirlisting/filedir_filefwd.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/container.H"
#include "x/w/factory.H"
#include "x/w/file_dialog_config.H"
#include <x/mpobj.H>
#include <x/pcrefwd.H>
#include <x/functionalrefptr.H>

#include <string>
#include <unistd.h>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! \see filedirlist_manager

class LIBCXX_HIDDEN filedirlist_managerObj::implObj : virtual public obj {

 public:

	//! Separate container for the currently-installed callback.

	//! This gets captured by the custom selection_type callback for
	//! the internal directory contents list, and is also referenced by
	//! current_selected, in order to implement set_selected_callback().

	struct current_selected_callbackObj : virtual public obj {

		mpobj<functionref<filedirlist_selected_callback_t>
		      > current_callback;

		current_selected_callbackObj();
	};

	//! Currently selected callback.

	//! Initialized to a no-op. Must be updated by set_selected_callback().

	const ref<current_selected_callbackObj> current_selected;

 private:
	//! The list display element.

	const focusable_container filedir_list;

	//! Return filedir_list's layout manager.

	inline panelayoutmanager get_layoutmanager()
	{
		return filedir_list->get_layoutmanager();
	}
 public:

	//! Mutex-protected information about the directory being managed.

	struct info_t {

		//! The actual name of the directory.
		std::string directory;

		//! Current filename filter pattern

		pcre filename_filter;

		//! Internal execution thread's mcguffin.

		//! This mcguffin gets captured by the internal
		//! filedircontents object. chdir() discards
		//! current_filedircontents, and installs a destructor
		//! callback on this mcguffin that creates a new
		//! filedircontents object. This makes sure that its
		//! associated execution thread goes away. Not really
		//! necessary, to do this, but just keeps things tidy.

		ref<obj> callback_mcguffin;

		//! What we know about the contents of the directory, part 1.
		std::vector<filedirlist_entry> subdirectories;

		//! What we know about the contents of the directory, part 2.
		std::vector<filedirlist_entry> files;

		//! Find a particular filename entry.

		//! Searches subdirectories using lower_bound, and the
		//! appropriate sorting order. Returns a flag if lower_bound
		//! did not return end(), and the returned iterator's
		//! "name" matches the given string, and the return value
		//! from lower_bound().

		std::tuple<bool, std::vector<filedirlist_entry>::iterator>
		find_subdirectory(const std::string &n);

		//! Find a particular filename entry.

		//! Searches files using lower_bound, and the
		//! appropriate sorting order. Returns a flag if lower_bound
		//! did not return end(), and the returned iterator's
		//! "name" matches the given string, and the return value
		//! from lower_bound().

		std::tuple<bool, std::vector<filedirlist_entry>::iterator>
		find_file(const std::string &n);

		//! The current_filedircontents object.

		//! In addition to the logic implemented in chdir(), we
		//! only create the filedircontents object when the display
		//! element is visible, thus not loading the contents of the
		//! the directory until it's actually needed. When the display
		//! element gets hidden we also get rid of the object and
		//! stop the internal execution thread.

		ptr<obj> current_filedircontents;
	};

	//! Custom subclass of \c mpobj that stores a protected info_t

	//! The filedir_list uses the pane layout manager, which is derived
	//! from the grid layout manager, which locks its grid before,
	//! before invoking any callback. For that reason we
	//! must make sure that we always construct the same lock first,
	//! otherwise a deadlock can occur.
	//!
	//! We do this by constructing the pane layout manager, first, before
	//! locking the internal mutex that protects info_t.
	//!
	//! Define a mutex-protected wrapper for info_t that's derived from
	//! a bog-standard mpobj. However we also define a custom \c lock
	//! inner class.
	//!
	//! The inner \c lock class contains two members, \c pane_lm the layout
	//! manager, and \c pl which is the pane lock, making
	//! sure that the pane layout manager gets created first.

	struct protected_info_t : private mpobj<info_t, std::recursive_mutex> {

		typedef mpobj<info_t, std::recursive_mutex> superclass_t;

		using superclass_t::superclass_t;

		//! Base class for the internal lock.

		//! The base class gets constructed first, before the
		//! lock on the underlying mpobj.

		struct lock_panebase {

			//! The available pane layout manager.
			panelayoutmanager pane_lm;

			//! Constructor
			lock_panebase(const panelayoutmanager &pane_lm)
				: pane_lm{pane_lm}
			{
			}

			//! Return the list layout manager for the subdir pane
			listlayoutmanager subdirectory_lm();

			//! Return the list layout manager for the files pane
			listlayoutmanager files_lm();
		};

		//! Custom lock that enforces the correct locking order.

		class lock : public lock_panebase, public superclass_t::lock {

		public:

			//! Constructor.
			lock(implObj &impl)
				: lock_panebase{impl.get_layoutmanager()},
				  superclass_t::lock{impl.info}
			{
			}
		};

		//! For those situation where the list will not be accessed.

		//! direct_lock gets used when we know we will never try
		//! to acquire a list lock after locking the internal info_t.
		//!
		//! This means we can avoid the overhead of constructing a
		//! list lock ourselves.

		struct direct_lock : public superclass_t::lock {

			direct_lock(implObj &impl)
				: superclass_t::lock{impl.info}
			{
			}
		};
	};

	//! Mutex-protected current contents of the directory.

	protected_info_t info;

	//! Constructor, phase 1.

	implObj(const factory &f,
		const std::string &initial_directory,
		file_dialog_type type);

	//! Semantics of access permissions

	const file_dialog_type type;

	//! Whether the directory is writable
	const bool writable;

	//! Constructor, phase 2.

	//! Installs a callback on the internal display element, that
	//! starts monitoring the directory when the display element becomes
	//! visible, and stops it when it's no longer visible.

	void constructor(const factory &f,
			 const std::string &,
			 file_dialog_type type);

	//! Destructor
	~implObj();

	//! Install the callback that gets invoked when a directory item is selected.

	void set_selected_callback(const functionref<
				   filedirlist_selected_callback_t> &);

	//! Retrieve directory contents.

	filedirlist_entry at(const filedirlist_entry_id &id);

	//! Change directories.
	void chdir(const std::string &directory);

	//! Change filename filters.
	void chfilter(const pcre &filter);

	//! Return the current directory.
	std::string pwd();

 private:

	//! Invoked by the internal execution thread. Updates the contents of the tracked directory.

	void update(const const_filedir_file &files);

	//! Start monitoring the current directory.
	void start();

	//! Stop monitoring the current directory.
	void stop();

	//! Start a new execution thread, unless there's one already running.

	//! If one is already running, quietly exit without doing anything.
	//! Otherwise install a destructor callback on the previous thread's
	//! callback_mcguffin, create a new callback_mcguffin for the
	//! new execution thread, but don't start the new thread until the
	//! old mcguffin gets destroyed.
	//!
	//! Invokes start_new() to actually get things going, once everything
	//! is set, but invoke the prepare() callback before doing so.

	void start(protected_info_t::lock &lock,
		   const functionref<void (protected_info_t::lock &lock)>
		   &prepare);

	//! Stop monitoring the current directory.
	void stop(protected_info_t::lock &lock);

	//! Start a new execution thread.
	void start_new(protected_info_t::lock &lock);
};

LIBCXXW_NAMESPACE_END

#endif
