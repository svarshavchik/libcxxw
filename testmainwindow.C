/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "x/w/main_window.H"
#include "x/w/screen.H"
#include <x/destroycallbackflag.H>

#include <string>
#include <iostream>
void testmainwindow()
{
	x::destroyCallbackFlag::base::guard guard;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base::create();

	guard(main_window->get_screen()->mcguffin());

	main_window->show();


	std::string s;
	std::getline(std::cin, s);
}

int main()
{
	try {
		testmainwindow();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
