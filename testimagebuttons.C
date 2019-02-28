/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/weakcapture.H>
#include <x/config.H>

#include "x/w/main_window.H"
#include "x/w/screen_positions.H"
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

static void set_train_label(const LIBCXX_NAMESPACE::w::factory &f,
			    bool selected)
{
	f->create_label(selected ? "Train (with weekends)":"Train")->show();
}

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

	int n=0;

	for (auto day_of_week:days_of_week)
	{
		++n;

		LIBCXX_NAMESPACE::w::gridfactory factory=
			layout->append_row();

		factory->right_padding(3);

		LIBCXX_NAMESPACE::w::image_button checkbox=
			factory->valign(LIBCXX_NAMESPACE::w::valign::middle)
			.create_checkbox([&]
					 (const auto &factory)
					 {
						 factory->create_label
						 (day_of_week)->show();
					 });

		if (n > 1 && n < 7)
			checkbox->set_value(2);

		checkbox->on_activate([day_of_week]
				      (THREAD_CALLBACK,
				       size_t flag,
				       const auto &trigger,
				       const auto &ignore)
				      {
					      if (trigger.index() ==
						  LIBCXX_NAMESPACE::w::callback_trigger_initial)
						      return;
					      std::cout << day_of_week
							<< ": "
							<< (flag ? "":"not ")
							<< "checked"
							<< std::endl;
				      });
		days_of_week_checkboxes.push_back(checkbox);
	}

	LIBCXX_NAMESPACE::w::radio_group group=LIBCXX_NAMESPACE::w::radio_group::create();

	auto factory=layout->append_columns(0);

	LIBCXX_NAMESPACE::w::image_button
		train=factory->valign(LIBCXX_NAMESPACE::w::valign::middle)
		.create_radio(group,
			      []
			      (const auto &factory)
			      {
				      set_train_label(factory, true);
			      });

	train->set_value(1);

	auto sunday=*days_of_week_checkboxes.begin();
	auto saturday=*--days_of_week_checkboxes.end();

	sunday->set_enabled(false);
	saturday->set_enabled(false);

	train->on_activate([saturday, sunday, train=LIBCXX_NAMESPACE::make_weak_capture(train)]
			   (THREAD_CALLBACK,
			    size_t flag,
			    const auto &trigger,
			    const auto &ignore)
			   {
				   if (trigger.index() ==
				       LIBCXX_NAMESPACE::w::callback_trigger_initial)
					   return;

				   std::cout << "Train: "
					     << (flag ? "":"not ")
					     << "checked"
					     << std::endl;
				   sunday->set_enabled(!flag);
				   saturday->set_enabled(!flag);

				   auto got=train.get();

				   if (got)
				   {
					   auto &[train]=*got;

					   train->update_label
						   ([&]
						    (const LIBCXX_NAMESPACE::w::factory &f)
						    {
							    set_train_label(f,
									    flag>0);
						    });
				   }
			   });

	factory=layout->append_columns(1);
	LIBCXX_NAMESPACE::w::image_button
		bus=factory->valign(LIBCXX_NAMESPACE::w::valign::middle)
		.create_radio(group,
			      []
			      (const auto &factory)
			      {
				      factory->create_label("Bus");
			      });

	bus->on_activate([]
			 (THREAD_CALLBACK,
			  size_t flag,
			  const auto &trigger,
			  const auto &ignore)
			 {
				 if (trigger.index() ==
				     LIBCXX_NAMESPACE::w::callback_trigger_initial)
					 return;

				 std::cout << "Bus: "
					   << (flag ? "":"not ")
					   << "checked"
					   << std::endl;
			 });

	factory=layout->append_columns(2);
	LIBCXX_NAMESPACE::w::image_button
		drive=factory->valign(LIBCXX_NAMESPACE::w::valign::middle)
		.create_radio(group,
			      []
			      (const auto &factory)
			      {
				      factory->create_label("Drive");
			      });

	drive->on_activate([]
			   (THREAD_CALLBACK,
			    size_t flag,
			    const auto &trigger,
			    const auto &ignore)
			   {
				   if (trigger.index() ==
				       LIBCXX_NAMESPACE::w::callback_trigger_initial)
					   return;

				   std::cout << "Drive: "
					     << (flag ? "":"not ")
					     << "checked"
					     << std::endl;
			   });

	for (size_t i=3; i<7; ++i)
		layout->append_columns(i)->create_canvas();

	// layout->default_col_border(2, "thick_dashed_0%");
}

void testimagebuttons()
{
	auto configfile=
		LIBCXX_NAMESPACE::configdir("testimagebuttons@libcxx.com")
		+ "/windows";
	LIBCXX_NAMESPACE::w::screen_positions pos{configfile};

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	LIBCXX_NAMESPACE::w::main_window_config config;

	config.screen_position(pos, "main");

	auto main_window=LIBCXX_NAMESPACE::w::screen::create()
		->create_mainwindow
		(config,
		 []
		 (const auto &main_window)
		 {
			 create_mainwindow(main_window);
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

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
	main_window->save(pos);
	pos.save(configfile);
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testimagebuttons();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
