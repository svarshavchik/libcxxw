/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/appid.H>

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
#include <x/w/copy_cut_paste_menu_items.H>
#include <x/singletonptr.H>

#include "close_flag.H"

std::string x::appid() noexcept
{
	return "copycutpastemenu.examples.w.libcxx.com";
}

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
			 // We can add some custom menu items here,
			 // before the standard Copy/Cut/Paste
			 // items. An input field's
			 // create_copy_cut_paste_popup_menu_items()
			 // adds Cut, Copy, and Paste items to the popup
			 // menu, after any existing items. In this example,
			 // there's nothing.

			 llm->append_copy_cut_paste(IN_THREAD, ifield)

				 // There's no need to install an
				 // on_state_update(), in this case, and
				 // shuffle everything in there.
				 //
				 // This is called from
				 // install_contextpopup_callback(), and
				 // we're about to show_all(), so just
				 // call update() immediately.

				 ->update(IN_THREAD);

			 // Or, any custom menu items can be added here,
			 // after the Copy/Cut/Paste items.
		 });

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

	// append_copy_paste() returns an x::w::copy_cut_paste_menu_items
	// reference-counted object. We'll need to use it in the
	// on_popup_state_update() callback.

	x::w::copy_cut_paste_menu_itemsptr ccp;

	x::w::menu file_menu=
		mbf->add([]
			 (const auto &f)
			 {
				 f->create_label("File");
			 },
			 [&]
			 (const x::w::listlayoutmanager &lm)
			 {
				 ccp=lm->append_copy_cut_paste(mw);
			 });

	// Attach a callback that gets invoked before the "File" menu
	// becomes visible (or hidden). Use this callback to enable or
	// disable the Cut/Copy/Paste menu items depending upon whether the
	// given operation is possible at this time.

	file_menu->on_popup_state_update
		([ccp=x::w::copy_cut_paste_menu_items{ccp}]
		 (ONLY IN_THREAD,
		  const x::w::element_state &es,
		  const x::w::busy &mcguffin)
		 {
			 if (es.state_update != es.before_showing)
				 return;

			 ccp->update(IN_THREAD);
		 });

	mw->get_menubar()->show();
	auto layout=mw->gridlayout();

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
	f->create_button("Ok");

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
