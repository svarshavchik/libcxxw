/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "file_dialog/file_dialog_impl.H"
#include "dirlisting/filedirlist_manager.H"
#include "main_window.H"
#include "dialog.H"
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
#include <x/fd.H>
#include <x/visitor.H>
#include <x/fileattr.H>
#include <x/weakcapture.H>
#include <courier-unicode.h>
#include <algorithm>

#include <sys/stat.h>

LIBCXXW_NAMESPACE_START

// All the elements in the file dialog. In one easy place for all callbacks
// to reference.

class LIBCXX_HIDDEN file_dialogObj::implObj::elementsObj : virtual public obj {


 public:

	buttonptr ok_button;
	buttonptr cancel_button;
	focusable_labelptr directory_field;
	input_fieldptr filename_field;
	filedirlist_managerptr directory_contents_list;

	// A directory entry was clicked on.

	void clicked(size_t n,
		     const callback_trigger_t &trigger)
	{
		if (std::holds_alternative<const button_event *>(trigger)
		    &&
		    !button_clicked(*std::get<const button_event *>(trigger)))
			return;

		auto e=directory_contents_list->at(n);

		if (S_ISDIR(e.st.st_mode))
		{
			chdir(e.name);
		}
	}

	// We take action whenever button 1 gets double-clicked.

	static bool button_clicked(const button_event &be)
	{
		return !be.press && be.button == 1 && be.click_count == 2;
	}

	// Show this in the directory field.

	text_param create_dirlabel(const std::string &);

	// Enter pressed in the filename field.

	void enter_key()
	{
		auto filename=input_lock{filename_field}.get();

		if (filename.empty())
			return;

		if (filename.substr(0, 1) != "/")
			filename=directory_contents_list->pwd()
				+ "/" + filename;

		struct ::stat st{};

		try {
			st=*fileattr::create(filename, false)->stat();
		} catch (...)
		{
		}

		if (S_ISDIR(st.st_mode))
		{
			if (access(filename.c_str(), R_OK) == 0)
			{
				chdir(fd::base::realpath(filename));
			}
		}
	}

 private:

	void create_hotspot(text_param &t,
			    const std::string &name,
			    const std::string &path);

	text_param hotspot_activated(const text_event_t &event,
				     const std::string &name,
				     const std::string &path);

	void chdir(const std::string &path);
};

typedef file_dialogObj::implObj::elementsObj elementsObj;

text_param elementsObj::create_dirlabel(const std::string &s)
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

void elementsObj::create_hotspot(text_param &t,
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

text_param elementsObj::hotspot_activated(const text_event_t &event,
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

void elementsObj::chdir(const std::string &path)
{
	filename_field->set("");
	directory_field->update(create_dirlabel(path));
	directory_contents_list->chdir(path);
}

// Factored out for readability. Creates the directory contents list.

static inline void
create_directory_contents_list(const ref<elementsObj> &elements,
			       const std::string &directory,
			       const factory &f)
{
	elements->directory_contents_list=
		filedirlist_manager::create
		(f, directory,
		 [elements=make_weak_capture(elements)]
		 (size_t n,
		  const callback_trigger_t &trigger,
		  const busy &)
		 {
			 elements.get([&]
				      (const auto &e) {
					      e->clicked(n, trigger);
				      });
		 });
}

// Factored out for readability. Creates the filename input field
static inline void create_file_input_field(const ref<elementsObj>
					   &elements,
					   const factory &f)
{
	input_field_config filename_field_config{60};

	auto field=f->create_input_field("", filename_field_config);

	elements->filename_field=field;
	field->on_key_event([elements=make_weak_capture(elements)]
			    (const auto &event,
			     const auto &busy_mcguffin)
			    {
				    if (!std::holds_alternative
					<const key_event *>(event))
					    return false;

				    const auto &ke=*
					    std::get<const key_event *>(event);

				    if (!ke.keypress ||
					ke.unicode != '\n')
					    return false;

				    elements.get([&]
						 (const auto &elements)
						 {
							 elements->enter_key();
						 });
				    return true;
			    });
}

file_dialog main_windowObj::create_file_dialog(const file_dialog_config &,
					       bool modal)
{
	std::string directory=fd::base::realpath(".");

	auto elements=ref<elementsObj>::create();

	auto d=create_standard_dialog("file-dialog", modal, {
			{"file-input-label",
					[&]
					(const auto &factory)
					{
						factory->create_label("File:");
					}},
			{"file-input-field",
					[&]
					(const auto &factory)
					{
						create_file_input_field
							(elements,
							 factory);
					}},
			{"directory-label",
					[&]
					(const auto &factory)
					{
						factory->create_label("Directory:");
					}},
			{"directory-field",
					[&]
					(const auto &factory)
					{
						elements->directory_field=
							factory
							->create_focusable_label
							(elements
							 ->create_dirlabel
							 (directory));
					}},
			{"directory-contents-list",
					[&]
					(const auto &factory)
					{
						create_directory_contents_list
							(elements,
							 directory,
							 factory);
					}},
			{"ok", dialog_ok_button("Ok", elements->ok_button,
						0)},
			{"filler", dialog_filler()},
			{"cancel", dialog_cancel_button("Cancel",
							elements->cancel_button,
							'\e')}
		});

	hide_and_invoke_when_activated
		(d, elements->ok_button,
		 []
		 (const auto &busy)
		 {
		 });

	hide_and_invoke_when_activated(d, elements->cancel_button,
				       []
				       (const auto &busy)
				       {
				       });

	hide_and_invoke_when_closed(d,
				    []
				    (const auto &busy)
				    {
				    });

	auto impl=ref<file_dialogObj::implObj>::create(elements, d);

	return file_dialog::create(impl);
}

//////////////////////////////////////////////////////////////////////////////

file_dialogObj::implObj::implObj(const ref<elementsObj> &elements,
				 const dialog &the_dialog)
	: elements{elements}, the_dialog{the_dialog}
{
}

file_dialogObj::implObj::~implObj()=default;


LIBCXXW_NAMESPACE_END
