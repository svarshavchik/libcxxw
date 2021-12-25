/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/config.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/label.H"
#include "x/w/button.H"
#include "x/w/color_picker.H"
#include "x/w/color_picker_config.H"
#include "x/w/font_picker.H"
#include "x/w/font_picker_config.H"
#include "x/w/canvas.H"
#include "x/w/screen_positions.H"
#include <string>
#include <list>
#include <iostream>
#include <random>
#include "testcolorpicker.inc.H"

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

struct rand_color_sourceObj : virtual public obj {

	std::mt19937 rng;

	std::uniform_int_distribution<rgb_component_t> dist{0, rgb::maximum};

	rand_color_sourceObj()
	{
		rng.seed(std::random_device{}());
	}

	rgb random_color()
	{
		return {dist(rng), dist(rng), dist(rng)};
	}
};

void testcolorpicker(const testcolorpicker_options &options)
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto configfile=
		LIBCXX_NAMESPACE::configdir("testcolorpicker@libcxx.com")
		+ "/windows";

	auto pos=LIBCXX_NAMESPACE::w::screen_positions::create(configfile);

	color_pickerptr cpp;

	LIBCXX_NAMESPACE::w::main_window_config config{"main"};

	config.restore(pos);

	auto main_window=main_window::create
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 layout->row_alignment(0, valign::middle);
			 layout->row_alignment(1, valign::middle);

			 auto factory=layout->append_row();

			 factory->create_label("Color:")->show();

			 LIBCXX_NAMESPACE::w::color_picker_config cconfig;

			 cconfig.restore(pos, "main_color");
			 cconfig.enable_alpha_channel=true;

			 auto cp=factory->create_color_picker(cconfig);

			 cp->on_color_update([]
					     (ONLY IN_THREAD,
					      const LIBCXX_NAMESPACE::w::rgb &color,
					      const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
					      const LIBCXX_NAMESPACE::w::busy &mcguffin) {

						     std::cout << color << std::endl;
					     });

			 cp->show();

			 cpp=cp;

			 factory=layout->append_row();
			 factory->create_label("Font:")->show();

			 font_picker_config config;

			 config.selection_required=options.required->value;
			 config.restore(pos, "main_font");

			 font f;

			 bool set_font=false;

			 if (!options.font_family->value.empty())
			 {
				 set_font=true;
				 f.family=options.font_family->value;
			 }

			 if (options.font_size->is_set())
			 {
				 set_font=true;
				 f.point_size=options.font_size->value;
			 }

			 if (options.font_weight->is_set())
			 {
				 set_font=true;
				 f.set_weight(options.font_weight->value);
			 }

			 if (set_font)
				 config.initial_font=f;

			 auto fp=factory->create_font_picker(config);

			 fp->on_font_update([mru=std::list<font_picker_group_id>
					 {config.most_recently_used.begin(),
					  config.most_recently_used.end()}]
					    (THREAD_CALLBACK,
					     const font &new_font,
					     const font_picker_group_id *new_font_group,
					     const font_picker &myself,
					     const callback_trigger_t &trigger,
					     const busy &mcguffin)
					    mutable {
						    if (!new_font_group)
							    return;

						    auto iter=std::find(mru.begin(),
									mru.end(),
									*new_font_group);

						    if (iter != mru.end())
							    mru.erase(iter);

						    if (mru.size() >= 3)
							    mru.pop_back();

						    mru.push_front(*new_font_group);
						    myself->most_recently_used
							    ({mru.begin(),
									    mru.end()});

					    });
			 fp->show();

			 auto b=layout->append_row()->colspan(2)
			 .halign(halign::center)
			 .padding(4).create_button
			 ("Random color");

			 b->on_activate([cp,
					 source=ref<rand_color_sourceObj>
					 ::create()]
					(ONLY IN_THREAD,
					 const auto &trigger,
					 const auto &busy) {
						cp->current_color
							(IN_THREAD,
							 source->random_color()
							 );
					});
			 b->show_all();

			 b=layout->append_row()->colspan(2)
			 .halign(halign::center)
			 .padding(4).create_button
			 ("Normal font");

			 b->on_activate([fp]
					(ONLY IN_THREAD,
					 const auto &trigger,
					 const auto &busy) {
						font f{"liberation sans",
								12};

						fp->current_font(IN_THREAD, f);
					});
			 b->show_all();

			 b=layout->append_row()->colspan(2)
			 .halign(halign::center)
			 .padding(4).create_button
			 ("Title font");

			 b->on_activate([fp]
					(ONLY IN_THREAD,
					 const auto &trigger,
					 const auto &busy) {
						font f{"liberation mono",
								18};
						f.set_weight("bold");
						fp->current_font(IN_THREAD, f);
					});
			 b->show_all();
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	main_window->set_window_title("Color picker");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

	main_window->save(pos);

	pos->save(configfile);
	std::cout << "Final color: " << cpp->current_color()
		  << std::endl;
}

int main(int argc, char **argv)
{
	try {
		testcolorpicker_options options;

		options.parse(argc, argv);
		testcolorpicker(options);
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
