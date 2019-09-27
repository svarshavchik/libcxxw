#include "libcxxw_config.h"

#include "creator/app.H"
#include "x/w/screen_positions.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/dialog_args.H"
#include "x/w/file_dialog_config.H"
#include "x/w/file_dialog.H"
#include "x/w/label.H"
#include "messages.H"

#include <x/config.H>
#include <x/messages.H>
#include <x/locale.H>
#include <cstdlib>

#ifndef CREATORDIR
#define CREATORDIR PKGDATADIR "/creator"
#endif

struct appObj::init_args {

	std::string configfile=x::configdir("cxxwcreator@libcxx.com")
		+ "/windows";

	app_elements_tptr elements;
};

// Helper for installing a main menu action.

static void install_menu_event(x::w::uielements &ui,
			       const char *menu_item,
			       void (appObj::*menu_event)())
{
	ui.get_listitemhandle(menu_item)->on_status_update
		([menu_event]
		 (ONLY IN_THREAD,
		  const auto &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 appinvoke(menu_event);
		 });
}

static inline appObj::init_args create_init_args()
{
	appObj::init_args args;

	auto pos=x::w::screen_positions::create(args.configfile);

	x::w::main_window_config config;

	config.restore(pos, "main");

	auto utf8_locale=x::locale::base::utf8();

	auto catalog=x::messages::create("libcxxw", utf8_locale);

	auto main_generator=
		x::w::const_uigenerators::create(CREATORDIR "/main.xml", pos,
						 catalog);

	args.elements.main_generator=main_generator;

	args.elements.main_window=x::w::main_window::create
		(config,
		 [&]
		 (const auto &mw)
		 {
			 x::w::uielements ui;

			 mw->on_disconnect([]
					   {
						   exit(1);
					   });

			 mw->on_delete
				 ([]
				  (ONLY IN_THREAD,
				   const auto &ignore)
				  {
					  appinvoke(&appObj::file_quit_event);
				  });

			 // Create the main menu.

			 auto mb=mw->get_menubarlayoutmanager();

			 mb->generate("mainmenubar", main_generator, ui);

			 // Create the file save dialog

			 x::w::standard_dialog_args file_save_dialog_args
				 {
				  "filesave@creator.w.libcxx.com",
				  true
				 };
			 file_save_dialog_args.restore(pos, "filesave");

			 x::w::file_dialog_config file_save_dialog_config
				 {[](ONLY IN_THREAD,
				     const auto &me,
				     const std::string &filename,
				     const x::w::busy &mcguffin)
				  {
					  appinvoke(&appObj::
						    do_check_and_file_save,
						    me,
						    filename);
				  },
				  [](ONLY IN_THREAD,
				     const auto &ignore)
				  {
				  },
				  x::w::file_dialog_type::create_file};

			 auto d=mw->create_file_dialog(file_save_dialog_args,
						       file_save_dialog_config);

			 d->dialog_window->set_window_title
				 (_("Save As..."));
			 mw->set_window_class("main", "creator@w.libcxx.com");

			 // Install main menu events

			 install_menu_event(ui, "file_save",
					    &appObj::file_save_event);
			 install_menu_event(ui, "file_save_as",
					    &appObj::file_save_as_event);
			 install_menu_event(ui, "file_quit",
					    &appObj::file_quit_event);

			 // Initial contents.

			 x::w::gridlayoutmanager lm=mw->get_layoutmanager();
			 lm->generate("main", main_generator, ui);
		 });
	return args;
}

x::xml::doc appObj::new_file()
{
	auto d=x::xml::doc::create();

	auto lock=d->writelock();

	lock->create_child()->element({"theme"})->attribute({"version", "1"});

	return d;
}

appObj::appObj() : appObj{create_init_args()}
{
}

appObj::appObj(init_args &&args)
	: app_elements_t{std::move(args.elements)},
	  configfile{args.configfile}
{
	loaded_file();
	main_window->get_menubar()->show();
	main_window->show_all();
}

void appObj::loaded_file()
{
	update_title();
}

// Update the main window title's after loading or saving a file.

void appObj::update_title()
{
	std::string title=_("LibCXXW UI Creator");

	auto n=themename.get();

	n=n.substr(n.rfind('/')+1);

	if (!n.empty())
	{
		title += " - ";
		title += n;
	}

	main_window->set_window_title(title);
}

appObj::~appObj()
{
	auto pos=x::w::screen_positions::create();

	main_window->save(pos);
	pos->save(configfile);
}

void appObj::mainloop()
{
	app me{this};

	while (running)
		eventqueue->pop()(me);
}

void appObj::file_save_event()
{
}

void appObj::file_save_as_event()
{
	main_window->get_dialog("filesave@creator.w.libcxx.com")
		->dialog_window->show_all();
}

void appObj::do_check_and_file_save(const x::w::file_dialog &me,
				    std::string filename)
{
	me->dialog_window->hide();

	// Warn if the file exists.

	if (access(filename.c_str(), 0) == 0)
	{
		main_window->create_ok_cancel_dialog
			({"alert@creator.w.libcxx.com", true},
			 "alert",
			 []
			 (const auto &f)
			 {
				 f->create_label(_("Overwrite existing"
						   " file?"));
			 },
			 [filename]
			 (ONLY IN_THREAD,
			  const auto &ignore)
			 {
				 unlink(filename.c_str());

				 appinvoke(&appObj::do_file_save,
					   filename);
			 },
			 []
			 (ONLY IN_THREAD,
			  const auto &ignore)
			 {
			 },
			 _("Overwrite"),
			 _("Cancel"))->dialog_window->show_all();
		return;
	}

	if (filename.find('.', filename.rfind('/')+1) == filename.npos)
		filename += ".xml";

	do_file_save(filename);
}

void appObj::do_file_save(const std::string &filename)
{
	theme.get()->readlock()->save_file(filename, true);

	themename=filename;
	edited=false;
	update_title();
}

void appObj::file_quit_event()
{
	eventqueue->event([]
			  (const app &a)
			  {
				  a->running=false;
			  });
}
