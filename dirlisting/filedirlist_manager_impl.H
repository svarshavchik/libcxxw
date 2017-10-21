/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedirlist_manager_impl_h
#define x_w_filedirlist_manager_impl_h

#include "dirlisting/filedirlist_manager.H"
#include "dirlisting/filedircontentsfwd.H"
#include "dirlisting/filedir_filefwd.H"
#include "x/w/textlistlayoutmanager.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/container.H"
#include "x/w/factory.H"
#include <x/mpobj.H>

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

		mpobj<std::function<filedirlist_selected_callback_t>
		      > current_callback;

		current_selected_callbackObj();
	};

	//! Currently selected callback.

	//! Initialized to a no-op. Must be updated by set_selected_callback().

	const ref<current_selected_callbackObj> current_selected;

	//! The list display element.

	const container filedir_list;

	//! Return filedir_list's layout manager.

	inline textlistlayoutmanager get_layoutmanager()
	{
		return filedir_list->get_layoutmanager();
	}

	//! Mutex-protected information about the directory being managed.

	struct info_t {

		//! The actual name of the directory.
		std::string directory;

		//! Internal execution thread's mcguffin.

		//! This mcguffin gets captured by the internal
		//! filedircontents object. chdir() discards
		//! current_filedircontents, and installs a destructor
		//! callback on this mcguffin that creates a new
		//! filedircontents object. This makes sure that its
		//! associated execution thread goes away. Not really
		//! necessary, to do this, but just keeps things tidy.

		ref<obj> callback_mcguffin;

		//! What we know about the contents of the directory.
		std::vector<filedirlist_entry> entries;

		//! Find a particular filename entry.
		std::vector<filedirlist_entry>::iterator
		find(const std::string &n);

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

	//! The container uses the list layout manager, acquiring a lock
	//! on the list before invoking any callback. For that reason we
	//! must make sure that we never attempt to acquire a list lock
	//! after info_t is already locked, otherwise a deadlock can occur.
	//!
	//! We do this by acquiring a list_lock ourselves, first, before
	//! locking the internal mutex that protects info_t.
	//!
	//! Define a mutex-protected wrapper for info_t that's derived from
	//! a bog-standard mpobj. However we also define a custom \c lock
	//! inner class.
	//!
	//! The inner \c lock class contains two members, \c lm the layout
	//! manager for the element, and \c ll which is the list lock, making
	//! sure that the list lock gets acquired first.

	struct protected_info_t : private mpobj<info_t, std::recursive_mutex> {

		typedef mpobj<info_t, std::recursive_mutex> superclass_t;

		using superclass_t::superclass_t;

		//! Base class for the internal lock.

		//! The base class gets constructed first, before the
		//! lock on the underlying mpobj.

		struct lock_listbase {

			//! The available list layout manager.
			textlistlayoutmanager lm;

			//! The list lock, which got here first.
			list_lock ll;

			//! Constructor
			lock_listbase(const textlistlayoutmanager &lm)
				: lm{lm}, ll{lm}
			{
			}
		};

		//! Custom lock that enforces the correct locking order.

		class lock : public lock_listbase, public superclass_t::lock {

		public:

			//! Constructor.
			lock(implObj &impl)
				: lock_listbase{impl.get_layoutmanager()},
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
		int access_mode);

	//! R_OK or W_OK
	const int access_mode;

	//! Constructor, phase 2.

	//! Installs a callback on the internal display element, that
	//! starts monitoring the directory when the display element becomes
	//! visible, and stops it when it's no longer visible.

	void constructor(const factory &f,
			 const std::string &,
			 int);

	//! Destructor
	~implObj();

	//! Install the callback that gets invoked when a directory item is selected.

	void set_selected_callback(const std::function<
				   filedirlist_selected_callback_t> &);

	//! Retrieve directory contents.

	filedirlist_entry at(size_t n);

	//! Change directories.
	void chdir(const std::string &directory);

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
		   const std::function<void (protected_info_t::lock &lock)>
		   &prepare);

	//! Stop monitoring the current directory.
	void stop(protected_info_t::lock &lock);

	//! Start a new execution thread.
	void start_new(protected_info_t::lock &lock);
};

LIBCXXW_NAMESPACE_END

#endif
