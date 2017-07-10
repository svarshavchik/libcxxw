/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/button.H"
#include "x/w/canvas.H"
#include "x/w/custom_comboboxlayoutmanager.H"
#include "x/w/focusable_label.H"
#include <string>
#include <iostream>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

class close_flagObj : public obj {

public:
	mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef ref<close_flagObj> close_flag_ref;


void testcombobox()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window
		::create([&]
			 (const auto &main_window)
			 {
				 gridlayoutmanager
				     layout=main_window->get_layoutmanager();

				 static const char *text[]={
					 "Lorem ipsum",
					 "dolor sit",
					 "ament",
					 "consectetur",
					 "adipisicing",
					 "elid set",
					 "do",
					 "eiusmod tempor",
				 };

				 new_custom_comboboxlayoutmanager clm
				 {[] (const auto &factory)
					 {
						 factory->create_focusable_label
							 ("");
					 },
				  [&]
					  (const x::w::list_lock &lock,
					   const x::w::listlayoutmanager
					   &combobox_listlayoutmanager,
					   size_t i,
					   bool selected,
					   x::w::focusable_label element,
					   const x::w::element &popup_element,
					   const x::w::busy &mcguffin) {
					  if (selected)
					  {
						  element->update(text[i]);
						  popup_element->hide();
					  }
				  }};


				 auto factory=layout->append_row();

				 factory->create_focusable_container
				 ([&]
				  (const focusable_container &c) {
					 x::w::custom_comboboxlayoutmanager
						 lm=c->get_layoutmanager();

					 for (const auto &t:text)
						 lm->append_item(t);
				 }, clm)->show();
			 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		testcombobox();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
