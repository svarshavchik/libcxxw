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
	//
	// Use x::w::inactive_shortcut to specify the default keyboard
	// shortcuts, to avoid interfering with the default implementation
	// of Ctrl-Ins/Shift-Ins/Shift-Del key commands, so that they
	// continue to work normally in their input field, instead of
	// executing the menu action (although this won't do much harm).

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
		  x::w::inactive_shortcut{"Ctrl-Ins"},
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
		  x::w::inactive_shortcut{"Shift-Del"},
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
		  x::w::inactive_shortcut{"Shift-Ins"},
		  "Paste"
		});
}

// Helper object for storing the popup menu
//
// A popup menu goes away when the last reference to it get destroyed.
// In order for the popup menu to be visible, a reference to it must exist,
// somewhere.
//
// The install_contextpopup_callback() callback captures a reference to this
// helper object. When the callback gets invoked, it creates the popup menu
// container and stores it here, then shows it. Even after the callback
// returns, because the reference to the popup menu containe remains here,
// it remains visible.
//
// We'll also remove the reference to the popup menu container when it gets
// closed, destroying all memory used by it. The same approach can be used
// with multiple fields, avoiding having to use up memory for multiple copies
// of the same basic context popup menu, only creating one when it needs to
// be shown, then getting rid of it when it is no longer needed.

class contextpopup_containerObj : virtual public x::obj {

public:

	// This container is accessed only IN_THREAD, so we don't need to
	// make it thread-safe and protected by a mutex.

	x::w::containerptr menu;
};

// A normal ref for a contextpopup_containerObj
typedef x::ref<contextpopup_containerObj> contextpopup_container;

// A weak reference to a contextpopup_container
typedef x::weakptr<x::ptr<contextpopup_containerObj>
		   > weak_contextpopup_containerptr;

// Context popup menu callback. install_contextpopup_callback() for the
// input field installs a callback that invokes this function.
//
// menu_container is the container holder, that's captured by the actual
// callback lambda, so it exists as long as the callback is installed.
//
// ifield is the input field this callback is for.
//
// This gets invoked when pointer button #3 get clicked on top of the input
// field, so:

static void create_context_menu(ONLY IN_THREAD,
				const contextpopup_container &menu_container,
				const x::w::input_field &ifield)
{
	// Create the right button context menu popup for
	// the input field.
	x::w::container context_popup=
		ifield->create_popup_menu
		([&]
		 (const x::w::listlayoutmanager &llm)
		 {
			 // Add some custom menu items here,
			 // before the standard Copy/Cut/Paste
			 // items. An input field's
			 // create_copy_cut_paste_popup_menu_items()
			 // adds Cut, Copy, and Paste items to the popup
			 // menu, after any existing items. In this example,
			 // there's nothing.
			 ifield->create_copy_cut_paste_popup_menu_items
				 (IN_THREAD, llm);

			 // Or, any custom menu items can be added here.
		 });

	// Before showing the menu, enable/disable the
	// Copy/Cut/Paste items in the menu. The third parameter is the
	// starting index position of the menu items that
	// create_copy_cut_paste_popup_menu_items() created. This is always
	// llm->size() before create_copy_cut_paste_popup_menu_items()
	// got called, basically. Since there were no existing items, this is
	// 0.

	ifield->update_copy_cut_paste_popup_menu_items
		(IN_THREAD, context_popup, 0);

	context_popup->show_all(IN_THREAD);

	// Store a reference to the new context popup menu
	// here, so it doesn't go out of scope, get destroyed
	// and immediately disappear...
	menu_container->menu=context_popup;

	// ... but when the popup menu gets closed, we
	// can destroy the context_popup by clearing
	// the menu->menu reference.
	//
	// It is necessary for this callback to capture
	// context_popup weakly, to avoid a circular
	// reference.

	context_popup->on_state_update
		([weak_menu=weak_contextpopup_containerptr{menu_container}]
		 (ONLY IN_THREAD,
		  const auto &state,
		  const auto &mcguffin)
		 {
			 // When the popup menu gets closed...
			 if (state.state_update != state.after_hiding)
				 return;

			 // ... recover the strong ref.
			 auto menu_container=weak_menu.getptr();

			 if (!menu_container)
				 return;

			 // And clear the context_popup.
			 menu_container->menu={};
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

	// This input field already has this right pointer button context
	// popup, but we'll go through the motions of creating one ourselves,
	// for demonstration purposes.

	ifield->install_contextpopup_callback
		([menu=contextpopup_container::create()]
		 (ONLY IN_THREAD,
		  const x::w::input_field &ifield,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 create_context_menu(IN_THREAD, menu, ifield);
		 });

	// The "Ok" button doesn't do anything.
	f->create_normal_button_with_label("Ok");

	return x::ref<my_appObj>::create(mw, file_menu, ifield);
}

void copycutpastemenu()
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
		copycutpastemenu();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
