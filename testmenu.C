/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/mpobj.H>
#include <x/weakcapture.H>
#include <x/singletonptr.H>
#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/menu.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "x/w/dialog.H"
#include "x/w/input_dialog.H"
#include "x/w/file_dialog.H"
#include "x/w/file_dialog_config.H"
#include "x/w/file_dialog_appearance.H"
#include "x/w/list_appearance.H"
#include "x/w/popup_list_appearance.H"
#include "x/w/print_dialog.H"
#include "x/w/print_dialog_config.H"
#include "x/w/image.H"
#include "x/w/shortcut.H"
#include <x/locale.H>
#include <x/cups/job.H>
#include <string>
#include <iostream>
#include <sstream>

class close_flagObj : public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef LIBCXX_NAMESPACE::ref<close_flagObj> close_flag_ref;

class app_dialogsObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	typedef LIBCXX_NAMESPACE::mpobj<std::function<
						void (const std::string &)>
					> on_file_open_t;

	on_file_open_t on_file_open;

	template<typename F>
	void open_file_open(const std::string &title, F &&f)
	{
		on_file_open=std::forward<F>(f);

		file_open->dialog_window->set_window_title(title);
		file_open->dialog_window->show_all();
	}

	void do_file_open(const std::string &name)
	{
		on_file_open_t::lock lock{on_file_open};

		(*lock)(name);
	}

	typedef LIBCXX_NAMESPACE::mpobj<
		std::function<void (const LIBCXX_NAMESPACE::w
				    ::print_callback_info &)>
		> on_file_print_t;

	on_file_print_t on_file_print;

	template<typename F>
	void open_file_print(const std::string &title, F &&f)
	{
		on_file_print=std::forward<F>(f);

		file_print->dialog_window->set_window_title(title);
		file_print->initial_show();
	}

	void do_file_print(const LIBCXX_NAMESPACE::w::print_callback_info &info)
	{
		on_file_print_t::lock lock{on_file_print};

		(*lock)(info);
	}

	LIBCXX_NAMESPACE::w::input_dialog help_question;
	LIBCXX_NAMESPACE::w::dialog help_about;
	LIBCXX_NAMESPACE::w::file_dialog file_open;
	LIBCXX_NAMESPACE::w::print_dialog file_print;

	app_dialogsObj(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::input_dialog create_help_question(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::dialog create_help_about(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::file_dialog create_file_open(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::print_dialog create_file_print(const LIBCXX_NAMESPACE::w::main_window &main_window);
};

typedef LIBCXX_NAMESPACE::singletonptr<app_dialogsObj> app_dialogs;

app_dialogsObj::app_dialogsObj(const LIBCXX_NAMESPACE::w::main_window &main_window)
	: help_question(create_help_question(main_window)),
	  help_about(create_help_about(main_window)),
	  file_open(create_file_open(main_window)),
	  file_print(create_file_print(main_window))
{
}

LIBCXX_NAMESPACE::w::input_dialog
app_dialogsObj::create_help_question(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto d=main_window->create_input_dialog
		({"question", true},
		 "question",
		 []
		 (const auto &factory)
		 {
			 factory->create_label("What is your name?");
		 },
		 "", // Initial text
		 {}, // input_field_config
		 []
		 (THREAD_CALLBACK, const auto &info)
		 {
			 LIBCXX_NAMESPACE::w::input_lock
				 lock{info.dialog_input_field};

			 std::cout << "Your name is " << lock.get()<< std::endl;
		 },
		 []
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 std::cout << "Never mind!" << std::endl;
		 });
	d->dialog_window->set_window_title("Hello!");

	return d;
}

LIBCXX_NAMESPACE::w::dialog
app_dialogsObj::create_help_about(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto d=main_window->create_ok_cancel_dialog
		({"alert", true},
		 "alert",
		 []
		 (const auto &factory)
		 {
			 factory->create_label("Help -> About!");
		 },
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
			 std::cout << "Help -> About Ok!" << std::endl;
		 },
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
			 std::cout << "Help -> About Cancel!" << std::endl;
		 });
	d->dialog_window->set_window_title("Help/About");

	return d;
}

LIBCXX_NAMESPACE::w::file_dialog
app_dialogsObj::create_file_open(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	LIBCXX_NAMESPACE::w::file_dialog_config config{
		[](ONLY IN_THREAD,
		   const auto &d,
		   const std::string &name,
		   const auto &busy_mcguffin)
		{
			d->dialog_window->hide();

			app_dialogs all_app_dialogs;

			if (!all_app_dialogs)
				return;

			all_app_dialogs->do_file_open(name);
		},
		[](ONLY IN_THREAD, const auto &busy_mcguffin)
		{
			std::cout << "File open cancelled" << std::endl;
		}
	};

	config.filename_filters.emplace_back
		("Text files", "\\.txt$");
	config.filename_filters.emplace_back
		("Image files", "\\.(gif|png|jpg)$");
	config.initial_filename_filter=0;

#if 0
	auto custom=config.appearance->clone();

	custom->filedir_filesize_font=custom->filedir_filename_font;

	{
		auto custom_dir=custom->dir_pane_appearance->clone();

		custom_dir->background_color="70%";

		custom->dir_pane_appearance=custom_dir;
	}
	config.appearance=custom;
#endif
	auto d=main_window->create_file_dialog({"file_open", true}, config);

	return d;
}

LIBCXX_NAMESPACE::w::print_dialog
app_dialogsObj::create_file_print(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	LIBCXX_NAMESPACE::w::print_dialog_config config{

		[](const auto &info)
		{
			app_dialogs all_app_dialogs;

			if (!all_app_dialogs)
				return;

			all_app_dialogs->do_file_print(info);
		},
		[](ONLY IN_THREAD)
		{
			std::cout << "Cancelled" << std::endl;
		}};

	auto d=main_window->create_print_dialog({"file_print", true}, config);

	d->dialog_window->set_window_title("Print File");

	return d;
}

void remove_help_menu(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto lm=main_window->get_menubarlayoutmanager();

	LIBCXX_NAMESPACE::w::menubar_lock lock{lm};

	if (lock.right_menus())
		lm->remove_right_menu(0);
}

void remove_view_menu(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto lm=main_window->get_menubarlayoutmanager();

	LIBCXX_NAMESPACE::w::menubar_lock lock{lm};

	if (lock.menus() > 1)
		lm->remove_menu(1);
}

void add_recent(const LIBCXX_NAMESPACE::w::main_window &main_window,
		int n)
{
	auto lm=main_window->get_menubarlayoutmanager();

	std::ostringstream o;

	o << "Recent submenu #" << n;

	auto s=o.str();

	LIBCXX_NAMESPACE::w::menubar_lock lock{lm};

	lock.get_menu(0)->get_layoutmanager()->get_item_layoutmanager(6)
		->append_items
		({
			[s](THREAD_CALLBACK, const auto &info)
			{
				std::cout << "YAY:" << s << std::endl;
			},
			LIBCXX_NAMESPACE::w::list_item_param{s}});
}

static inline void file_print_selected()
{
	app_dialogs all_app_dialogs;

	if (!all_app_dialogs)
		return;

	all_app_dialogs->open_file_open
		("Select file to print",
		 []
		 (const std::string &file)
		 {
			 app_dialogs all_app_dialogs;

			 if (!all_app_dialogs)
				 return;

			 all_app_dialogs->open_file_print
				 ("Print file",
				  [file]
				  (const auto &info)
				  {
					  info.job->add_document_file("File",
								      file);
					  info.job->submit("testmenu");
				  });
		 });
}

void file_menu(const LIBCXX_NAMESPACE::w::main_window &main_window,
	       const LIBCXX_NAMESPACE::w::listlayoutmanager &m,
	       const LIBCXX_NAMESPACE::w::menu &view_menu,
	       size_t view_options_item)
{
	m->append_items({
			LIBCXX_NAMESPACE::w::shortcut{"Alt", 'N'},

			[](THREAD_CALLBACK, const auto &ignore)
			{
				app_dialogs all_app_dialogs;

				if (all_app_dialogs)
					all_app_dialogs->open_file_open
						("New File",
						 []
						 (const std::string &name)
						 {
							 std::cout << "New: "
								   << name
								   << std::endl;
						 });
			},
			"New",
			LIBCXX_NAMESPACE::w::shortcut{"Alt", 'O'},
			[](THREAD_CALLBACK, const auto &ignore)
			{
				app_dialogs all_app_dialogs;

				if (all_app_dialogs)
					all_app_dialogs->open_file_open
						("Open File",
						 []
						 (const std::string &name)
						 {
							 std::cout << "Open: "
								   << name
								   << std::endl;
						 });
			},
			"Open",
			[](THREAD_CALLBACK, const auto &ignore)
			{
				std::cout << "File->Close selected"
					  << std::endl;
			},
			"Close",
			LIBCXX_NAMESPACE::w::shortcut{"Alt", 'P'},
			[](THREAD_CALLBACK, const auto &ignore)
			{
				file_print_selected();
			},
			"Print",

			LIBCXX_NAMESPACE::w::separator{},

			[=](THREAD_CALLBACK, const auto &ignore)
			{
				auto l=view_menu->get_layoutmanager();

				l->enabled(view_options_item,
					   !l->enabled(view_options_item));
			},

			"Toggle Options",

			LIBCXX_NAMESPACE::w::submenu{
				[](const LIBCXX_NAMESPACE::w::listlayoutmanager
				   &recent_menu)
				{
					for (size_t i=1; i <= 4; ++i)
					{
						std::ostringstream o;

						o << "Recent submenu #" << i;

						auto s=o.str();

						recent_menu->append_items
							({LIBCXX_NAMESPACE::w::list_item_param{
									[s](THREAD_CALLBACK,
									    const auto &)
									{
										std::cout << s
											  << std::endl;
									}
								}, LIBCXX_NAMESPACE::w::list_item_param{
							 s}});
					}
				},
				LIBCXX_NAMESPACE::w::popup_list_appearance
				::base::submenu_theme(),
			},
			"Recent",


			LIBCXX_NAMESPACE::w::submenu{
				[](const LIBCXX_NAMESPACE::w::listlayoutmanager
				   &m)
				{
					m->append_items({"Lorem",
							 "Ipsum",
							 "Dolor",
							 "Sit",
							 "Amet"});
				}},
			"Another submenu",

			[=](THREAD_CALLBACK, const auto &ignore)
			{
				std::cout << "File->Quit selected" << std::endl;
			},
			"Quit",
			[main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)]
				(THREAD_CALLBACK, const auto &ignore)
			{
				auto got=main_window.get();

				if (got)
				{
					auto &[main_window]=*got;

					remove_help_menu(main_window);
				}
			},
			"Remove Help menu",
			[main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)]
				(THREAD_CALLBACK, const auto &ignore)
			{
				auto got=main_window.get();

				if (got)
				{
					auto &[main_window]=*got;

					remove_view_menu(main_window);
				}
			},
			"Remove View menu",

			[i=4, main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)]
				(THREAD_CALLBACK, const auto &ignore)
				mutable
			{
				auto got=main_window.get();

				if (got)
				{
					auto &[main_window]=*got;

					add_recent(main_window, ++i);
				}
			},
			"Add to recent submenu"});
}

size_t view_menu(const LIBCXX_NAMESPACE::w::listlayoutmanager &m)
{
	m->replace_all_items({
		LIBCXX_NAMESPACE::w::menuoption{},
		[](THREAD_CALLBACK, const auto &info)
		{
			std::cout << "View->Tools: " << info.selected
				  << std::endl;
		},

		"Tools",

		LIBCXX_NAMESPACE::w::menuoption{},
		[](THREAD_CALLBACK, const auto &info)
		{
			std::cout << "View->Options: " << info.selected
				  << std::endl;
		},
		"Options"
			});

	return 1;
}


void help_menu(const LIBCXX_NAMESPACE::w::main_window &main_window,
	       const LIBCXX_NAMESPACE::w::listlayoutmanager &m)
{
	m->insert_items(0, {
		       [](THREAD_CALLBACK, const auto &ignore)
		       {
			       app_dialogs all_app_dialogs;

			       if (all_app_dialogs)
			       {
				       all_app_dialogs->help_question
					       ->input_dialog_field
					       ->set("");
				       all_app_dialogs->help_question
					       ->dialog_window->show_all();
			       }
		       },
		       "Question",

		       LIBCXX_NAMESPACE::w::shortcut{"F1"},

		       [](THREAD_CALLBACK, const auto &ignore)
		       {
			       app_dialogs all_app_dialogs;

			       if (all_app_dialogs)
				       all_app_dialogs->help_about
					       ->dialog_window->show_all();
		       },
		       "About"
			       });
}

void make_context_menu(const LIBCXX_NAMESPACE::w::listlayoutmanager &m)
{
	m->append_items
		({
			[]
			(THREAD_CALLBACK, const auto &ignore)
			{
				std::cout << "Help selected" << std::endl;
			},
			"Help",
			[]
			(THREAD_CALLBACK, const auto &ignore)
			{
				std::cout << "About selected" << std::endl;
			},
			"About",
				});
}

void testmenu()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window::create
		([]
		 (const auto &main_window)
		 {
			 LIBCXX_NAMESPACE::w::gridlayoutmanager
			 layout=main_window->get_layoutmanager();
			 LIBCXX_NAMESPACE::w::gridfactory factory=
			 layout->append_row();

			 auto i=factory->create_image("docbook/menu.png");

			 auto context_menu=i->create_popup_menu
			 ([]
			  (const auto &lm) {
				 make_context_menu(lm);
			 });

			 i->install_contextpopup_callback
			 ([context_menu]
			  (THREAD_CALLBACK,
			   const auto &e,
			   const auto &t,
			   const auto &m) {
				 context_menu->show();
			 }, {"F3"});

			 auto mb=main_window->get_menubarlayoutmanager();

			 LIBCXX_NAMESPACE::w::menubar_lock mbl{mb};

			 auto f=mb->append_menus();

			 size_t options_menu_item;

			 LIBCXX_NAMESPACE::w::menu view_m=f->add([]
								 (const auto &factory) {
									 factory->create_label("View");
								 },
								 [&]
								 (const auto &factory) {
									 options_menu_item=view_menu(factory);
								 });

			 f=mb->insert_menus(0);

			 auto file_m=f->add_text
				 ("File",
				  [&]
				  (const auto &factory) {
					  file_menu(main_window,
						    factory,
						    view_m,
						    options_menu_item);
				  });

			 if (mbl.get_menu(0) != file_m ||
			     mbl.get_menu(1) != view_m)
				 abort();

			 if (getpid() & 1)
				 f=mb->append_right_menus();
			 else
				 f=mb->insert_right_menus(0);
			 auto help_m=f->add_text
				 ("Help",
				  [&]
				  (const auto &factory) {
					  help_menu(main_window,
						    factory);
				  });

			 if (help_m != mbl.get_right_menu(0))
				 abort();

			 main_window->get_menubar()->show();

			 LIBCXX_NAMESPACE::w::menubar_lock lock{mb};

			 std::cout << lock.menus() << std::endl;
			 std::cout << lock.right_menus() << std::endl;
		 });

	main_window->set_window_title("Hello world!");
	main_window->set_window_class("main", "testmenu@examples.w.libcxx.com");

	guard(main_window->connection_mcguffin());

	app_dialogs all_app_dialogs{
		LIBCXX_NAMESPACE::ref<app_dialogsObj>::create(main_window)
			};


	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::locale::base::environment()->global();

		testmenu();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
