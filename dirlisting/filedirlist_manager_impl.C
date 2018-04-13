/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dirlisting/filedirlist_manager_impl.H"
#include "dirlisting/filedircontents.H"
#include "dirlisting/filedir_file.H"
#include "x/w/impl/container_element.H"
#include "run_as.H"
#include "batch_queue.H"
#include "x/w/text_param_literals.H"
#include "x/w/label.H"
#include "x/w/button_event.H"
#include "x/w/panefactory.H"
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
			   (THREAD_CALLBACK, const filedirlist_entry_id &,
			    const callback_trigger_t &,
			    const busy &)
			   {
			   })
{
}

// Create an internal display element that shows the contents of a directory.

// This is a focusable container with a pane layout manager, and two panes:
// subdirectories and files.

static inline auto create_filedir_list(const factory &f,
				       const std::string &initial_directory,
				       const auto &current_selected)
{
	new_panelayoutmanager nplm;

	auto pane_container=f->create_focusable_container([]
							  (const auto &ignore)
							  {
							  }, nplm);

	panelayoutmanager plm=pane_container->get_layoutmanager();

	auto pf=plm->append_panes();

	new_listlayoutmanager nlm{highlighted_list};

	nlm.columns=3;
	nlm.variable_height();
	nlm.vertical_scrollbar=scrollbar_visibility::automatic_reserved;

	// Give all space to the first column, with the filename.
	nlm.requested_col_widths.emplace(0, 100);

	// The rightmost column, file size, is right-aligned.
	nlm.col_alignments.emplace(2, halign::right);

	nlm.selection_type=[current_selected]
		(ONLY IN_THREAD,
		 const listlayoutmanager &ignore,
		 size_t n,
		 const callback_trigger_t &trigger,
		 const busy &mcguffin)
		{
			current_selected->current_callback.get()
			(IN_THREAD,
			filedirlist_entry_id{filedirlist_entry_id::dir_section,
					n}, trigger,
			 mcguffin);
		};

	pf->set_initial_size(30)
		.set_scrollbar_visibility(LIBCXX_NAMESPACE::w
					  ::scrollbar_visibility::never)
		.halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_focusable_container([]
					    (const auto &ignore)
					    {
					    },
					    nlm)->show();

	nlm.selection_type=[current_selected]
		(ONLY IN_THREAD,
		 const listlayoutmanager &ignore,
		 size_t n,
		 const callback_trigger_t &trigger,
		 const busy &mcguffin)
		{
			current_selected->current_callback.get()
			(IN_THREAD,
			filedirlist_entry_id{
				filedirlist_entry_id::file_section, n}, trigger,
			 mcguffin);
		};

	pf->set_initial_size(50)
		.set_scrollbar_visibility(LIBCXX_NAMESPACE::w
					  ::scrollbar_visibility::never)
		.halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_focusable_container([]
					    (const auto &ignore)
					    {
					    },
					    nlm)->show();
	return pane_container;
}

listlayoutmanager filedirlist_managerObj::implObj::protected_info_t
::lock_panebase::subdirectory_lm()
{
	focusable_container c=pane_lm->get(0);

	return c->get_layoutmanager();
}

listlayoutmanager filedirlist_managerObj::implObj::protected_info_t
::lock_panebase::files_lm()
{
	focusable_container c=pane_lm->get(1);

	return c->get_layoutmanager();
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
		 (THREAD_CALLBACK, const auto &state, const auto &busy)
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
::set_selected_callback(const functionref<filedirlist_selected_callback_t> &c)
{
	current_selected->current_callback=c;
}

// Sort the contents of the directory using case-insensitive search, assuming
// UTF-8 encoding.

static bool compare(const filedirlist_entry &a,
		    const std::tuple<std::string, std::string>
		    &str_and_lowercase_str)
{
	// Case-insensitive comparison.
	auto as=unicode::tolower(a.name, unicode::utf_8);

	if (as == std::get<1>(str_and_lowercase_str))
		return a.name < std::get<0>(str_and_lowercase_str);

	return as < std::get<1>(str_and_lowercase_str);
}

std::tuple<bool, std::vector<filedirlist_entry>::iterator>
filedirlist_managerObj::implObj::info_t::find_subdirectory(const std::string &n)
{
	auto lowercase_str=unicode::tolower(n, unicode::utf_8);

	auto iter=std::lower_bound(subdirectories.begin(),
				   subdirectories.end(),
				   std::tuple{n, lowercase_str}, compare);

	return {iter != subdirectories.end() && iter->name == n, iter};
}

std::tuple<bool, std::vector<filedirlist_entry>::iterator>
filedirlist_managerObj::implObj::info_t::find_file(const std::string &n)
{
	auto lowercase_str=unicode::tolower(n, unicode::utf_8);

	auto iter=std::lower_bound(files.begin(),
				   files.end(),
				   std::tuple{n, lowercase_str}, compare);

	return {iter != files.end() && iter->name == n, iter};
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
			// A directory entry was removed. We check first
			// subdirectories, then files.

			auto [found, iter]=lock->find_file(f.name);

			if (found)
			{
				lock.files_lm()
					->remove_item(iter-lock->files.begin());
				lock->subdirectories.erase(iter);
			}
			else
			{
				auto [found, iter]=
					lock->find_subdirectory(f.name);

				if (found)
				{
					lock.subdirectory_lm()->remove_item
						(iter-
						 lock->subdirectories.begin());
					lock->subdirectories.erase(iter);
				}
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

		auto is_directory=S_ISDIR(st.st_mode);

		// Prepared to deal with a hit from inotify for something
		// we already know about.

		auto &which_one=
			is_directory ? lock->subdirectories:lock->files;

		auto [found, p]=is_directory
			? lock->find_subdirectory(f.name)
			: lock->find_file(f.name);

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

		size_t pos=p-which_one.begin();

		if (!found)
			which_one.insert(p, {f.name, st});
		else // We already have this one, must be an update from inotify
			*p={f.name, st};

		text_param filename{
			"filedir_filename"_theme_font,
				name_uc};
		text_param filedate{ "filedir_filedate"_theme_font,
				date_uc};

		text_param filesize{ "filedir_filesize"_theme_font,
				size_uc};

		auto lm=is_directory ? lock.subdirectory_lm():lock.files_lm();

		if (found)
		{
			lm->replace_items(pos, {filename, filedate, filesize});
		}
		else
		{
			lm->insert_items(pos, {filename, filedate, filesize});
		}
		lm->enabled(pos, enabled);
	}

	// Hold onto the files object until the connection thread has nothing
	// to do, in order to delay processing of the next chunk of files until
	// everything gets redrawn. We are throttling the monitoring
	// execution thread, this way. The execution thread will not send
	// another update until the files object gets destroyed.

	filedir_list->elementObj::impl->THREAD->get_batch_queue()->run_as
		([files]
		 (ONLY IN_THREAD)
		 {
			 IN_THREAD->idle_callbacks(IN_THREAD)->push_back
				 ([files]
				  (ONLY IN_THREAD)
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
					    const functionref
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

	// Use replace_all_items() as means of wiping the lists.
	lock.subdirectory_lm()->replace_all_items({});
	lock.files_lm()->replace_all_items({});

	lock->subdirectories.clear();
	lock->files.clear();
}

void filedirlist_managerObj::implObj::start_new(protected_info_t::lock &lock)
{
	lock.subdirectory_lm()->replace_all_items({});
	lock.files_lm()->replace_all_items({});

	lock->subdirectories.clear();
	lock->files.clear();

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

filedirlist_entry filedirlist_managerObj::implObj
::at(const filedirlist_entry_id &id)
{
	protected_info_t::direct_lock lock{*this};

	auto d=lock->directory;

	if (d != "/")
		d += "/";

	// Return a full filename
	auto e=id.section == id.dir_section ? lock->subdirectories.at(id.n)
		: lock->files.at(id.n);

	e.name=d+e.name;
	return e;
}

LIBCXXW_NAMESPACE_END
