/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "file_dialog/file_dialog_impl.H"
#include "dirlisting/filedirlist_manager.H"
#include "main_window.H"
#include "dialog.H"
#include "element.H"
#include "messages.H"
#include "x/w/input_field.H"
#include "x/w/input_field_lock.H"
#include "x/w/file_dialog_config.H"
#include "x/w/label.H"
#include "x/w/focusable_label.H"
#include "x/w/text_hotspot.H"
#include "x/w/button.H"
#include "x/w/text_param.H"
#include "x/w/gridfactory.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include <x/fd.H>
#include <x/visitor.H>
#include <x/fileattr.H>
#include <x/pcre.H>
#include <x/weakcapture.H>
#include <courier-unicode.h>
#include <algorithm>
#include <errno.h>
#include <sys/stat.h>

LIBCXXW_NAMESPACE_START


file_dialogObj::implObj
::implObj(const focusable_label &directory_field,
	  const input_field &filename_field,
	  const focusable_container &filter_field,
	  const filedirlist_manager &directory_contents_list,
	  const button &ok_button,
	  const button &cancel_button,
	  const std::function<void (const file_dialog &,
				    const std::string &, const busy &)
	  > &ok_action,
	  file_dialog_type type,
	  const std::string &access_denied_message,
	  const std::string &access_denied_title)
	: directory_field(directory_field),
	  filename_field(filename_field),
	  filter_field(filter_field),
	  directory_contents_list(directory_contents_list),
	  ok_button(ok_button),
	  cancel_button(cancel_button),
	  ok_action(ok_action),
	  type(type),
	  access_denied_message(access_denied_message),
	  access_denied_title(access_denied_title)
{
}

file_dialogObj::implObj::~implObj()=default;

void file_dialogObj::implObj::clicked(size_t n,
				      const callback_trigger_t &trigger,
				      const busy &mcguffin)
{
	auto e=directory_contents_list->at(n);

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
		chdir(e.name);
	}
	else
	{
		// We require a double-click to auto-select a file.
		//
		// This means that we always call selected() unless:
		// a) this is a button click, b) click count is not 2.

		if (autoselect_file)
			selected(e.name, mcguffin);
	}
}

bool file_dialogObj::implObj::button_clicked(const button_event &be)
{
	return filename_field->elementObj::impl->activate_for(be) && be.button == 1;
}

void file_dialogObj::implObj::enter_key(const busy &mcguffin)
{
	auto filename=input_lock{filename_field}.get();

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
			chdir(filename);
		}
	}
	else
	{
		selected(filename, mcguffin);
	}
}

void file_dialogObj::implObj::selected(const std::string &filename,
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
			break;

		access_denied(d, access_denied_message, filename);
		return;

	case file_dialog_type::create_file:
		if (access(dir.c_str(), W_OK) == 0)
			break;
		access_denied(d, access_denied_message, filename);
		return;
	}
	ok_action(d, filename, mcguffin);
}

text_param file_dialogObj::implObj::create_dirlabel(const std::string &s)
{
	text_param t;

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
	   (const text_event_t &event)
	   {
		   text_param t;

		   me.get([&]
			  (const auto &me)
			  {
				  t=me->hotspot_activated(event, name, path);
			  });
		   return t;
	   }));

	t(name);
}

// Hotspot for a directory component has been activated. Figure out the
// course of action.

text_param file_dialogObj::implObj
::hotspot_activated(const text_event_t &event,
		    const std::string &name,
		    const std::string &path)
{
	return std::visit(visitor {
			[&, this](focus_change e)
			{
				text_param t;

				if (e==focus_change::gained)
				{
					t(theme_color{"filedir_highlight_fg"});
					t(theme_color{"filedir_highlight_bg"});
				}
				t(name);
				return t;
			},
			[&, this](const button_event *b)
			{
				if (button_clicked(*b))
					this->chdir(path);
				return text_param{};
			},
			[&, this](const key_event *)
			{
				this->chdir(path);
				return text_param{};
			}},
		event);
}

void file_dialogObj::implObj::chdir(const std::string &path)
{
	auto realpath=fd::base::realpath(path);
	filename_field->set("");
	directory_field->update(create_dirlabel(realpath));
	directory_contents_list->chdir(realpath);
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
	auto d=the_file_dialog->dialog_window->create_ok_dialog
		("error@libcxx", "alert",
		 [error_message]
		 (const auto &f)
		 {
			 f->create_label(error_message, 100.00, halign::center);
		 },
		 the_file_dialog->dialog_window
		 ->destroy_when_closed("error@libcxx"),
		 true);
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

	std::string directory;

	init_args(const std::string &directory)
		: directory{fd::base::realpath(directory)}
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
		{"file-input-label",
				[&]
				(const auto &factory)
				{
					factory->create_label("File:");
				}},
		{"file-input-field",
				[&, this]
				(const auto &factory)
				{
					filename_field=
						create_file_input_field
						(factory);
				}},
		{"directory-label",
				[&]
				(const auto &factory)
				{
					factory->create_label("Directory:");
				}},
		{"directory-field",
				[&, this]
				(const auto &factory)
				{
					directory_field=factory
						->create_focusable_label
						("");
				}},
		{"filter-label",
				[&]
				(const auto &factory)
				{
					factory->create_label("Files:");
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
							 directory,
							 conf.type);
				}},
		{"ok", dialog_ok_button("Ok", ok_button, 0)},
		{"filler", dialog_filler()},
		{"cancel", dialog_cancel_button("Cancel",
						cancel_button,
						'\e')}
	};
}

file_dialog main_windowObj
::create_file_dialog(const std::string_view &dialog_id,
		     const file_dialog_config &conf,
		     bool modal)
{
	return create_custom_dialog
		(dialog_id,
		 [&]
		 (const dialog_args &args)
		 {
			 file_dialogObj::init_args init_args{
				 conf.initial_directory};

			 args.dialog_window->initialize_theme_dialog
				 ("file-dialog",
				  init_args.create_elements(conf));

			 return file_dialog::create(args,
						    conf,
						    init_args);
		 },
		 modal);
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
				    conf.access_denied_title))
{
}

// Convert conf.filename_filters to pcre objects.

// Factored out for readability.

static inline auto create_pcre_filters(const auto &filename_filters)
{
	std::vector<pcre> filters;

	filters.reserve(filename_filters.size());
	for (const auto &f:filename_filters)
		filters.push_back(pcre::create(std::get<std::string>(f)));

	return filters;
}

// Phase 2 of the constructor. Set up all the callbacks.
void file_dialogObj::constructor(const dialog_args &d_args,
				 const file_dialog_config &conf,
				 const init_args &args)
{
	auto d=dialog{this};

	// Need to tell the implementation object who we are.

	impl->the_file_dialog=d;

	// The cancel button, and the window close button, invokes the
	// cancel_action, as usual.
	hide_and_invoke_when_activated(d, impl->cancel_button,
				       [cancel_action=conf.cancel_action]
				       (const auto &busy)
				       {
					       cancel_action(busy);
				       });

	hide_and_invoke_when_closed(d,
				    [cancel_action=conf.cancel_action]
				    (const auto &busy)
				    {
					    cancel_action(busy);
				    });

	// The "Ok" button is initially disabled.
	//
	// Enable it when the filename_field is not empty.
	impl->ok_button->set_enabled(false);

	impl->filename_field
		->on_change([ok_button=impl->ok_button]
			    (const auto &info)
			    {
				    ok_button->set_enabled(info.size > 0);
			    });

	// Set up the callback to reread the directory when the field
	// combo-box changes.


	{
		standard_comboboxlayoutmanager lm=impl->filter_field
			->get_layoutmanager();

		if (lm->size() <= conf.initial_filename_filter)
			throw EXCEPTION(_("Invalid initial_filename_filter"));

		// Callback to invoke manager->chfilter() whenever the filename
		// filter combobox dropdown changes.

		lm->selection_changed
			([filters=create_pcre_filters(conf.filename_filters),
			  manager=impl->directory_contents_list]
			 (const auto &info)
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

	// Set up the clicked() callback, clicking on a directory entry.

	impl->directory_contents_list->set_selected_callback
		([impl=make_weak_capture(impl)]
		 (size_t n,
		  const callback_trigger_t &trigger,
		  const busy &mcguffin)
		 {
			 impl.get([&]
				  (const auto &impl) {
					  impl->clicked(n, trigger, mcguffin);
				  });
		 });

	// Set up a callback that invokes enter().

	impl->filename_field->on_key_event
		([impl=make_weak_capture(impl)]
		 (const auto &event,
		  const auto &busy_mcguffin)
		 {
			 if (!std::holds_alternative<const key_event *>(event))
				 return false;

			 const auto &ke=*
				 std::get<const key_event *>(event);

			 if (ke.unicode != '\n')
				 return false;

			 impl.get([&]
				      (const auto &elements)
				      {
					      if (!elements->filename_field
						  ->elementObj::impl
						  ->activate_for(ke))
						      return;

					      elements->enter_key
						      (busy_mcguffin)
						      ;
				      });
			 return true;
		 });

	// The initial path displayed by the current directory label.

	impl->directory_field->update
		(impl->create_dirlabel(impl->directory_contents_list->pwd()));

	// Set up the Ok button to act like the Enter key.
	impl->ok_button->on_activate
		([impl=make_weak_capture(impl)]
		 (const auto &trigger, const auto &busy)
		 {
			 impl.get([&]
				  (const auto &elements)
				  {
					  elements->enter_key(busy);
				  });
		 });
}

file_dialogObj::~file_dialogObj()=default;

void file_dialogObj::chdir(const std::string &path)
{
	impl->chdir(path);
}

LIBCXXW_NAMESPACE_END
