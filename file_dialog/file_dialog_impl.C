/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "file_dialog/file_dialog_impl.H"
#include "dirlisting/filedirlist_manager_impl.H"
#include "main_window.H"
#include "dialog.H"
#include "x/w/uielements.H"
#include "x/w/impl/element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/child_element.H"
#include "messages.H"
#include "x/w/input_field.H"
#include "x/w/input_field_lock.H"
#include "x/w/input_dialog.H"
#include "x/w/file_dialog_config.H"
#include "x/w/file_dialog_appearance.H"
#include "x/w/label.H"
#include "x/w/focusable_label.H"
#include "x/w/text_hotspot.H"
#include "x/w/button.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "x/w/gridfactory.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/impl/layoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/callback_trigger.H"
#include "x/w/dialog.H"
#include "x/w/input_dialog.H"
#include "drag_destination_element.H"
#include "connection_info.H"
#include "selection/current_selection_handler.H"
#include "connection_thread.H"
#include "messages.H"
#include "generic_window_handler.H"
#include "busy.H"
#include <x/fd.H>
#include <x/visitor.H>
#include <x/fileattr.H>
#include <x/pcre.H>
#include <x/weakcapture.H>
#include <x/strtok.H>
#include <x/uriimpl.H>
#include <x/visitor.H>
#include <courier-unicode.h>
#include <algorithm>
#include <vector>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

LIBCXXW_NAMESPACE_START

// Capture the current state of the popups, before something might change
// above either one of them. Then, update() checks if they changed, and
// invokes update_popup_status() if needed.

class file_dialogObj::implObj::update_popup_menu_status {

public:

	implObj &me;

	const std::optional<size_t> prev_dir_status;
	const std::optional<size_t> prev_file_status;

	update_popup_menu_status(ONLY IN_THREAD, implObj &me)
		: me {me},
		  prev_dir_status{me.dir_status(IN_THREAD).item()},
		  prev_file_status{me.file_status(IN_THREAD).item()}
	{
	}


	void update(ONLY IN_THREAD)
	{
		auto new_dir_status=me.dir_status(IN_THREAD).item();
		auto new_file_status=me.file_status(IN_THREAD).item();

		if (new_dir_status != prev_dir_status)
			me.directory_contents_list->impl->update_popup_status
				(IN_THREAD,
				 filedirlist_entry_id::dir_section,
				 new_dir_status);

		if (new_file_status != prev_file_status)
			me.directory_contents_list->impl->update_popup_status
				 (IN_THREAD,
				  filedirlist_entry_id::file_section,
				  new_file_status);
	}
};

file_dialogObj::implObj
::implObj(const focusable_label &directory_field,
	  const input_field &filename_field,
	  const focusable_container &filter_field,
	  const filedirlist_manager &directory_contents_list,
	  const button &ok_button,
	  const button &cancel_button,
	  const functionref<void (THREAD_CALLBACK,
				  const file_dialog &,
				  const std::string &, const busy &)
	  > &ok_action,
	  file_dialog_type type,
	  const std::string &access_denied_message,
	  const std::string &access_denied_title,
	  const const_file_dialog_appearance &appearance)
	: directory_field{directory_field},
	  filename_field{filename_field},
	  filter_field{filter_field},
	  directory_contents_list{directory_contents_list},
	  ok_button{ok_button},
	  cancel_button{cancel_button},
	  ok_action{ok_action},
	  type{type},
	  access_denied_message{access_denied_message},
	  access_denied_title{access_denied_title},
	  appearance{appearance}
{
}

file_dialogObj::implObj::~implObj()=default;

void file_dialogObj::implObj
::file_or_dir_selection(ONLY IN_THREAD,
			int section,
			const std::optional<size_t> &n)
{
	update_popup_menu_status update{IN_THREAD, *this};

	auto &status=section == filedirlist_entry_id::dir_section
		? dir_status(IN_THREAD) : file_status(IN_THREAD);

	dir_status(IN_THREAD).new_focus(false);
	file_status(IN_THREAD).new_focus(false);

	if (n)
	{
		status.new_focus(true);
		status.new_list_item(*n);
	}
	update.update(IN_THREAD);
}

void file_dialogObj::implObj::clicked(ONLY IN_THREAD,
				      const filedirlist_entry_id &id,
				      const callback_trigger_t &trigger,
				      const busy &mcguffin)
{
	file_or_dir_selection(IN_THREAD, id.section, id.n);

	auto e=directory_contents_list->at(id);

	bool autoselect_file=true;

	if (std::holds_alternative<const button_event *>(trigger))
	{
		auto be=std::get<const button_event *>(trigger);

		if (!button_clicked(*be))
			return; // Wrong button.

		// Single click sets the input field.

		if (!S_ISDIR(e.st.st_mode)) // But only for a file
		{
			auto p=e.name.rfind('/');

			if (p == std::string::npos)
				p=0;
			else
				++p;

			filename_field->set(e.name.substr(p));

			if (be->click_count != 2)
				autoselect_file=false;
		}
	}

	if (S_ISDIR(e.st.st_mode))
	{
		chdir(IN_THREAD, e.name);
	}
	else
	{
		// We require a double-click to auto-select a file.
		//
		// This means that we always call selected() unless:
		// a) this is a button click, b) click count is not 2.

		if (autoselect_file)
			selected(IN_THREAD, e.name, mcguffin);
	}
}

bool file_dialogObj::implObj::button_clicked(const button_event &be)
{
	return filename_field->elementObj::impl->activate_for(be) && be.button == 1;
}

void file_dialogObj::implObj::enter_key(ONLY IN_THREAD,
					const busy &mcguffin)
{
	process_filename(IN_THREAD, input_lock{filename_field}.get(),
			 mcguffin);
}

void file_dialogObj::implObj::process_filename(ONLY IN_THREAD,
					       const std::string &filename_arg,
					       const busy &mcguffin)
{
	auto filename=filename_arg;

	if (filename.empty())
		return;

	if (filename.substr(0, 1) != "/")
		filename=directory_contents_list->pwd()
			+ "/" + filename;

	auto filename_st=fileattr::create(filename, false)->try_stat();

	if (filename_st && S_ISDIR(filename_st->st_mode))
	{
		if (access(filename.c_str(), (R_OK|X_OK)) == 0)
		{
			chdir(IN_THREAD, filename);
		}
	}
	else
	{
		selected(IN_THREAD, filename, mcguffin);
	}
}

void file_dialogObj::implObj::selected(ONLY IN_THREAD,
				       const std::string &filename,
				       const busy &mcguffin)
{
	auto d=the_file_dialog.getptr();

	if (!d)
		return;

	size_t p=filename.rfind('/');

	if (p == std::string::npos)
		p=0;
	else
		++p;

	auto dir=filename.substr(0, p);

	switch (type) {
	case file_dialog_type::existing_file:
		if (access(filename.c_str(), R_OK) == 0)
			break;
		access_denied(d, access_denied_message, filename);
		return;

	case file_dialog_type::write_file:
		if (access(filename.c_str(), W_OK) == 0)
			break;
		if (errno == ENOENT)
		{
			if (access(dir.c_str(), W_OK) == 0)
				break;
		}

		access_denied(d, access_denied_message, filename);
		return;

	case file_dialog_type::create_file:
		if (access(dir.c_str(), W_OK) == 0)
			break;
		access_denied(d, access_denied_message, filename);
		return;
	}
	ok_action(IN_THREAD,
		  d, filename, mcguffin);
}

text_param file_dialogObj::implObj::create_dirlabel(const std::string &s)
{
	text_param t;

	t( appearance->filedir_directoryfont );

	if (s.empty())
		return t; // Shouldn't happen.

	// Truncate the shown filename.
	auto b=s.begin();
	auto e=s.end();

	auto p=(e-b) < 40 ? b:e-40;

	// Make sure p points at a slash.

	p=std::find(p, e, '/');

	if (p == e)
		while (p > b && *--p != '/')
			;

	// Now make sure there's at least one more slash, so that 'p'
	// points to at least two directory components, that we'll show.

	auto slash=std::find(p+1, e, '/');

	if (slash == e)
	{
		while (p > b)
			if (*--p == '/')
				break;
	}

	// p now points either to an intermediate / separator, or to the
	// leading /. It's always a slash, here.

	if (p > b)
	{
		t(".../");
	}
	else
	{
		create_hotspot(t, "/", "/"); // Root directory
	}

	++p;

	while (p != e)
	{
		auto q=p;

		p=std::find(p, e, '/');

		create_hotspot(t, {q, p}, {b, p});

		// Terminate the hotspot. We do this here, instead of inside
		// create_hotspot(), because another hotspot immediately
		// follows the leading "/".

		t(nullptr);

		if (p != e)
		{
			// Not done yet.
			t("/");
			++p;
		}
	}

	return t;
}

// Create a hotspot for a component of the directory path.

void file_dialogObj::implObj::create_hotspot(text_param &t,
					     const std::string &name,
					     const std::string &path)
{
	t(text_hotspot::create
	  ([name, path,
	    me=make_weak_capture(ref(this))]
	   (ONLY IN_THREAD,
	    const text_event_t &event)
	   {
		   text_param t;

		   auto got=me.get();

		   if (got)
		   {
			   auto &[me]=*got;

			   t=me->hotspot_activated(IN_THREAD,
						   event, name, path);
		   }
		   return t;
	   }));

	t(name);
}

// Hotspot for a directory component has been activated. Figure out the
// course of action.

text_param file_dialogObj::implObj
::hotspot_activated(ONLY IN_THREAD,
		    const text_event_t &event,
		    const std::string &name,
		    const std::string &path)
{
	return std::visit(visitor {
			[&, this](focus_change e)
			{
				text_param t;

				t( appearance->filedir_directoryfont );

				if (e==focus_change::gained)
				{
					t(appearance->filedir_highlight_fg);
					t(appearance->filedir_highlight_bg);
				}
				t(name);
				return t;
			},
			[&, this](const button_event *b)
			{
				if (button_clicked(*b))
					this->chdir(IN_THREAD, path);
				return text_param{};
			},
			[&, this](const key_event *)
			{
				this->chdir(IN_THREAD, path);
				return text_param{};
			}},
		event);
}

void file_dialogObj::implObj::chdir(ONLY IN_THREAD, const std::string &path)
{
	auto realpath=fd::base::realpath(path);
	filename_field->set("");
	directory_field->update(create_dirlabel(realpath));
	directory_contents_list->chdir(realpath);

	update_popup_menu_status update{IN_THREAD, *this};

	dir_status(IN_THREAD).new_focus(false);
	file_status(IN_THREAD).new_focus(false);

	update.update(IN_THREAD);
}

void file_dialogObj::implObj::chfilter(const pcre &filter)
{
	directory_contents_list->chfilter(filter);
}

void file_dialogObj::implObj::access_denied(const file_dialog &the_file_dialog,
					    const std::string &msg,
					    const std::string &filename)
{
	size_t p=filename.rfind('/');

	if (p == std::string::npos)
		p=0;
	else
		++p;

	error_dialog(the_file_dialog,
		     gettextmsg(msg, filename.substr(p)),
		     access_denied_title);
}

void file_dialogObj::implObj::error_dialog(const file_dialog &the_file_dialog,
					   const std::string &error_message,
					   const std::string &title)
{
	standard_dialog_args args{"error@libcxx.com", true};

	args.urgent=true;

	auto d=the_file_dialog->dialog_window->create_ok_dialog
		(args, "alert",
		 [error_message]
		 (const auto &f)
		 {
			 label_config config;

			 config.widthmm=100;
			 config.alignment=halign::center;
			 f->create_label(error_message, config);
		 },
		 the_file_dialog->dialog_window
		 ->destroy_when_closed("error@libcxx.com"));
	d->dialog_window->set_window_title(title);
	d->dialog_window->show_all();
}

// Factored out for readability. Creates the filename input field
static inline input_field create_file_input_field(const factory &f)
{
	input_field_config filename_field_config{60};

	return f->create_input_field("", filename_field_config);

}

// Factored out for readability. Creates the filename input field
static inline focusable_container create_filter_field(const factory &f,
						      const file_dialog_config
						      &conf)
{
	return f->create_focusable_container
		([&]
		 (const auto &new_container)
		 {
			 standard_comboboxlayoutmanager lm=new_container
				 ->get_layoutmanager();

			 std::vector<list_item_param> list;

			 list.reserve(conf.filename_filters.size());

			 for (const auto &f:conf.filename_filters)
			 {
				 list.push_back(std::get<text_param>(f));
			 }

			 lm->replace_all_items(list);

		 },
		 new_standard_comboboxlayoutmanager{}
		 );
}

//! Internal constructor arguments

//! Temporary object that gets created by the public constructor before
//! calling the internal constructor.
//!
//! Provides a place to store the dialog's display elements that get created
//! by the dialogObj superclass, before actually constructing the file_dialog.
struct LIBCXX_HIDDEN file_dialogObj::init_args {

	input_fieldptr filename_field;
	focusable_labelptr directory_field;
	focusable_containerptr filter_field;
	filedirlist_managerptr directory_contents_list;
	buttonptr ok_button;
	buttonptr cancel_button;

	main_window parent_window;
	std::string directory;

	init_args(const main_window &parent_window,
		  const std::string &directory)
		: parent_window{parent_window},
		directory{fd::base::realpath(directory)}
	{
	}

	~init_args()=default;

	standard_dialog_elements_t
		create_elements(const file_dialog_config &conf);
};

// Create the factories for the theme-specified display elements.

standard_dialog_elements_t file_dialogObj::init_args
::create_elements(const file_dialog_config &conf)
{
	return {
		{"file-input-field",
				[&, this]
				(const auto &factory)
				{
					filename_field=
						create_file_input_field
						(factory);
				}},
		{"filter-field",
				[&, this]
				(const auto &factory)
				{
					filter_field=
						create_filter_field(factory,
								    conf);
				}},
		{"directory-contents-list",
				[&, this]
				(const auto &factory)
				{
					directory_contents_list=
						filedirlist_manager
						::create(factory,
							 directory, conf);
				}},
		{"ok", dialog_ok_button(_("Ok"), ok_button, 0)},
		{"cancel", dialog_cancel_button(_("Cancel"),
						cancel_button,
						'\e')}
	};
}

namespace {
#if 0
}
#endif

// Weak reference to the file dialog implementation object.
//
// file_dialogObj::implObj gets constructed after all the elements and
// their encompassing container, for the file dialog. Additionally,
// the callback from the dialog container needs to invoke
// file_dialogObj::implObj::process_filename(). Safely store a weak pointer
// to the file_dialogObj::implObj object, for use when needed.

class LIBCXX_HIDDEN file_dialog_weak_refObj : virtual public obj {


 public:

	mpobj<weakptr<ptr<file_dialogObj::implObj>>> file_dialog_impl;
};

typedef ref<file_dialog_weak_refObj> file_dialog_weak_ref;

// Handler for a text/uri-list selection that's getting dropped into the
// file dialog.
//
// Parse it, and invoke process_filename() in the file dialog, as if the file
// was manually typed in or clicked on.

class LIBCXX_HIDDEN text_uri_selection_handlerObj
	: public current_selection_handlerObj {

	const ref<obj> mcguffin;

	const file_dialog_weak_ref r;

	std::string text;

 public:
	text_uri_selection_handlerObj(const ref<obj> &mcguffin,
				      const file_dialog_weak_ref &r)
		: mcguffin{mcguffin}, r{r}
	{
	}

	bool begin_converted_data(ONLY IN_THREAD,
				  xcb_atom_t type,
				  xcb_timestamp_t timestamp) override
	{
		return type==IN_THREAD->info
			->atoms_info.text_uri_list_mime;
	}

	//! Received converted data.
	void converted_data(ONLY IN_THREAD,
			    void *data,
			    size_t size,
			    generic_windowObj::handlerObj &me) override
	{
		char *p=reinterpret_cast<char *>(data);

		text.insert(text.end(), p, p+size);
	}

	//! End of conversion.
	void end_converted_data(ONLY IN_THREAD,
				generic_windowObj::handlerObj &me) override
	{
		std::vector<std::string> uri_strings;

		strtok_str(text, "\r\n", uri_strings);

		for (const auto &s:uri_strings)
		{
			if (s.empty() || *s.c_str() == '#')
				continue;

			uriimpl u{s};

			auto scheme=u.getScheme();

			if (!scheme.empty() && scheme != "file")
			{
				me.stop_message(static_cast<std::string>
						(gettextmsg
						 (_("I can only open files, I "
						    "don't speak %1%"),
						  scheme)));
				return;
			}

			auto authority=u.getAuthority().to_string();

			if (!authority.empty() && authority != "localhost")
			{
				me.stop_message(static_cast<std::string>
						(gettextmsg
						 (_("I can only open files, I "
						    "cannot open files on "
						    "\"%1%\""),
						  authority)));
				return;
			}

			auto path=u.getPath();

			auto file_dialog_impl=r->file_dialog_impl
				.get().getptr();

			if (!file_dialog_impl)
				return;

			busy_impl yes_i_am{me};

			file_dialog_impl->process_filename(IN_THREAD, path,
							   yes_i_am);
		}
	}

	//! Conversion has failed.
	void conversion_failed(ONLY IN_THREAD,
			       xcb_atom_t type,
			       generic_windowObj::handlerObj &me) override
	{
	}
};

// Implementation object for the file dialog parent container.
//
// Implements accepting dropped text/uri-list content, as if it was
// selected directly from the file dialog.

class LIBCXX_HIDDEN file_dialog_container_implObj :
	public drag_destination_elementObj<container_elementObj
					   <child_elementObj>> {

 public:

	const file_dialog_weak_ref dialog_impl_ref;

	typedef drag_destination_elementObj<
		container_elementObj<child_elementObj>> superclass_t;

	file_dialog_container_implObj(const ref<containerObj::implObj>
				      &parent_container)
		: superclass_t{
		parent_container,
			child_element_init_params{"background@libcxx.com"}},
		dialog_impl_ref{file_dialog_weak_ref::create()}
	{
	}


	bool accepts_drop(ONLY IN_THREAD,
			  const source_dnd_formats_t &formats,
			  xcb_timestamp_t timestamp) override
	{
		return formats.find(IN_THREAD->info
				    ->atoms_info.text_uri_list_mime)
			!= formats.end();
	}

	// Create a text_uri_selection_handler for dropping the URI list.

	current_selection_handlerptr drop(ONLY IN_THREAD,
					  xcb_atom_t &type,
					  const ref<obj> &finish_mcguffin)
		override
	{
		type=IN_THREAD->info->atoms_info.text_uri_list_mime;

		return ref<text_uri_selection_handlerObj>
			::create(finish_mcguffin, dialog_impl_ref);
	}
};

#if 0
{
#endif
}

file_dialog main_windowObj
::create_file_dialog(const standard_dialog_args &args,
		     const file_dialog_config &conf)
{
	return create_custom_dialog
		(create_dialog_args{args},
		 [&, this]
		 (const dialog_args &args)
		 {
			 file_dialogObj::init_args init_args
				 {
				  ref{this},
				  conf.initial_directory
				 };

			 uielements tmpl{init_args.create_elements(conf)};

			 gridlayoutmanager glm=
				 args.dialog_window->get_layoutmanager();
			 auto f=glm->append_row();

			 f->padding(0);

			 new_gridlayoutmanager nglm;

			 auto child_impl=
				 ref<file_dialog_container_implObj>
				 ::create(f->get_container_impl());
			 auto c=container::create(child_impl,
						  nglm.create(child_impl));

			 gridlayoutmanager inner_glm=c->get_layoutmanager();

			 inner_glm->generate("file-dialog", tmpl);

			 init_args.directory_field=
				 tmpl.get_element("directory-field");

			 f->created_internally(c);

			 auto fd=file_dialog::create(args,
						     conf,
						     init_args);

			 // Now that everything's built, we can fill in the
			 // last missing piece.

			 child_impl->dialog_impl_ref->file_dialog_impl=
				 fd->impl;

			 return fd;
		 });
}

//////////////////////////////////////////////////////////////////////////

// Construction

file_dialogObj::file_dialogObj(const dialog_args &d_args,
			       const file_dialog_config &conf,
			       const init_args &args)
	: dialogObj(d_args),
	  impl(ref<implObj>::create(args.directory_field,
				    args.filename_field,
				    args.filter_field,
				    args.directory_contents_list,
				    args.ok_button,
				    args.cancel_button,
				    conf.ok_action,
				    conf.type,
				    conf.access_denied_message,
				    conf.access_denied_title,
				    conf.appearance))
{
}

// Convert conf.filename_filters to pcre objects.

// Factored out for readability.

static inline auto create_pcre_filters(const
				       std::vector<std::tuple<text_param,
				       std::string>> &filename_filters)
{
	std::vector<pcre> filters;

	filters.reserve(filename_filters.size());
	for (const auto &f:filename_filters)
		filters.push_back(pcre::create(std::get<std::string>(f)));

	return filters;
}

static std::u32string base_filename(const std::string &filename)
{
	if (filename == "/")
		return U"/";

	auto p=filename.rfind('/');

	auto ustr=unicode::iconvert::tou::convert(filename.substr(p+1),
						  unicode::utf_8).first;

	if (ustr.size() < 40)
		return ustr;

	return ustr.substr(0, 40) + U"\u2026";
}

// Filename field in dialogs: does not accept '/' characters.

static void no_slashes(const input_field &f)
{
	f->on_filter([]
		     (ONLY IN_THREAD,
		      const input_field_filter_info &info)
		     {
			     auto p=info.new_contents.rfind('/');

			     info.update(info.starting_pos,
					 info.n_delete,
					 info.new_contents.substr(++p));
		     });
}

static void filename_required(const input_field &filename,
			      const button &ok_button)
{
	// The "Ok" button is initially disabled.
	//
	// Enable it when the filename_field is not empty.

	ok_button->set_enabled(false);

	filename->on_change([ok_button]
			    (THREAD_CALLBACK,
			     const auto &info)
			    {
				    ok_button->set_enabled(info.size > 0);
			    });
}

// mkdir dialog, factored out for readability.

static inline void popup_mkdir_dialog(const main_window &w,
				      const std::string &filename,
				      const ref<file_dialogObj::implObj> &impl)
{
	auto autodestroy=w->destroy_when_closed("mkdir@libcxx.com");

	auto d=w->create_input_dialog
		({"mkdir@libcxx.com", true},
		 "question",
		 [&]
		 (const auto &f)
		 {
			 f->create_label({_("New subdirectory in "),
					  base_filename(filename),
					  U":"},
				 {halign::left, 100});;
		 },
		 "",
		 input_field_config{40},
		 [filename, impl, autodestroy]
		 (ONLY IN_THREAD,
		  const auto &args)
		 {
			 input_lock lock{args.dialog_input_field};

			 auto dir=filename + "/" + lock.get();

			 if (mkdir(dir.c_str(), 0777) < 0)
			 {
				 args.dialog_main_window->stop_message
					 (strerror(errno));
				 return;
			 }
			 else
			 {
				 // Open this directory.
				 impl->chdir(IN_THREAD, dir);
			 }
			 autodestroy(IN_THREAD, args);
		 },
		 autodestroy,
		 _("Create subdirectory"),
		 _("Cancel"));

	no_slashes(d->input_dialog_field);
	filename_required(d->input_dialog_field,
			  d->input_dialog_ok);
	d->dialog_window->show_all();
}

// rmdir dialog, factored out for readability.

static inline void popup_rmdir_dialog(const main_window &w,
				      const std::string &filename)
{
	auto autodestroy=w->destroy_when_closed("rmdir@libcxx.com");

	auto d=w->create_ok_cancel_dialog
		({"rmdir@libcxx.com", true},
		 "question",
		 [&]
		 (const auto &f)
		 {
			 f->create_label({_("Delete "),
					  base_filename(filename), "?"},
				 {halign::left, 100});;
		 },
		 [filename, autodestroy]
		 (ONLY IN_THREAD,
		  const auto &args)
		 {
			 if (rmdir(filename.c_str()) < 0)
			 {
				 args.dialog_main_window->stop_message
					 (strerror(errno));
				 return;
			 }
			 autodestroy(IN_THREAD, args);
		 },
		 autodestroy,
		 _("Delete subdirectory"),
		 _("Cancel"));

	d->dialog_window->show_all();
}

// rename dialog, factored out for readability.

static inline void popup_rename_dialog(const main_window &w,
				       const std::string &filename)
{
	auto autodestroy=w->destroy_when_closed("rename@libcxx.com");

	auto d=w->create_input_dialog
		({"rename@libcxx.com", true},
		 "question",
		 [&]
		 (const auto &f)
		 {
			 f->create_label({_("Rename "),
					  base_filename(filename),
					  U":"},
				 {halign::left, 100});
		 },
		 "",
		 input_field_config{40},
		 [filename, autodestroy]
		 (ONLY IN_THREAD,
		  const auto &args)
		 {
			 input_lock lock{args.dialog_input_field};

			 auto new_filename=
				 filename.substr(0, filename.rfind('/')+1)
				 + lock.get();

			 if (rename(filename.c_str(),
				    new_filename.c_str()) < 0)
			 {
				 args.dialog_main_window->stop_message
					 (strerror(errno));
				 return;
			 }
			 autodestroy(IN_THREAD, args);
		 },
		 autodestroy,
		 _("Rename"),
		 _("Cancel"));

	no_slashes(d->input_dialog_field);
	filename_required(d->input_dialog_field,
			  d->input_dialog_ok);
	d->dialog_window->show_all();
}

// unlink dialog, factored out for readability.

static inline void popup_unlink_dialog(const main_window &w,
				       const std::string &filename)
{
	auto autodestroy=w->destroy_when_closed("unlink@libcxx.com");

	auto d=w->create_ok_cancel_dialog
		({"unlink@libcxx.com", true},
		 "question",
		 [&]
		 (const auto &f)
		 {
			 f->create_label({_("Delete "),
					  base_filename(filename), "?"},
				 {halign::left, 100});;
		 },
		 [filename, autodestroy]
		 (ONLY IN_THREAD,
		  const auto &args)
		 {
			 if (unlink(filename.c_str()) < 0)
			 {
				 args.dialog_main_window->stop_message
					 (strerror(errno));
				 return;
			 }
			 autodestroy(IN_THREAD, args);
		 },
		 autodestroy,
		 _("Delete file"),
		 _("Cancel"));

	d->dialog_window->show_all();
}

// Phase 2 of the constructor. Set up all the callbacks.
void file_dialogObj::constructor(const dialog_args &d_args,
				 const file_dialog_config &conf,
				 const init_args &args)
{
	auto d=dialog{this};

	// Need to tell the implementation object who we are.

	impl->the_file_dialog=d;

	auto me=ref{this};
	// The cancel button, and the window close button, invokes the
	// cancel_action, as usual.
	hide_and_invoke_when_activated(args.parent_window, d,
				       impl->cancel_button,
				       [cancel_action=conf.cancel_action]
				       (ONLY IN_THREAD,
					const auto &args)
				       {
					       cancel_action(IN_THREAD, args);
				       });

	hide_and_invoke_when_closed(args.parent_window, d,
				    [cancel_action=conf.cancel_action]
				    (ONLY IN_THREAD,
				     const auto &busy)
				    {
					    cancel_action(IN_THREAD, busy);
				    });

	filename_required(impl->filename_field, impl->ok_button);

	// Set up the callback to reread the directory when the field
	// combo-box changes.


	{
		standard_comboboxlayoutmanager lm=impl->filter_field
			->get_layoutmanager();

		if (conf.filename_filters.size()<=conf.initial_filename_filter)
			throw EXCEPTION(_("Invalid initial_filename_filter"));

		// Callback to invoke manager->chfilter() whenever the filename
		// filter combobox dropdown changes.

		lm->selection_changed
			([filters=create_pcre_filters(conf.filename_filters),
			  manager=impl->directory_contents_list]
			 (ONLY IN_THREAD,
			  const auto &info)
			 {
				 if (!info.list_item_status_info.selected)
					 return;

				 auto i=info.list_item_status_info.item_number;

				 manager->chfilter(filters.at(i));
			 });

		// This will invoke the newly-installed callback, indirectly
		// initializing the initial filename filter.

		lm->autoselect(conf.initial_filename_filter);
	}

	impl->directory_contents_list->set_selected_callback
		([impl=make_weak_capture(impl, dialog_window)]
		 (ONLY IN_THREAD,
		  const auto &arg,
		  const callback_trigger_t &trigger)
		 {
			 auto got=impl.get();

			 if (!got)
				 return;

			 auto &[impl, dialog_window]=*got;

			 // We want to ignore events reported on button release
			 // events that get directed to a popup, and thus
			 // release input focus/selection on the underlying
			 // list.
			 //
			 // Right mouse button press opens the popup menu.
			 //
			 // When the mouse button gets released, this causes
			 // the list to lose keyboard focus, which gets
			 // reported.
			 //
			 // This would normally result in us deactivated
			 // the relevant menu items.
			 //
			 // Simply ignore button release events, here.

			 std::visit
				 (visitor
				  {
				   [&](const filedirlist_selected &what)
				   {
					   impl->clicked(IN_THREAD,
							 what, trigger,
							 what.mcguffin);
				   },
				   [&](const filedirlist_current_list_item
				       &item)
				   {
					   // If the item change occured
					   // because of keyboard activity,
					   // treat it as a selection for the
					   // purposes of the right context
					   // popup menus' status.

					   if (!std::holds_alternative
					       <const key_event *>(trigger))
						   return;

					   impl->file_or_dir_selection
						   (IN_THREAD, item.section,
						    item.n);
				   },
				   [&](const filedirlist_mkdir &what)
				   {
					   popup_mkdir_dialog
						   (dialog_window,
						    what.filename,
						    impl);
				   },
				   [&](const filedirlist_rmdir &what)
				   {
					   popup_rmdir_dialog(dialog_window,
							      what.filename);
				   },
				   [&](const filedirlist_rename &what)
				   {
					   popup_rename_dialog(dialog_window,
							       what.filename);
				   },
				   [&](const filedirlist_unlink &what)
				   {
					   popup_unlink_dialog(dialog_window,
							       what.filename);
				   }},
				 arg);
		 });

	// Set up a callback that invokes enter().

	impl->filename_field->on_key_event
		([impl=make_weak_capture(impl)]
		 (ONLY IN_THREAD,
		  const auto &event,
		  bool activated,
		  const auto &busy_mcguffin)
		 {
			 if (!std::holds_alternative<const key_event *>(event))
				 return false;

			 const auto &ke=*
				 std::get<const key_event *>(event);

			 if (ke.unicode != '\n')
				 return false;
			 if (!activated)
				 return true;

			 auto got=impl.get();

			 if (got)
			 {
				 auto &[elements]=*got;

				 if (elements->filename_field
				     ->elementObj::impl
				     ->activate_for(ke))
					 elements->enter_key(IN_THREAD,
							     busy_mcguffin);
			 }

			 return true;
		 });

	// The initial path displayed by the current directory label.

	impl->directory_field->update
		(impl->create_dirlabel(impl->directory_contents_list->pwd()));

	// Set up the Ok button to act like the Enter key.
	impl->ok_button->on_activate
		([impl=make_weak_capture(impl)]
		 (ONLY IN_THREAD,
		  const auto &trigger, const auto &busy)
		 {
			 auto got=impl.get();

			 if (got)
			 {
				 auto &[elements]=*got;

				 elements->enter_key(IN_THREAD, busy);
			 }
		 });
}

file_dialogObj::~file_dialogObj()=default;

void file_dialogObj::chdir(const std::string &path)
{
	dialog_window->in_thread([path, impl=impl]
				 (ONLY IN_THREAD)
				 {
					 impl->chdir(IN_THREAD, path);
				 });
}

LIBCXXW_NAMESPACE_END
