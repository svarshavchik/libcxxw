/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/weakcapture.H>

#include <x/w/main_window.H>
#include <x/w/label.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/scrollbar.H>
#include <x/w/image_button.H>
#include <x/w/input_field.H>
#include <x/w/container.H>
#include <x/w/key_event.H>
#include <x/w/dialog.H>

#include "close_flag.H"

#include <string>
#include <string_view>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

// Need an object to keep track of some metadata, that can be captured
// by several callbacks.

class volume_infoObj : virtual public x::obj {

public:

	bool use_decimals=false;

	double current_volume=0;

	// Return a scrollbar that ranges 0-11, or 0-110.

	auto scrollbar_config() const
	{
		x::w::scrollbar_config config{12, 1};

		if (use_decimals)
		{
			config.range=120;
			config.page_size=10;
			config.value=(int)std::round(current_volume * 10);
		}
		else
		{
			config.value=(int)current_volume;
		}
		return config;
	}

	void set_volume(double v,
			const x::w::input_field &input_field)
	{
		current_volume=use_decimals ? v/10:v;

		std::ostringstream o;

		if (use_decimals)
			o << std::fixed << std::setprecision(1);
		o << current_volume;

		input_field->set(o.str());
	}

	void set_volume(const x::w::input_field &input_field,
			const x::w::scrollbar &scrollbar,
			const x::w::main_window &main_window)
	{
		std::string current_volume=x::w::input_lock{input_field}.get();

		std::istringstream i{current_volume};

		double v;

		if ( (i >> v) && i.get() == std::istream::traits_type::eof())
		{
			if (use_decimals)
				v *= 10;

			int n=std::round(v);

			if (n >= 0 && n <= (use_decimals ? 110:11))
			{
				scrollbar->set(n);
				return;
			}
		}

		// Bad input. Show an error dialog.

		auto d=main_window->create_ok_dialog
			({"scrollbar_error@examples.w.libcxx.com", true},
			 "alert",
			 []
			 (const x::w::factory &f)
			 {
				 f->create_label("Bad input");
			 },

			 main_window->destroy_when_closed
			 ("scrollbar_error@examples.w.libcxx.com"));

		d->dialog_window->set_window_title("Error");
		main_window->set_window_class
			("main",
			 "scrollbar@examples.w.libcxx.com");
		d->dialog_window->show_all();
	}

	void set_decimals(const x::w::input_field &input_field,
			  const x::w::scrollbar &scrollbar)
	{
		scrollbar->reconfigure(scrollbar_config());

		set_volume(current_volume, input_field);
	}
};

typedef x::ref<volume_infoObj> volume_info;

void initialize_volume_control(const x::w::main_window &main_window)
{
	x::w::gridlayoutmanager	layout=main_window->get_layoutmanager();
	x::w::gridfactory factory=layout->append_row();

	auto vi=volume_info::create();

	x::w::image_button checkbox=
		factory->create_checkbox([]
					 (const auto &f)
					 {
						 f->create_label("Volume has decimal points");
					 });

	factory=layout->append_row();
	// Create an input_field, with a '%' immediately afterwards. Center it
	// because the scrollbar will be below it, and wider. For doing this,
	// we'll create an inner container, that's centered.

	factory->halign(x::w::halign::center);

	x::w::input_fieldptr input_fieldptr;

	factory->create_container
		([&]
		 (const auto &container)
		 {
			 x::w::gridlayoutmanager glm=
				 container->get_layoutmanager();

			 glm->row_alignment(0, x::w::valign::middle);

			 auto row=glm->append_row();

			 // Don't need any padding, the main grid default
			 // padding will suffice.

			 row->padding(0);

			 x::w::input_field_config config{5};
			 config.alignment=x::w::halign::right;

			 input_fieldptr=row->create_input_field("0", config);
			 row->create_label("%");
		 },
		 x::w::new_gridlayoutmanager{});

	x::w::input_field input_field=input_fieldptr;

	factory=layout->append_row();

	x::w::scrollbar sb=
		factory->create_horizontal_scrollbar(vi->scrollbar_config(),
						     [vi, input_field]
						     (ONLY IN_THREAD,
						      const x::w::scrollbar_info_t &status)
						     {
							     vi->set_volume(status.dragged_value,
									    input_field);
						     },

						     // 75 millimeters wide.
						     75);

	// Add a key event to the input field, for the Enter key, to set the
	// manually typed-in value as the explicit value.

	input_field->on_key_event([vi,
				   fields=x::make_weak_capture(sb, input_field,
							       main_window)]
				  (ONLY IN_THREAD,
				   const auto &why,
				   bool activated,
				   const auto &ignore)
				  {
					  // Verify that that a key_event gets
					  // passed in.

					  if (!std::holds_alternative<const x::w::key_event *>(why))
						  return false;

					  auto ke=std::get<const x::w::key_event *>(why);

					  if (ke->unicode != '\n')
						  return false;

					  if (!activated)
						  return true;

					  auto got=fields.get();

					  if (got)
					  {
						  auto & [sb, input_field,
							  main_window] = *got;

						  vi->set_volume(input_field,
								 sb,
								 main_window);
					  }

					  return true;
				  });


	checkbox->on_activate([vi,
			       fields=x::make_weak_capture(sb, input_field)]
			      (ONLY IN_THREAD,
			       size_t state,
			       const auto &ignore1,
			       const auto &ignore2)
			      {
				      vi->use_decimals=state > 0;

				      auto got=fields.get();

				      if (got)
				      {
					      auto &[sb, input_field]=*got;

					      vi->set_decimals(input_field, sb);
				      }

			      });
}

void testscrollbar()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 initialize_volume_control(main_window);
				 main_window->set_window_title("Volume control");
				 main_window->show_all();
			 });

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

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		testscrollbar();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
