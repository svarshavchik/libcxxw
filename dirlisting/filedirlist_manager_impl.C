/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dirlisting/filedirlist_manager_impl.H"
#include "dirlisting/filedircontents.H"
#include "dirlisting/filedir_file.H"
#include "container_element.H"
#include "run_as.H"
#include "batch_queue.H"
#include "x/w/text_param_literals.H"
#include "x/w/label.H"
#include "x/w/button_event.H"
#include <x/weakcapture.H>
#include <x/fileattr.H>
#include <x/ymdhms.H>
#include <x/fmtsize.H>
#include <x/pcre.H>
#include <sys/stat.h>

LIBCXXW_NAMESPACE_START

filedirlist_managerObj::implObj::current_selected_callbackObj
::current_selected_callbackObj()
	: current_callback([]
			   (size_t,
			    const callback_trigger_t &,
			    const busy &)
			   {
			   })
{
}

//! Create an internal display element that shows the contents of a directory.

static inline auto create_filedir_list(const factory &f,
				       const std::string &initial_directory,
				       const auto &current_selected)
{
	new_listlayoutmanager nlm{highlighted_list};

	nlm.columns=3;
	nlm.rows=10;

	// Give all space to the first column, with the filename.
	nlm.requested_col_widths.emplace(0, 100);

	// The rightmost column, file size, is right-aligned.
	nlm.col_alignments.emplace(2, halign::right);

	nlm.selection_type=[current_selected]
		(const listlayoutmanager &ignore,
		 size_t n,
		 const callback_trigger_t &trigger,
		 const busy &mcguffin)
		{
			current_selected->current_callback.get()
			(n, trigger, mcguffin);
		};
	return f->create_focusable_container([]
					     (const auto &ignore)
					     {
					     },
					     nlm);
}

filedirlist_managerObj::implObj
::implObj(const factory &f,
	  const std::string &initial_directory,
	  file_dialog_type type)
	: current_selected(ref<current_selected_callbackObj>::create()),
	  filedir_list(create_filedir_list(f, initial_directory,
					   current_selected)),
	  info(info_t{initial_directory, pcre::create("."),
				  ref<obj>::create()}),
	  type(type),
	  writable(access(initial_directory.c_str(), W_OK) == 0)
{
}

filedirlist_managerObj::implObj::~implObj()=default;

void filedirlist_managerObj::implObj::constructor(const factory &,
						  const std::string &,
						  file_dialog_type)
{
	// When the list becomes visible, start the thread to populate
	// its contents. When the list is no longer visible, stop the thread.

	filedir_list->on_state_update
		([me=make_weak_capture(ref(this))]
		 (const auto &state, const auto &busy)
		 {
			 auto got=me.get();

			 if (got)
			 {
				 auto &[me]=*got;

				 if (state.shown)
					 me->start();
				 else
					 me->stop();
			 }
		 });
}

void filedirlist_managerObj::implObj
::set_selected_callback(const std::function<filedirlist_selected_callback_t> &c)
{
	current_selected->current_callback=c;
}

// Sort the contents of the directory. Any subdirectories come first, then
// the files.

static bool compare(const filedirlist_entry &a,
		    const filedirlist_entry &b)
{
	int ad=S_ISDIR(a.st.st_mode) ? 0:1;
	int bd=S_ISDIR(b.st.st_mode) ? 0:1;

	if (ad != bd)
		return ad < bd;

	// Case-insensitive comparison.
	auto as=unicode::tolower(a.name, unicode::utf_8);
	auto bs=unicode::tolower(b.name, unicode::utf_8);

	return as < bs;
}

// Because we sort directories first, when searching for an existing entry
// by name we have to try it both ways.

std::vector<filedirlist_entry>::iterator
filedirlist_managerObj::implObj::info_t::find(const std::string &n)
{
	filedirlist_entry dummy{ n, {} };

	auto b=entries.begin(),
		e=entries.end(),
		p=std::lower_bound(b, e, dummy, compare);

	if (p == e || p->name != n)
	{
		dummy.st.st_mode=S_IFDIR;
		p=std::lower_bound(b, e, dummy, compare);
	}

	return p;
}

void filedirlist_managerObj::implObj::update(const const_filedir_file &files)
{
	protected_info_t::lock lock{*this};

	if (!lock->current_filedircontents)
		return; // Winding things down.

	auto prefix=lock->directory;

	if (prefix != "/")
		prefix += "/";

	// Note that due to some race conditions, we can get a "removed" file
	// or an added file again. This is because the monitoring thread
	// reads the directory from start to finish, then starts monitoring
	// the inotify events. The directory read+inotity monitoring is not
	// atomic. Hence, we have to do all this work...

	for (const auto &f:files->files)
	{
		if (f.removed)
		{
			auto p=lock->find(f.name);

			bool found=p != lock->entries.end() &&
				p->name == f.name;

			if (found)
			{
				lock.lm->remove_item(p-lock->entries.begin());
				lock->entries.erase(p);
			}
			continue;
		}

		struct ::stat st{};
		bool stat_succeeded=false;

		std::string fullname=prefix + f.name;

		auto fullname_st=fileattr::create(fullname, false)->try_stat();
		if (fullname_st)
		{
			st=*fullname_st;
			stat_succeeded=true;

			if (!S_ISDIR(st.st_mode) &&
			    !lock->filename_filter->match(f.name))
				continue;
		}

		bool enabled=true;

		filedirlist_entry e{f.name, st};

		auto p=std::lower_bound(lock->entries.begin(),
					lock->entries.end(), e, compare);

		// Prepared to deal with a hit from inotify for something
		// we already know about.

		bool found=p != lock->entries.end() &&
			p->name == f.name;

		// Prepare the text that represents this file.

		std::u32string name_uc;
		std::u32string date_uc=U"????????";
		std::u32string size_uc=U"????????";

		unicode::iconvert::convert
			(f.name, unicode::utf_8, name_uc);

		if (stat_succeeded)
		{
			unicode::iconvert::convert
				(tostring(ymdhms(st.st_mtime)
					  .short_format()),
				 unicode::utf_8,
				 date_uc);

			size_uc.clear();

			if (S_ISDIR(st.st_mode))
			{
				name_uc.push_back('/');
				if (access(fullname.c_str(), (R_OK|X_OK)))
					enabled=false;
			}
			else
			{
				unicode::iconvert::convert
					(fmtsize(st.st_size),
					 unicode::utf_8,
					 size_uc);

				switch (type) {
				case file_dialog_type::existing_file:
					if (access(fullname.c_str(), R_OK))
						enabled=false;
					break;
				case file_dialog_type::write_file:
					if (access(fullname.c_str(), W_OK))
						enabled=false;
					break;
				case file_dialog_type::create_file:
					enabled=writable;
					break;
				}
			}
		}

		if (date_uc.size() < 14)
			date_uc.insert(0, 14-date_uc.size(), ' ');
		if (size_uc.size() < 8)
			size_uc.insert(0, 8-size_uc.size(), ' ');

		size_t pos=p-lock->entries.begin();

		if (!found)
			lock->entries.insert(p, {f.name, st});
		else
			*p={f.name, st};

		text_param filename{
			"filedir_filename"_theme_font,
				name_uc};
		text_param filedate{ "filedir_filedate"_theme_font,
				date_uc};

		text_param filesize{ "filedir_filesize"_theme_font,
				size_uc};

		if (found)
		{
			lock.lm->replace_items(pos, {filename, filedate,
						filesize});
		}
		else
		{
			lock.lm->insert_items(pos, {filename, filedate,
						filesize});
		}
		lock.lm->enabled(pos, enabled);
	}

	// Hold onto the files object until the connection thread has nothing
	// to do, in order to delay processing of the next chunk of files until
	// everything gets redrawn. We are throttling the monitoring
	// execution thread, this way. The execution thread will not send
	// another update until the files object gets destroyed.

	filedir_list->elementObj::impl->THREAD->get_batch_queue()->run_as
		([files]
		 (IN_THREAD_ONLY)
		 {
			 IN_THREAD->idle_callbacks(IN_THREAD)->push_back
				 ([files]
				  (IN_THREAD_ONLY)
				  {
				  });
		 });
}

void filedirlist_managerObj::implObj::chdir(const std::string &directory)
{
	protected_info_t::lock lock{*this};

	if (!lock->current_filedircontents)
		// The element is not visible, just update the directory.
	{
		lock->directory=directory;
		return;
	}

	// Stop the current execution thread, make arrangements to start
	// another one.

	stop(lock);

	start(lock,
	      [directory]
	      (auto &lock)
	      {
		      lock->directory=directory;
	      });

}

void filedirlist_managerObj::implObj::chfilter(const pcre &filter)
{
	protected_info_t::lock lock{*this};

	if (!lock->current_filedircontents)
		// The element is not visible, just update the directory.
	{
		lock->filename_filter=filter;
		return;
	}

	// Stop the current execution thread, make arrangements to start
	// another one.

	stop(lock);

	start(lock,
	      [filter]
	      (auto &lock)
	      {
		      lock->filename_filter=filter;
	      });

}

std::string filedirlist_managerObj::implObj::pwd()
{
	protected_info_t::direct_lock lock{*this};

	return lock->directory;
}


void filedirlist_managerObj::implObj::start()
{
	protected_info_t::lock lock{*this};

	start(lock,
	      []
	      (auto &lock)
	      {
	      });
}

void filedirlist_managerObj::implObj::start(protected_info_t::lock &lock,
					    const std::function
					    <void
					    (protected_info_t::lock &lock)>
					    &prepare)
{
	if (lock->current_filedircontents)
		return;

	// callback_mcguffin is the previous thread's (if any) mcguffin.
	auto current_mcguffin=lock->callback_mcguffin;

	// Create a new mcguffin, and install it. Attach a destructor callback
	// to the previous mcguffin. When the previous mcguffin gets
	// truly destroyed...
	auto new_mcguffin=ref<obj>::create();

	lock->callback_mcguffin=new_mcguffin;

	current_mcguffin->ondestroy
		([new_mcguffin, me=make_weak_capture(ref(this)),
		  prepare]
		 {
			 auto got=me.get();

			 if (got)
			 {
				 auto &[me]=*got;

				 // Let's make sure that the new
				 // callback_mcguffin that was originally
				 // create is still there. If not,
				 // start() must've been called again.
				 // That's ok, just go away quietly.

				 protected_info_t::lock lock{*me};

				 if (lock->callback_mcguffin !=
				     new_mcguffin)
					 return;

				 // If so, we're all set.

				 prepare(lock);
				 me->start_new(lock);
			 }
		 });
}

void filedirlist_managerObj::implObj::stop()
{
	protected_info_t::lock lock{*this};

	stop(lock);
}


void filedirlist_managerObj::implObj::stop(protected_info_t::lock &lock)
{
	lock->current_filedircontents=nullptr;
	lock.lm->replace_all_items({});
	lock->entries.clear();
}

void filedirlist_managerObj::implObj::start_new(protected_info_t::lock &lock)
{
	lock.lm->replace_all_items({});

	lock->entries.clear();

	lock->current_filedircontents=
		filedircontents::create
		(lock->directory,
		 [mcguffin= lock->callback_mcguffin,
		  me=make_weak_capture(ref(this))]
		 (const const_filedir_file &f)
		 {
			 auto got=me.get();

			 if (got)
			 {
				 auto &[me]=*got;

				 me->update(f);
			 }
		 });
}

filedirlist_entry filedirlist_managerObj::implObj::at(size_t n)
{
	protected_info_t::direct_lock lock{*this};

	auto d=lock->directory;

	if (d != "/")
		d += "/";

	// Return a full filename
	auto e=lock->entries.at(n);
	e.name=d+e.name;
	return e;
}

LIBCXXW_NAMESPACE_END
