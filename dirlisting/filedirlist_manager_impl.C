/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dirlisting/filedirlist_manager_impl.H"
#include "dirlisting/filedircontents.H"
#include "dirlisting/filedir_file.H"
#include "x/w/impl/container_element.H"
#include "run_as.H"
#include "batch_queue.H"
#include "x/w/label.H"
#include "x/w/button_event.H"
#include "x/w/panefactory.H"
#include "x/w/focusable_container.H"
#include "x/w/file_dialog_config.H"
#include "x/w/file_dialog_appearance.H"
#include "x/w/pane_layout_appearance.H"
#include "x/w/pane_appearance.H"
#include "x/w/uielements.H"
#include "messages.H"
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
			   (THREAD_CALLBACK,
			    const filedirlist_selected_callback_arg_t &,
			    const callback_trigger_t &)
			   {
			   })
{
}

// Parameters to the delegated constructor.

struct filedirlist_managerObj::implObj::init_args {

	ref<current_selected_callbackObj> current_selected;

	focusable_container filedir_list;
	container dir_popup;
	container file_popup;
};

// Update the subdirectory list's right context popup menu to reflect
// the current directory open and the currently selected directory entry.

static void set_dir_popup_contents(const ref<filedirlist_managerObj::implObj
				   ::current_selected_callbackObj>
				   &current_selected,
				   const listlayoutmanager &lm,
				   const std::string &current_directory,
				   bool writable,
				   const std::optional<std::string> &subdir)
{

	// Set fullpath if there's a selected subdirectory.

	std::string fullpath;

	if (subdir && !current_directory.empty())
	{
		fullpath.reserve(current_directory.size() + 1 + subdir->size());


		fullpath += current_directory;

		if (fullpath != "/")
			fullpath += "/";

		fullpath += *subdir;
	}

	lm->replace_all_items
		({
		  [current_selected,
		   current_directory]
		  (ONLY IN_THREAD,
		   const auto &info)
		  {
			  if (!current_directory.empty())
				  current_selected->current_callback.get()
					  (IN_THREAD,
					   filedirlist_mkdir{current_directory},
					   info.trigger);
		  },
		  shortcut{"ALT-C"},
		  _("Create subdirectory"),
		  [current_selected,
		   fullpath]
		  (ONLY IN_THREAD,
		   const auto &info)
		  {
			  if (!fullpath.empty())
				  current_selected->current_callback.get()
					  (IN_THREAD,
					   filedirlist_rename{fullpath},
					   info.trigger);
		  },
		  shortcut{"ALT-R"},
		  _("Rename subdirectory"),
		  [current_selected,
		   fullpath]
		  (ONLY IN_THREAD,
		   const auto &info)
		  {
			  if (!fullpath.empty())
				  current_selected->current_callback.get()
					  (IN_THREAD,
					   filedirlist_rmdir{fullpath},
					   info.trigger);
		  },
		  shortcut{"DEL"},
		  _("Delete subdirectory")
		});

	// If the current directory is not writable, or there is no current
	// directory in the first place (popup is closed, the "Create
	// subdirectory" link is disabled.

	if (!writable || current_directory.empty())
		lm->enabled(0, false);

	// If the current directory is not writable, or no sudirectory is
	// picked/highlighted in the list, the rename/delete subdirectory links
	// are disabled.

	if (!writable || fullpath.empty())
	{
		lm->enabled(1, false);
		lm->enabled(2, false);
	}
}

// Update the file list's right context popup menu to reflect the currently
// selected file.

static void set_file_popup_contents(const ref<filedirlist_managerObj::implObj
				    ::current_selected_callbackObj>
				    &current_selected,
				    const listlayoutmanager &lm,
				    const std::string &current_directory,
				    bool writable,
				    const std::optional<std::string> &filename)
{
	std::string fullpath;

	if (writable && !current_directory.empty() && filename)
	{
		fullpath.reserve(current_directory.size() + 1 +
				 filename->size());

		fullpath += current_directory;

		if (fullpath != "/")
			fullpath += "/";

		fullpath += *filename;
	}

	// Even though the same shortcuts are used here, our logic
	// carefully makes sure that only one of the shortcuts is enabled
	// and the same time.

	lm->replace_all_items
		({
		  [current_selected,
		   fullpath]
		  (ONLY IN_THREAD,
		   const auto &info)
		  {
			  if (!fullpath.empty())
				  current_selected->current_callback.get()
					  (IN_THREAD,
					   filedirlist_rename{fullpath},
					   info.trigger);
		  },
		  shortcut{"ALT-R"},
		  _("Rename file"),
		  [current_selected,
		   fullpath]
		  (ONLY IN_THREAD,
		   const auto &info)
		  {
			  if (!fullpath.empty())
				  current_selected->current_callback.get()
					  (IN_THREAD,
					   filedirlist_unlink{fullpath},
					   info.trigger);
		  },
		  shortcut{"DEL"},
		  _("Delete file")
		});

	// If nothing is selected, the rename/delete links are disabled.

	if (fullpath.empty())
	{
		lm->enabled(0, false);
		lm->enabled(1, false);
	}
}

// Create an internal display element that shows the contents of a directory.

// This is a focusable container with a pane layout manager, and two panes:
// subdirectories and files.

static inline filedirlist_managerObj::implObj::init_args
create_init_args(const uielements &tmpl,
		 const std::string &initial_directory,
		 const file_dialog_config &config)
{
	container pane_container=tmpl.get_element("directory-contents-pane");
	container dc=tmpl.get_element("subdirectory-pane");
	container fc=tmpl.get_element("file-pane");

	auto current_selected=
		ref<filedirlist_managerObj::implObj
		    ::current_selected_callbackObj>::create();

	// nlm.appearance=config.appearance->dir_pane_list_appearance;

	listlayoutmanager dcllm=tmpl.get_layoutmanager("subdirectory-pane");

	dcllm->on_current_list_item_changed
		([current_selected]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 std::optional<size_t> selected;

			 if (info.selected)
				 selected=info.item_number;

			 current_selected->current_callback.get()
				 (IN_THREAD,
				  filedirlist_current_list_item{
					 filedirlist_entry_id::dir_section,
						 selected},
				  info.trigger);
		 });

	dcllm->selection_type
		 ([current_selected]
		  (ONLY IN_THREAD,
		   const listlayoutmanager &ignore,
		   size_t n,
		   const callback_trigger_t &trigger,
		   const busy &mcguffin)
		  {
			  current_selected->current_callback.get()
				  (IN_THREAD,
				   filedirlist_selected{
					  filedirlist_entry_id::dir_section, n,
						  mcguffin},
				   trigger);
		  });

	//pf->appearance=config.appearance->dir_pane_appearance;

	dc->show();

	// Create the directory list's right mouse button popup contents.

	auto dir_popup=dc->create_popup_menu
		([&]
		 (const listlayoutmanager &llm)
		 {
			 // Initial contents of the popup

			 set_dir_popup_contents(current_selected,
						llm, "",
						false,
						std::nullopt);
		 });

	// The directory context popup menu gets created here, in advance,
	// and install_contextpopup_callback() merely shows it.
	//
	// This results in the context popup's keyboard shortcuts being
	// active right away.

	dc->install_contextpopup_callback
		([dir_popup]
		 (ONLY IN_THREAD,
		  const auto &element,
		  const auto &trigger,
		  const auto &busy)
		 {
			 dir_popup->show_all();
		 });

	listlayoutmanager fcllm=tmpl.get_layoutmanager("file-pane");

	fcllm->selection_type
		([current_selected]
		 (ONLY IN_THREAD,
		  const listlayoutmanager &ignore,
		  size_t n,
		  const callback_trigger_t &trigger,
		  const busy &mcguffin)
		 {
			 current_selected->current_callback.get()
				 (IN_THREAD,
				  filedirlist_selected{
					 filedirlist_entry_id::file_section, n,
						 mcguffin},
				  trigger);
		 });

	fcllm->on_current_list_item_changed
		([current_selected]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 std::optional<size_t> selected;

			 if (info.selected)
				 selected=info.item_number;

			 current_selected->current_callback.get()
				 (IN_THREAD,
				  filedirlist_current_list_item{
					 filedirlist_entry_id::file_section,
						 selected},
				  info.trigger);
		 });

	// nlm.appearance=config.appearance->file_pane_list_appearance;

	// pf->appearance=config.appearance->file_pane_appearance;

	fc->show();

	// Create the file list's right mouse button popup contents.

	auto file_popup=fc->create_popup_menu
		([&]
		 (const listlayoutmanager &llm)
		 {
			 // Initial contents of the popup
			 set_file_popup_contents(current_selected,
						 llm, initial_directory, false,
						 std::nullopt);
		 });

	// The file context popup menu gets created here, in advance,
	// and install_contextpopup_callback() merely shows it.
	//
	// This results in the context popup's keyboard shortcuts being
	// active right away.

	fc->install_contextpopup_callback
		([file_popup]
		 (ONLY IN_THREAD,
		  const auto &element,
		  const auto &trigger,
		  const auto &busy)
		 {
			 file_popup->show_all();
		 });

	return {
		current_selected,
		pane_container,
		dir_popup,
		file_popup,
	};
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
::implObj(const uielements &tmpl,
	  const std::string &initial_directory,
	  const file_dialog_config &config)
	: implObj{create_init_args(tmpl, initial_directory, config),
		  initial_directory, config}
{
}

filedirlist_managerObj::implObj::implObj(const init_args &args,
					 const std::string &initial_directory,
					 const file_dialog_config &config)
	: current_selected{args.current_selected},
	  filedir_list{args.filedir_list},
	  dir_popup{args.dir_popup},
	  file_popup{args.file_popup},
	  info{info_t{access(initial_directory.c_str(), W_OK) == 0,
		      initial_directory, pcre::create("."),
		      ref<obj>::create()}},
	  type{config.type},
	  appearance{config.appearance}
{
}

filedirlist_managerObj::implObj::~implObj()=default;

void filedirlist_managerObj::implObj::constructor(const uielements &,
						  const std::string &,
						  const file_dialog_config &)
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
				lock->files.erase(iter);
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
				(to_string(ymdhms(st.st_mtime)
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
					enabled=lock->writable;
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
			appearance->filedir_filename_font,
			name_uc};
		text_param filedate{
			appearance->filedir_filedate_font,
			date_uc};

		text_param filesize{
			appearance->filedir_filesize_font,
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
		lock->writable=access(directory.c_str(), W_OK) == 0;
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
		      lock->writable=access(directory.c_str(), W_OK) == 0;
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

	// Update the context popup menus.
	update_directory_popup_status(lock, std::nullopt);
	update_file_popup_status(lock, std::nullopt);
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

	// Update the context popup menus.
	update_directory_popup_status(lock, std::nullopt);
	update_file_popup_status(lock, std::nullopt);
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

void filedirlist_managerObj::implObj
::update_popup_status(ONLY IN_THREAD,
		      int section,
		      const std::optional<size_t> &n)
{
	protected_info_t::lock lock{*this};

	if (section == filedirlist_entry_id::dir_section)
		update_directory_popup_status(lock, n);
	else
		update_file_popup_status(lock, n);
}

void filedirlist_managerObj::implObj
::update_directory_popup_status(protected_info_t::lock &lock,
				const std::optional<size_t> &n)
{
	std::string directory;
	bool writable=false;
	std::optional<std::string> subdir;

	if (lock->current_filedircontents)
		// Execution thread is running. Otherwise, the entire popup
		// must be closed.
	{
		directory=lock->directory;
		writable=lock->writable;

		if (n)
			subdir=lock->subdirectories.at(*n).name;
	}

	set_dir_popup_contents(current_selected,
			       dir_popup->get_layoutmanager(),
			       directory,
			       writable,
			       subdir);
}

void filedirlist_managerObj::implObj
::update_file_popup_status(protected_info_t::lock &lock,
			   const std::optional<size_t> &n)
{
	std::string directory;
	bool writable=false;
	std::optional<std::string> file;

	if (lock->current_filedircontents)
		// Execution thread is running. Otherwise, the entire popup
		// must be closed.
	{
		directory=lock->directory;
		writable=lock->writable;

		if (n)
			file=lock->files.at(*n).name;
	}

	set_file_popup_contents(current_selected,
				file_popup->get_layoutmanager(),
				directory,
				writable,
				file);
}

LIBCXXW_NAMESPACE_END
