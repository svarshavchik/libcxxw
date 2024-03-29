/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/property_properties.H>
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/label.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/progressbar.H"

x::mpobj<int> slider_counter=0;
x::mpcobj<int> label_counter=0;

#define TEST_SLIDER_GRADIENT() do { slider_counter=slider_counter.get()+1; } \
	while(0);
#define TEST_TEXTLABEL_DRAW() do {				\
		x::mpcobj<int>::lock lock{label_counter};	\
								\
		++*lock;					\
		lock.notify_all();				\
	}							\
	while(0);

#include "progressbar_slider.C"
#define DEBUG_INITIAL_METRICS
#include "textlabel_impl.C"
#include "picture_impl.C"

#include <string>
#include <iostream>

void wait_for_label_draw(int value)
{
	x::mpcobj<int>::lock lock{label_counter};

	lock.wait([&]
		  {
			  return *lock>=value;
		  });
}

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

void test_progressbar(const LIBCXX_NAMESPACE::w::factory &f)
{
	LIBCXX_NAMESPACE::w::progressbar_config config;
	LIBCXX_NAMESPACE::w::new_gridlayoutmanager nglm;

	auto creator=
		[](const LIBCXX_NAMESPACE::w::progressbar &pb)
		{
		};

	f->create_progressbar(creator);
	f->create_progressbar(creator, config);
	f->create_progressbar(creator, nglm);
	f->create_progressbar(creator, config, nglm);
	f->create_progressbar(creator,
			      LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});
	f->create_progressbar(creator, config,
			      LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});
	f->create_progressbar(creator,
			      LIBCXX_NAMESPACE::w::progressbar_config{},
			      LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});
}

static auto initialize_progressbar(const LIBCXX_NAMESPACE::w::factory &f)
{
	LIBCXX_NAMESPACE::w::progressbar_config config;

	config.appearance=config.appearance
		->modify([]
			 (const auto &appearance)
			 {
				 appearance->foreground_color=
					 LIBCXX_NAMESPACE::w::green;
				 appearance->label_font=
					 LIBCXX_NAMESPACE::w::font
					 {"liberation mono; point_size=24"};
			 });

	auto pb=f->create_progressbar
		([]
		 (const LIBCXX_NAMESPACE::w::progressbar &pb)
		 {
			 LIBCXX_NAMESPACE::w::gridlayoutmanager
			 glm=pb->get_layoutmanager();

			 auto f=glm->append_row();
			 f->halign(LIBCXX_NAMESPACE::w::halign::center);
			 f->create_label("0%")->show();
		 }, config);

	pb->show();

	return pb;
}

static void testnormalize(const std::vector<size_t> &indexes,
			  size_t expected)
{
	LIBCXX_NAMESPACE::w::rgb_gradient g;

	for (const auto &i:indexes)
		g.insert({i, {}});

	LIBCXX_NAMESPACE::w::gradient_normalize
		(g,
		 [&]
		 (const xcb_render_fixed_t *f,
		  const xcb_render_color_t *,
		  size_t n)
		 {
			 if (n != expected)
				 throw EXCEPTION("gradient_normalize: wrong # of values");
			 if (f[0] != 0)
				 throw EXCEPTION("First normalized value is not 0");

			 if (f[n-1] != 0x10000)
				 throw EXCEPTION("Last normalized value is not 1");
			 for (size_t i=1; i<n; ++i)
			 {
				 if (f[i] <= f[i-1])
					 throw EXCEPTION("Not monotonic");
			 }
		 });
}

void testprogressbar()
{
	alarm(60);
	testnormalize({}, 2);
	testnormalize({0}, 2);
	testnormalize({100}, 2);
	testnormalize({0, 0x1000, 0x100000000, 0x100000001,
				0x200000000, 0x200000001,
				0x200000002, 0x200000003}, 7);
	testnormalize({0x200000002, 0x200000003}, 2);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager
				     layout=main_window->get_layoutmanager();
				 LIBCXX_NAMESPACE::w::gridfactory factory=
				     layout->append_row();

				 initialize_progressbar(factory);
			 });

	LIBCXX_NAMESPACE::w::progressbar pb=
		LIBCXX_NAMESPACE::w::gridlayoutmanager
		{
			main_window->get_layoutmanager()
		}->get(0, 0);

	LIBCXX_NAMESPACE::w::label l=
		LIBCXX_NAMESPACE::w::gridlayoutmanager
		{
			pb->get_layoutmanager()
		}->get(0, 0);

	main_window->set_window_title("Progressbar!");

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

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	int v=0;

	int redraw_counter=0;
	while (1)
	{
		wait_for_label_draw(++redraw_counter);
		lock.wait_for(std::chrono::milliseconds(100),
			      [&]
			      {
				      return *lock;
			      });
		if (v >= 100)
			break;
		v += 5;

		std::ostringstream o;

		o << v << '%';

		pb->update(v, 100, [txt=o.str(), l]
			   (ONLY IN_THREAD)
			   {
				   l->update(txt);
			   });
	}

	lock.wait_for(std::chrono::seconds(3),
		      [&] { return *lock; });

	if (label_counter.get() != 21)
		throw EXCEPTION("Label was redrawn " <<
				label_counter.get() << " times instead of 21");
}

int main(int argc, char **argv)
{
	x::property::load_property("x::w::themes", "./themes", true, false);
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testprogressbar();
		if (slider_counter.get() != 1)
			throw EXCEPTION("Progress bar gradient was created "
					<< slider_counter.get()
					<< " times instead of 1");

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
