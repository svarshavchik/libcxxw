/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/factory.H>
#include <x/w/label.H>
#include <x/w/input_field.H>
#include <x/w/button.H>
#include <x/w/menubarlayoutmanager.H>
#include <x/w/menubarfactory.H>
#include <x/w/menu.H>
#include <x/w/listlayoutmanager.H>
#include <x/singletonptr.H>

#include "close_flag.H"

// Our "application" object.

class my_appObj : virtual public x::obj {

public:

	// The main application window
	const x::w::main_window main_window;

	// The "File" menu
	const x::w::menu file_menu;

	// The input field

	const x::w::input_field input_field;

	my_appObj(const x::w::main_window &main_window,
		  const x::w::menu &file_menu,
		  const x::w::input_field &input_field)
		: main_window{main_window},
		  file_menu{file_menu},
		  input_field{input_field}
	{
	}
};

typedef x::singletonptr<my_appObj> my_app;

// Populate the application "File" menu.

static inline void make_file_menu(const x::w::main_window &mw,
				  const x::w::listlayoutmanager &file_menu)
{
	// Cut, Copy, and Paste menu items.
	//
	// Any display element's cut_or_copy_selection() (for the cutting and
	// copying operation) and receive_selection() (for the paste operation)
	// execute the corresponding operation. These methods are available
	// from every display element, and they get executed in whichever
	// display element in the same window has keyboard focus, not
	// necessarily the same element whose method gets invoked.

	file_menu->append_items
		({
		  []
		  (ONLY IN_THREAD,
		   const x::w::list_item_status_info_t &status_info)
		  {
			  my_app app;

			  if (!app)
				  return;

			  app->main_window->cut_or_copy_selection
				  (IN_THREAD, x::w::cut_or_copy_op::copy);
		  },
		  "Copy",

		  []
		  (ONLY IN_THREAD,
		   const x::w::list_item_status_info_t &status_info)
		  {
			  my_app app;

			  if (!app)
				  return;

			  app->main_window->cut_or_copy_selection
				  (IN_THREAD, x::w::cut_or_copy_op::cut);
		  },
		  "Cut",

		  []
		  (ONLY IN_THREAD,
		   const x::w::list_item_status_info_t &status_info)
		  {
			  my_app app;

			  if (!app)
				  return;

			  app->main_window->receive_selection(IN_THREAD);
		  },
		  "Paste"
		});
}

// Populate the context popup menu that opens by right-clicking on the
// text input field.

static void create_context_menu(const x::w::listlayoutmanager &llm)
{
	// Cut/Copy/Paste menu items.

	// Focusable elements also have focusable_cut_or_copy_selection()
	// and focusable_receive_selection(), which are similar to
	// cut_or_copy_selection() and receive_selection(), but unlike them
	// they only take action if their focusable element has keyboard
	// input focus, and they get ignored otherwise.
	//
	// This the context popup menu gets shown only when the keyboard
	// focus is in the input field, this makes no material difference,
	// this is for demonstration purposes.
	//
	// Like with the regular popup menu, these menu items get enabled or
	// disabled depending upon whether their corresponding action can be
	// taken. This is done before showing the context menu.

	llm->append_items
		({
		  []
		  (ONLY IN_THREAD,
		   const x::w::list_item_status_info_t &status_info)
		  {
			  my_app app;

			  if (!app)
				  return;

			  app->input_field->focusable_cut_or_copy_selection
				  (IN_THREAD, x::w::cut_or_copy_op::copy);
		  },
		  "Copy",

		  []
		  (ONLY IN_THREAD,
		   const x::w::list_item_status_info_t &status_info)
		  {
			  my_app app;

			  if (!app)
				  return;

			  app->input_field->focusable_cut_or_copy_selection
				  (IN_THREAD, x::w::cut_or_copy_op::cut);
		  },
		  "Cut",

		  []
		  (ONLY IN_THREAD,
		   const x::w::list_item_status_info_t &status_info)
		  {
			  my_app app;

			  if (!app)
				  return;

			  app->input_field->focusable_receive_selection();
		  },
		  "Paste",
		});
}

x::ref<my_appObj> create_mainwindow(const x::w::main_window &mw)
{
	mw->set_window_title("Copy/Cut/Paste menus");
	auto mblm=mw->get_menubarlayoutmanager();
	auto mbf=mblm->append_menus();

	// Create the file menu.

	x::w::menu file_menu=mbf->add([]
				      (const auto &f)
				      {
					      f->create_label("File");
				      },
				      [&]
				      (const x::w::listlayoutmanager &lm)
				      {
					      make_file_menu(mw, lm);
				      });

	// Attach a callback that gets invoked before the "File" menu
	// becomes visible (or hidden). Use this callback to enable or
	// disable the Cut/Copy/Paste menu items depending upon whether the
	// given operation is possible at this time.
	//
	// Passing "x::w::cut_or_copy_op::available" to
	// cut_or_copy_selection() indicates whether the current element
	// with keyboard focus has something that's copyable or cuttable.
	//
	// selection_can_be_received() indicates whether the display element
	// with the keyboard focus accepts pasting text, and
	// selection_has_owner() indicates whether any window has something
	// to paste. Both must be true in order to enable the "Paste" option.
	// Like cut_or_copy_selection and receive_selection(),
	// selection_can_be_received() checks the display element with the
	// keyboard focus, not necessarily the same display element whose
	// selection_can_be_received() gets invoked.

	file_menu->on_popup_state_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::element_state &es,
		  const x::w::busy &mcguffin)
		 {
			 if (es.state_update != es.before_showing)
				 return;

			 my_app app;

			 if (!app)
				 return;

			 x::w::listlayoutmanager lm=
				 app->file_menu->get_layoutmanager();

			 bool has_cut_or_copy=
				 app->file_menu->cut_or_copy_selection
				 (IN_THREAD, x::w::cut_or_copy_op::available);

			 // Menu items are Copy, Cut, and Paste
			 lm->enabled(0, has_cut_or_copy);
			 lm->enabled(1, has_cut_or_copy);

			 lm->enabled(2, app->file_menu
				     ->selection_can_be_received() &&
				     app->file_menu->selection_has_owner());
		 });
	mw->get_menubar()->show();
	x::w::gridlayoutmanager layout=mw->get_layoutmanager();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory f=layout->append_row();

	f->create_label("Input Field:");

	x::w::input_field_config config{30};

	x::w::input_field ifield=f->create_input_field("", config);

	// Create the right button context menu popup for the input field.
	x::w::container context_popup=ifield->create_popup_menu
		([&]
		 (const x::w::listlayoutmanager &llm)
		 {
			 create_context_menu(llm);
		 });

	ifield->install_contextpopup_callback
		([context_popup]
		 (ONLY IN_THREAD,
		  const x::w::element &my_element,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 my_app app;

			 if (!app)
				 return;

			 // Determine whether cut, copy, and paste actions are
			 // possible before showing the popup menu.

			 x::w::listlayoutmanager l=
				 context_popup->get_layoutmanager();

			 bool cut_or_copy=app->input_field
				 ->focusable_cut_or_copy_selection
				 (IN_THREAD, x::w::cut_or_copy_op::available);

			 // Menu items are Copy, Cut, and Paste
			 l->enabled(IN_THREAD, 0, cut_or_copy);
			 l->enabled(IN_THREAD, 1, cut_or_copy);
			 l->enabled(IN_THREAD, 2,
				    app->input_field->selection_has_owner() &&
				    app->input_field
				    ->selection_can_be_received());
			 context_popup->show_all();
		 });

	// The "Ok" button doesn't do anything.
	f->create_normal_button_with_label("Ok");

	return x::ref<my_appObj>::create(mw, file_menu, ifield);
}

void testfocusrestore()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::ptr<my_appObj> created_app;

	auto mw=x::w::main_window::create
		([&]
		 (const x::w::main_window &mw)
		 {
			 created_app=create_mainwindow(mw);
		 });

	mw->on_disconnect([]
			  {
				  _exit(1);
			  });

	my_app app{created_app};

	guard(mw->connection_mcguffin());

	mw->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	mw->show_all();

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		testfocusrestore();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
