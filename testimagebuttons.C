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
#include "x/w/image_button.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/label.H"
#include "x/w/radio_group.H"
#include "x/w/canvas.H"
#include <string>
#include <iostream>

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

static void create_mainwindow(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager layout=main_window->get_layoutmanager();

	static const char * const days_of_week[]={
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday"};

	std::vector<LIBCXX_NAMESPACE::w::image_button> days_of_week_checkboxes;

	for (auto day_of_week:days_of_week)
	{
		LIBCXX_NAMESPACE::w::gridfactory factory=
			layout->append_row();

		LIBCXX_NAMESPACE::w::image_button checkbox=
			factory->valign(LIBCXX_NAMESPACE::w::valign::middle)
			.create_checkbox();

		checkbox->on_activate([day_of_week]
				      (bool first_time, size_t flag,
				       const auto &ignore)
				      {
					      if (first_time)
						      return;

					      std::cout << day_of_week
							<< ": "
							<< (flag ? "":"not ")
							<< "checked"
							<< std::endl;
				      });
		days_of_week_checkboxes.push_back(checkbox);
		auto label=factory->right_padding(3)
			.create_label({day_of_week});

		label->label_for(checkbox);
	}

	LIBCXX_NAMESPACE::w::radio_group group=LIBCXX_NAMESPACE::w::radio_group::create();

	auto factory=layout->append_columns(0);
	LIBCXX_NAMESPACE::w::image_button
		train=factory->valign(LIBCXX_NAMESPACE::w::valign::middle)
		.create_radio(group);

	train->set_value(1);


	auto sunday=*days_of_week_checkboxes.begin();
	auto saturday=*--days_of_week_checkboxes.end();

	sunday->set_enabled(false);
	saturday->set_enabled(false);

	train->on_activate([saturday, sunday]
			   (bool first_time, size_t flag,
			    const auto &ignore)
			   {
				   if (first_time)
					   return;

				   std::cout << "Train: "
					     << (flag ? "":"not ")
					     << "checked"
					     << std::endl;
				   sunday->set_enabled(!flag);
				   saturday->set_enabled(!flag);
			   });
	factory->create_label({"Train"});

	factory=layout->append_columns(1);
	LIBCXX_NAMESPACE::w::image_button
		bus=factory->valign(LIBCXX_NAMESPACE::w::valign::middle)
		.create_radio(group);
	bus->on_activate([]
			   (bool first_time, size_t flag,
			    const auto &ignore)
			   {
				   if (first_time)
					   return;

				   std::cout << "Bus: "
					     << (flag ? "":"not ")
					     << "checked"
					     << std::endl;
			   });

	factory->create_label({"Bus"});

	factory=layout->append_columns(2);
	LIBCXX_NAMESPACE::w::image_button
		drive=factory->valign(LIBCXX_NAMESPACE::w::valign::middle)
		.create_radio(group);
	drive->on_activate([]
			   (bool first_time, size_t flag,
			    const auto &ignore)
			   {
				   if (first_time)
					   return;

				   std::cout << "Drive: "
					     << (flag ? "":"not ")
					     << "checked"
					     << std::endl;
			   });
	factory->create_label({"Drive"});

	for (size_t i=3; i<7; ++i)
		layout->append_columns(i)->colspan(2)
			.create_canvas();

	// layout->default_col_border(2, "thick_dashed_0%");
}

void testimagebuttons()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([]
			 (const auto &main_window)
			 {
				 create_mainwindow(main_window);
			 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	auto original_theme=main_window->get_screen()->get_connection()
		->current_theme();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

#if 1
	lock.wait_for(std::chrono::seconds(30),
		      [&] { return *lock; });
#else
	for (int i=0; i < 4 && !*lock; ++i)
	{
		lock.wait_for(std::chrono::seconds(1),
			      [&] { return *lock; });

		main_window->get_screen()->get_connection()
			->set_theme(original_theme.first,
				    (i % 2) ? 100:200);
	}
#endif
}

int main(int argc, char **argv)
{
	try {
		testimagebuttons();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}