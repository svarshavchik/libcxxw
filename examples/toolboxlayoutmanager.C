/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/weakcapture.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/singletonptr.H>
#include <x/config.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/dialog.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/button.H>
#include <x/w/canvas.H>
#include <x/w/toolboxlayoutmanager.H>
#include <x/w/toolboxfactory.H>
#include <x/w/image_button.H>
#include <x/w/menubarlayoutmanager.H>
#include <x/w/menubarfactory.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/shortcut.H>
#include <x/w/focus_border_appearance.H>
#include <string>
#include <iostream>
#include <sstream>

#include "close_flag.H"

std::string x::appid() noexcept
{
	return "toolboxlayoutmanager.examples.w.libcxx.com";
}

// Singleton application object.

class appObj : virtual public x::obj {

public:

	// The main window
	const x::w::main_window main_window;

	// The dialog that uses the toolbox layout manager.
	const x::w::dialog toolbox_dialog;

	// Close application flag.
	const close_flag_ref close_flag;

	appObj(const x::w::main_window &main_window,
	       const x::w::dialog &toolbox_dialog)
		: main_window{main_window},
		  toolbox_dialog{toolbox_dialog},
		  close_flag{close_flag_ref::create()}
	{
	}

	~appObj()=default;

	// View/Toolbox menu item selection callback.
	void view_toolbox(const x::w::list_item_status_info_t &s)
	{
		// The callback gets invoked for several reasons, including
		// selection status, but we are interested only in reasons
		// caused by the menu item getting selected, by keyboard or
		// button.
		switch (s.trigger.index()) {
		case x::w::callback_trigger_key_event:
		case x::w::callback_trigger_button_event:
			break;
		default:
			return;
		}

		auto toolbox_dialog_window=toolbox_dialog->dialog_window;

		// We'll assume that this item's selection status indicates
		// whether the toolbox is visible or not, so we'll show and
		// hide it accordingly. We don't update the selection status
		// here, this is toolbox_visible()'s job.
		//
		// There are other ways the toolbox dialog can get closed,
		// such as the dialog's close button. Our job here is to
		// open or close the dialog itself, and toolbox_visible()
		// keeps track of this menu item's selection status, no
		// matter how the toolbox got opened or closed.

		if (s.layout_manager->selected(s.item_number))
		{
			toolbox_dialog_window->hide();
			return;
		}

		// If the dialog is not currently visible, reset its position
		// to the default position on the left of the main window,
		// before showing it.
		//
		// set_dialog_position() only works when the dialog is not
		// visible.
		toolbox_dialog->set_dialog_position
			(x::w::dialog_position::on_the_left);
		toolbox_dialog_window->show_all();
	}

	void toolbox_visible(bool flag)
	{
		// The toolbox dialog is now hidden or shown. "File" menu is
		// menu #0, "View" menu is menu #1, and its item #0 is the
		// "Toolbox" item. Update its selection status accordingly.

		auto view_menu=main_window->get_menubarlayoutmanager()
			->get_menu(1)->listlayout();

		view_menu->selected(0, flag);
	}
};

// Newly created application
typedef x::ref<appObj> new_app;

// The application singleton.

typedef x::singletonptr<appObj> app;

// The creator for the toolbox dialog.
static void create_toolbox_contents(const x::w::toolboxlayoutmanager &tlm)
{
	// Initialize the contents of the toolbox container.

	auto f=tlm->append_tools();

	// The toolbox icons are really radio buttons. We'll create 8 of them.

	for (size_t i=0; i<8; ++i)
	{
		// Borrow icon images from the default theme. They're all
		// the same size.
		//
		// Each icon is two images: clicked and unclicked.

		static const char *icons[][2]=
			{
			 {"scroll-left1", "scroll-left2"},
			 {"scroll-right1", "scroll-right2"},
			 {"scroll-up1", "scroll-up2"},
			 {"scroll-down1", "scroll-down2"},
			};

		auto icon_set=icons[i % (sizeof(icons)/sizeof(icons[0]))];

		// modify() the default radio button theme.
		//
		// radio_theme() returns a constant, cached object of the
		// appearance scheme for radio buttons. Use modify() to make
		// a copy of the theme object.

		x::w::const_image_button_appearance custom=
			x::w::image_button_appearance::base::radio_theme()
			->modify
			([&]
			 (const x::w::image_button_appearance &custom)
			 {
				 // Bonus for image_button_appearance-s only:
				 // replace the invisible border when there's
				 // no focus with one that's visually visible.

				 custom->focus_border=
					 x::w::focus_border_appearance
					 ::base::visible_thin_theme();

				 // And replace the images with our custom ones:
				 custom->images={icon_set[0], icon_set[1]};
			 });

		// And pass the custom appearance as an additional parameter
		// to create_radio().

		auto b=f->create_radio("toolboxradiogroup@toolboxlayoutmanager.examples.w.libcxx.com",
				       [](const auto &f) {},
				       custom);

		// Install a callback that just prints a message on the
		// console, when the radio button gets selected.
		b->on_activate
			([i]
			 (THREAD_CALLBACK,
			  size_t n,
			  const auto &trigger,
			  const auto &mcguffin)
			 {
				 // Ignore the initial callback invocation.
				 if (std::holds_alternative<x::w::initial>
				     (trigger))
					 return;

				 if (n == 0)
					 return; // Deselected

				 std::cout << "Tool "
					   << (i+1)
					   << std::endl;
			 });
	}
}

// Creator for the main window.

static void create_main_window(const x::w::main_window &mw,
			       x::w::dialogptr &toolbox_dialog)
{
	auto glm=mw->gridlayout();

	// Create a canvas element, to give the window some size.
	x::w::canvas_config config;

	config.width={50, 100, 150};
	config.height={50, 100, 150};

	glm->append_row()->create_canvas(config);

	// The layout manager for the new toolbox.
	x::w::new_toolboxlayoutmanager dialog_lm;

	// Default number of columns is 2. This is the default value.
	dialog_lm.default_width=2;

	// Creating a dialog with this unique identifier.
	x::w::create_dialog_args
		args{"toolbox_dialog1.toolboxlayoutmanager.examples.w.libcxx.com"};

	// Initial position of this dialog is to the left of the main window.
	args.restore(x::w::dialog_position::on_the_left);

	// The new dialog's layout manager is th etoolbox layout manager.
	args.dialog_layout=dialog_lm;

	// The toolbox dialog will not grab input focus when shown. Some
	// window managers may not need this when specifying a "toolbar" for
	// set_window_type(), below.
	args.grab_input_focus=false;

	auto d=mw->create_dialog
		(args,
		 []
		 (const x::w::dialog &d)
		 {
			 create_toolbox_contents(d->dialog_window
						 ->toolboxlayout());
		 });

	// Set the X window type. Most window manager
	// probably ignore this, but we'll go ahead and do this.

	d->dialog_window->set_window_type("toolbar,normal");

	// If the window manager gives the dialog a close button, have its
	// action be to close the dialog.
	d->dialog_window->on_delete
		([]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 app my_app;

			 if (!my_app)
				 return;

			 my_app->toolbox_dialog->dialog_window->hide();
		 });

	// Attach a state update callback to the dialog, to report when it's
	// shown or hidden to toolbox_visible(). This updates the selection
	// status of View/Toolbox.

	d->dialog_window->on_state_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::element_state &s,
		  const x::w::busy &mcguffin)
		 {
			 app my_app;

			 if (!my_app)
				 return;

			 if (s.state_update == s.after_hiding)
				 my_app->toolbox_visible(false);

			 if (s.state_update == s.after_showing)
				 my_app->toolbox_visible(true);
		 });

	// Application menu.
	auto new_menubar=mw->get_menubarlayoutmanager()->append_menus();

	// "File" menu, with just a "Quit" option.

	new_menubar->add_text
		("File",
		 []
		 (const x::w::listlayoutmanager &lm)
		 {
			 lm->append_items({
					   x::w::shortcut{"Alt",'Q'},
					   []
					   (THREAD_CALLBACK,
					    const auto &ignore)
					   {
						   app my_app;

						   if (!my_app)
							   return;
						   my_app->close_flag->close();
					   },
					   "Quit"});

		 });

	// The "View" menu with the "Toolbox" option.

	new_menubar->add_text
		("View",
		 []
		 (const x::w::listlayoutmanager &lm)
		 {
			 lm->append_items
				 ({
				   x::w::shortcut{"Alt",'T'},
				   []
				   (THREAD_CALLBACK,
				    const auto &status)
				   {
					   app my_app;

					   if (!my_app)
						   return;

					   my_app->view_toolbox(status);
				   },
				   "Toolbox"});

		 });

	mw->get_menubar()->show();

	// When the main window becomes visible and its position and size
	// are stable, show the dialog immediately afterwards.
	mw->on_stabilized
		([]
		 (THREAD_CALLBACK,
		  const x::w::busy &mcguffin)
		 {
			 app my_app;

			 if (!my_app)
				 return;

			 my_app->toolbox_dialog->dialog_window->show_all();
		 });

	toolbox_dialog=d;
}

// Create the new application object, optionally restoring previously-
// used position on the screen.

new_app create_app()
{
	x::w::dialogptr toolbox_dialog;

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window,
						    toolbox_dialog);
			 });

	main_window->set_window_title("Toolbox");

	return new_app::create(main_window, toolbox_dialog);
}

void testtoolbox()
{
	x::destroy_callback::base::guard guard;

	new_app my_app=create_app();

	guard(my_app->main_window->connection_mcguffin());

	my_app->main_window->on_disconnect([]
					   {
						   exit(1);
					   });

	my_app->main_window->on_delete
		([]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 app my_app;

			 if (my_app)
				 my_app->close_flag->close();
		 });

	app created_app{my_app};

	my_app->main_window->show_all();

	x::mpcobj<bool>::lock
		lock{my_app->close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		testtoolbox();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
