/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/button.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/text_param.H>
#include <x/w/text_param_literals.H>

#include <x/w/file_dialog.H>
#include <x/w/print_dialog.H>
#include <x/mpobj.H>
#include <x/singletonptr.H>
#include <x/cups/job.H>

#include "close_flag.H"

#include <string>
#include <iostream>

std::string x::appid() noexcept
{
	return "printdialog.examples.w.libcxx.com";
}

// Our "application" object.

class my_appObj : virtual public x::obj {

public:

	// The main application window
	const x::w::main_window main_window;

	// The file_dialog used to select a file to print.

	const x::w::file_dialog select_file_dialog;

	// And the print dialog

	const x::w::print_dialog print_dialog;

	// Once the file dialog selects the file we save it and open the
	// print dialog. Once the print dialog's "Print" button gets clicked
	// we retrieve the filename and print it.

	// Currently there's no possibility for this std::string to be
	// accessed concurrently by multiple threads, and everything gets
	// correctly sequenced. However, let's not rely on that, and wrap it
	// with an mpobj.

	x::mpobj<std::string> selected_file;

	my_appObj(const x::w::main_window &main_window);

};

// The application object will be a singleton object.

typedef x::singletonptr<my_appObj> my_app;

static x::w::file_dialog create_select_file_dialog(const x::w::main_window &main_window)
{
	// The "Print" button in the main window shows the file dialog, first,
	// to select the file to print.

	x::w::file_dialog_config config{
		[](ONLY IN_THREAD,
		   const x::w::file_dialog &this_dialog,
		   const std::string &filename,
		   const x::w::busy &ignored)
		{
			// It's our responsibility to close the dialog.

			this_dialog->dialog_window->hide();

			my_app app;

			if (!app)
				return;

			// Need to remember the filename.

			app->selected_file=filename;
			app->print_dialog->initial_show();
		},

		[](ONLY IN_THREAD,
		   const auto &ignore)
		{
			std::cout << "File dialog cancelled." << std::endl;
		}};

	auto d=main_window->create_file_dialog
		({"file@printdialog.examples.w.libcxx.com", true},
		 config);

	d->dialog_window->set_window_title("Select the file to print");

	return d;
}

static x::w::print_dialog create_print_dialog(const x::w::main_window &main_window)
{
	// Once the file gets selected, the print dialog gets opened. Here
	// the print dialog object gets created in advance.

	x::w::print_dialog_config config{
		[]
		(const x::w::print_callback_info &info)
		{
			// The "Print" button was selected. The print dialog
			// takes care of closing itself. Our job is to
			// submit the print job. The print dialog helpfully
			// gives us the x::cups::job object, with all the
			// selected options set.
			//
			// All we have to do is specify the file to print
			// and submit the print job.

			my_app app;

			if (!app)
				return;

			info.job->add_document_file("file",
						    app->selected_file.get());

			std::ostringstream o;

			o << "Created print job "
			  << info.job->submit("printdialog");

			// This is not really an error message, I'm just too
			// lazy to formally create_ok_dialog().

			x::w::alert_message_config config;

			config.title="Job submitted";

			app->main_window->alert_message(o.str(), config);
		},
		[]
		(ONLY IN_THREAD)
		{
			std::cout << "Print dialog cancelled." << std::endl;
		}};

	auto d=main_window->create_print_dialog
		({"print@printdialog.examples.w.libcxx.com", true},
		 config);
	d->dialog_window->set_window_title("Select the printer");

	return d;
}

my_appObj::my_appObj(const x::w::main_window &main_window)
	: main_window{main_window},
	  select_file_dialog{create_select_file_dialog(main_window)},
	  print_dialog{create_print_dialog(main_window)}
{
}

///////////////////////////////////////////////////////////////////////////
//
// main_window creator function. Just a print button, with ample margins
// around it.

static inline void initialize_mainwindow(const x::w::gridfactory &f)
{
	// The main window is just a button. With plenty of padding.
	auto b=f->padding(20).create_button
		({
			x::w::text_decoration::underline,
			"P",
			x::w::text_decoration::none,
			"rint",
		},
		x::w::shortcut{"Alt-P"});

	// This is the only display element in the main window. Might as
	// well give it focus automatically.

	b->autofocus(true);

	// The button's callback opens the file dialog.

	b->on_activate([]
		       (ONLY IN_THREAD,
			const x::w::callback_trigger_t &trigger,
			const x::w::busy &ignore)
		       {
			       my_app app;

			       if (!app)
				       return;

			       app->select_file_dialog->dialog_window
				       ->show_all();
		       });
}

void testprintdialog()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::ptr<my_appObj> app_ptr;

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 auto layout=main_window->gridlayout();

				 x::w::gridfactory factory=
					 layout->append_row();

				 initialize_mainwindow(factory);

				 app_ptr=x::ref<my_appObj>::create(main_window);
			 });

	main_window->set_window_title("Print something!");

	// Construct the application singleton object.
	//
	// This object holds additional references to the top level window and
	// its dialogs. This singleton object gets constructed in automatic
	// scope, so it goes out of scope and gets destroyed before
	// the main_window, and before the destructor guard, so all these
	// references will go away in an orderly fashion.

	my_app app{app_ptr};

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	x::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		testprintdialog();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
