/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/font_literals.H>
#include <x/w/input_field.H>
#include <x/w/input_field_lock.H>
#include <x/w/container.H>

#include <string>
#include <iostream>
#include <sstream>

x::w::input_field create_mainwindow(const x::w::main_window &main_window)
{
	x::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	factory->create_label("Password:");

	x::w::input_field_config password_conf{21};

	password_conf.maximum_size=20;
	password_conf.set_password();
	password_conf.alignment=x::w::halign::center;

	return factory->create_input_field("", password_conf);
}

void enterpassword()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::input_fieldptr password_field;

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 password_field=create_mainwindow(main_window);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Password!");
	main_window->set_window_class("main",
				      "passwordinput@examples.w.libcxx.com");
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	std::cout << "Password was: "
		  << x::w::input_lock{password_field}.get() << std::endl;

}

int main(int argc, char **argv)
{
	try {
		enterpassword();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
