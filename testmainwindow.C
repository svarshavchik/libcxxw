/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/mpobj.H>
#include <vector>

struct sausage_factory {

	int number_of_calls=0;
	int number_of_areas=0;
	int rebuild_elements=0;
	int created_straight_border=0;
	int changed_corner_border=0;

	int called_recalculate_with_false_flag=0;
	int called_recalculate_with_true_flag=0;
	std::vector<int> border_elements_survey;
	bool correct_metrics_set=false;
	bool correct_metrics_set_to_configure_window=false;
	bool correct_metrics_set_when_mapping_window=false;
	bool drew_stub_border=false;
};

typedef LIBCXX_NAMESPACE::mpcobj<sausage_factory> sausage_factory_t;

sausage_factory_t sausages;

#define CLEAR_TO_COLOR_LOG() do {					\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		lock->number_of_areas += areas.size();			\
		lock->number_of_calls++;				\
		lock.notify_all();					\
	} while(0)

#define REQUEST_VISIBILITY_LOG(w,h) do {				\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		if (lock->correct_metrics_set && (w) > 4 && (h) > 4)	\
			lock->correct_metrics_set_to_configure_window=true; \
	} while(0)

#define MAP_LOG() do {				\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		if (lock->correct_metrics_set_to_configure_window)	\
			lock->correct_metrics_set_when_mapping_window=true; \
	} while(0)


#define GRIDLAYOUTMANAGER_RECALCULATE_LOG(elements) do {		\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		if (elements->all_elements.size() > 0)			\
			lock->correct_metrics_set=true;			\
	} while(0)

#define GRID_REBUILD_ELEMENTS() do {					\
		sausage_factory_t::lock					\
			lock(sausages);					\
		++lock->rebuild_elements;				\
	} while (0)

#define GRID_REBUILD_ELEMENTS_DONE() do {				\
		sausage_factory_t::lock					\
			lock(sausages);					\
		for (auto &l:lookup)					\
			lock->border_elements_survey			\
				.push_back(l.second			\
					   ->border_elements.size());	\
	} while (0)

#define CREATE_STRAIGHT_BORDER() do {					\
		sausage_factory_t::lock					\
			lock(sausages);					\
		++lock->created_straight_border;			\
	} while (0)

#define CALLING_RECALCULATE() do {					\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		if (flag)						\
		{							\
			++lock->called_recalculate_with_true_flag;	\
			if (lock->called_recalculate_with_true_flag == 2) \
			{						\
				IN_THREAD->run_as(RUN_AS,		\
						  [me=ref<implObj>(this)] \
						  (IN_THREAD_ONLY) {	\
							  me->child_metrics_updated(IN_THREAD);	\
						  });			\
			}						\
			if (lock->called_recalculate_with_true_flag == 5) \
			{						\
				IN_THREAD->run_as(RUN_AS,		\
						  [me=ref<implObj>(this)] \
						  (IN_THREAD_ONLY) {	\
							  grid_map_t::lock \
								  lock(me->grid_map); \
							  lock->elements_have_been_modified(); \
									\
							  me->child_metrics_updated(IN_THREAD);	\
						  });			\
			}						\
		}							\
		else							\
			++lock->called_recalculate_with_false_flag;	\
									\
		std::cout << "RECALCULATE_METRICS, flag=" << flag << std::endl;\
	} while (0)

#define CHANGED_CORNER_BORDER() do {					\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		++lock->changed_corner_border;				\
	} while(0)

#define DRAW_STUB_BORDER() do {						\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		lock->drew_stub_border=true;				\
	} while(0)


#include "element_impl.C"
#include "generic_window_handler.C"
#include "gridlayoutmanager_impl.C"
#include "gridlayoutmanager_impl_elements.C"
#include "straight_border_impl.C"
#include "corner_border_impl.C"

#include "x/w/main_window.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/factory.H"
#include "x/w/rgb.H"
#include "x/w/picture.H"
#include "x/w/canvas.H"
#include <x/destroy_callback.H>
#include <x/mpobj.H>
#include <x/functionalrefptr.H>
#include <x/mcguffinstash.H>
#include <string>
#include <iostream>
#include <unistd.h>
#include "testmainwindowoptions.H"

class countstateupdateObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::mpobj<int> counter{0};

	void increment()
	{
		LIBCXX_NAMESPACE::mpobj<int>::lock lock(counter);

		++*lock;
	}

	int get()
	{
		return *LIBCXX_NAMESPACE::mpobj<int>::lock(counter);
	}
};

typedef LIBCXX_NAMESPACE::ref<countstateupdateObj> countstateupdate;

auto wait_until_clear(int current_number_of_calls)
{
	sausage_factory_t::lock lock(sausages);

	lock.wait([&]
		  { return lock->number_of_calls > current_number_of_calls; });

	return *lock;
}

void set_filler_color(const LIBCXX_NAMESPACE::w::element &e)
{
	e->set_background_color(e->get_screen()
				->create_solid_color_picture({0, 0, 0}));
}

countstateupdate runteststate(bool individual_show)
{
	std::cout << "*** runteststate" << std::endl << std::flush;
	{
		sausage_factory_t::lock lock(sausages);

		*lock=sausage_factory();
	}

	alarm(15);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	typedef LIBCXX_NAMESPACE::mcguffinstash<> stash_t;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base
		::create([individual_show]
			 (const auto &main_window)
			 {
				 auto stash=stash_t::create();

				 main_window->appdata=stash;

				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 auto e=m->create()->create_canvas
				 ([&]
				  (const auto &c) {
					 set_filler_color(c);
					 if (individual_show)
						 c->show();
				 },
				  10, 10);

				 stash->insert("canvas", e);
			 });

	guard(main_window->get_screen()->mcguffin());

	countstateupdate c=countstateupdate::create();

	auto mcguffin=main_window->on_state_update
		([c]
		 (const auto &what)
		 {
			 std::cout << "Window state update: " << what
			 << std::endl;

			 c->increment();
		 });

	if (individual_show)
	{
		std::cout << "Waiting" << std::endl << std::flush;
		sleep(3);
		main_window->show();
	}
	else
	{
		main_window->show_all();
	}

	main_window->get_screen()->get_connection()->on_disconnect([]
								   {
									   exit(1);
								   });

	wait_until_clear(0);

	return c;
}

void teststate(bool flag)
{
	auto c=runteststate(flag);

	alarm(0);

	if (c->get() != 4)
		throw EXCEPTION("Expected 4 state updates");

	sausage_factory_t::lock lock(sausages);

	if (lock->number_of_calls != 1 ||
	    lock->number_of_areas != 1)
		throw EXCEPTION("There were " << lock->number_of_calls
				<< " clear_to_color() calls, for "
				<< lock->number_of_areas
				<< " rectangles instead of a single call");

	if (!lock->correct_metrics_set_when_mapping_window)
		throw EXCEPTION("Didn't get the right initial window size");
}

void runtestflashwithcolor(const testmainwindowoptions &options)
{
	{
		sausage_factory_t::lock lock(sausages);

		*lock=sausage_factory();
	}

	alarm(30);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	typedef LIBCXX_NAMESPACE::mcguffinstash<> stash_t;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base
		::create([&]
			 (const auto &main_window)
			 {
				 auto stash=stash_t::create();

				 main_window->appdata=stash;

				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 LIBCXX_NAMESPACE::w::border_infomm b;

				 auto bg=main_window->get_screen()
				 ->create_solid_color_picture
				 ({LIBCXX_NAMESPACE::w::rgb::maximum,
				   LIBCXX_NAMESPACE::w::rgb::maximum,
				   LIBCXX_NAMESPACE::w::rgb::maximum});

				 b.colors.push_back(bg);
				 b.width=3;
				 b.height=3;
				 b.rounded=true;
				 b.radius=3;

				 auto e=m->create()->border(b).create_canvas
				 ([&]
				  (const auto &c) {
					 if (options.showhide->value)
						 set_filler_color(c);
				 },
				  10.0, 10.0);
				 stash->insert("canvas", e);


			 });

	guard(main_window->get_screen()->mcguffin());

	LIBCXX_NAMESPACE::w::element e=
		stash_t(main_window->appdata)->get("canvas");

	if (options.usemain->value)
		e=main_window;

	main_window->show_all();

	main_window->get_screen()->get_connection()->on_disconnect([]
								   {
									   exit(1);
								   });
	bool flag=true;

	for (int i=0; i<4; ++i)
	{
		{
			sausage_factory_t::lock lock(sausages);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}

		if (options.showhide->value)
		{
			if (flag)
			{
				e->hide();
			}
			else
			{
				e->show();
			}
		}
		else
		{
			if (flag)
			{
				set_filler_color(e);
			}
			else
			{
				e->remove_background_color();
			}
		}
		flag= !flag;
		{
			sausage_factory_t::lock lock(sausages);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}
	}
}

void runtestflashwiththeme(const testmainwindowoptions &options)
{
	{
		sausage_factory_t::lock lock(sausages);

		*lock=sausage_factory();
	}

	alarm(30);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base
		::create([&]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 m->create()->create_canvas
				 ([]
				  (const auto &ignore) {},
				  10, 10);
			 });

	auto original_theme=main_window->get_screen()->get_connection()
		->current_theme();

	std::string alternate_theme;

	for (const auto &theme:LIBCXX_NAMESPACE::w::connection::base
		     ::available_themes())
	{
		if (theme.identifier == original_theme.first)
			continue;
		alternate_theme=theme.identifier;
		break;
	}

	if (alternate_theme.empty())
		throw EXCEPTION("Couldn't find an alternate theme to use");

	guard(main_window->get_screen()->mcguffin());

	main_window->show_all();

	main_window->get_screen()->get_connection()->on_disconnect([]
								   {
									   exit(1);
								   });
	bool flag=true;

	for (int i=0; i<4; ++i)
	{
		{
			sausage_factory_t::lock lock(sausages);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}

		main_window->get_screen()->get_connection()
			->set_theme(flag ? alternate_theme:original_theme.first,
				    original_theme.second);

		flag= !flag;
		{
			sausage_factory_t::lock lock(sausages);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}
	}
}

void testflashwithcolor(const testmainwindowoptions &options)
{
	runtestflashwithcolor(options);
	alarm(0);

	sausage_factory_t::lock lock(sausages);

	if (lock->number_of_calls != 5 || lock->number_of_areas != 5)
		throw EXCEPTION("There were " << lock->number_of_calls
				<< " clear_to_color() calls, for "
				<< lock->number_of_areas
				<< " rectangles instead of five");
}

///////////////////////////////////////////////////////////////////////////

class countsizeupdateObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	typedef std::map<LIBCXX_NAMESPACE::w::dim_t, int> s;

	LIBCXX_NAMESPACE::mpobj<s> counter;

	void increment(auto n)
	{
		LIBCXX_NAMESPACE::mpobj<s>::lock lock(counter);

		++(*lock)[n];
	}

	auto get()
	{
		return *LIBCXX_NAMESPACE::mpobj<s>::lock(counter);
	}
};

typedef LIBCXX_NAMESPACE::ref<countsizeupdateObj> countsizeupdate;

std::tuple<countsizeupdate,
	   countsizeupdate>
runtestthemescale(const testmainwindowoptions &options)
{
	{
		sausage_factory_t::lock lock(sausages);

		*lock=sausage_factory();
	}

	alarm(30);

	typedef LIBCXX_NAMESPACE::mcguffinstash<> stash_t;

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base
		::create([&]
			 (const auto &main_window)
			 {
				 auto stash=stash_t::create();

				 main_window->appdata=stash;

				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();

				 auto bg=main_window->get_screen()
				 ->create_solid_color_picture({0, 0, 0});

				 LIBCXX_NAMESPACE::w::border_infomm b;

				 b.colors.push_back(bg);
				 b.width=1;
				 b.height=1;
				 b.dashes.push_back(2);
				 b.dashes.push_back(3);
				 b.dashes.push_back(2);

				 auto c=m->create()->border(b).create_canvas
				 ([]
				  (const auto &ignore) {},
				  30, 30);

				 stash->insert("canvas", c);
			 });

	auto original_theme=main_window->get_screen()->get_connection()
		->current_theme();

	guard(main_window->get_screen()->mcguffin());

	countsizeupdate cmain=countsizeupdate::create();

	auto mainmcguffin=main_window->on_state_update
		([cmain]
		 (const auto &what)
		 {
			 std::cout << "Main window: " << what << std::endl;

			 if (what.shown &&
			     what.state_update == what.current_state)
				 cmain->increment(what.current_position.width);
		 });

	countsizeupdate ccanvas=countsizeupdate::create();

	auto canvasmcguffin=
		LIBCXX_NAMESPACE::w::element(stash_t(main_window->appdata)
					     ->get("canvas"))
		->on_state_update
		([ccanvas]
		 (const auto &what)
		 {
			 std::cout << "Canvas: " << what << std::endl;
			 if (what.shown &&
			     what.state_update == what.current_state)
				 ccanvas->increment(what.current_position.width);
		 });

	main_window->show_all();

	main_window->get_screen()->get_connection()->on_disconnect([]
								   {
									   exit(1);
								   });
	bool flag=true;

	for (int i=0; i<4; ++i)
	{
		{
			sausage_factory_t::lock lock(sausages);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}

		main_window->get_screen()->get_connection()
			->set_theme(original_theme.first,
				    (i % 2) ? 100:200);

		flag= !flag;
		{
			sausage_factory_t::lock lock(sausages);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}
	}
	return {cmain, ccanvas};
}

void testthemescale(const testmainwindowoptions &options)
{
	auto c=runtestthemescale(options);
	alarm(0);

	{
		sausage_factory_t::lock lock(sausages);

		// Initial rebuild, plus rebuild forced by manually setting
		// elements_have_been_modified();

		if (lock->rebuild_elements != 2)
			throw EXCEPTION("Rebuilt elements " <<
					lock->rebuild_elements <<
					" times instead of two times.");
		// Check that rebuild_elements created a single grid_element
		// with 8 borders.
		if (lock->border_elements_survey != std::vector<int>({8, 8}))
			throw EXCEPTION("Did not get expected border_elements.");
		if (lock->created_straight_border != 4)
			throw EXCEPTION("Expected 4 new borders, got " <<
					lock->created_straight_border);

		if (lock->called_recalculate_with_false_flag != 0)
			throw EXCEPTION("Expected 0 false recalcs, got " <<
					lock->called_recalculate_with_false_flag
					);

		// Initial recalc
		// 4 recalcs due to theme scale change.
		// Two extra dummy recalcs triggered by CALLING_RECALCULATE()

		if (lock->called_recalculate_with_true_flag != 7)
			throw EXCEPTION("Expected 7 true recalcs, got " <<
					lock->called_recalculate_with_true_flag
					);

		if (lock->drew_stub_border)
			throw EXCEPTION("Did not expect to draw a stub border");

		if (lock->changed_corner_border > 0)
			throw EXCEPTION("Did not expect to change a corner border");
	}
	auto s=std::get<0>(c)->get();

	if (s.size() != 2)
		throw EXCEPTION("Main window did not have exactly two sizes");

	for (const auto &n:s)
		if (n.second != 2)
			throw EXCEPTION("Main window expected two updates for each size");

	s=std::get<1>(c)->get();

	if (s.size() != 2)
		throw EXCEPTION("Canvas did not have exactly two sizes");

	for (const auto &n:s)
		if (n.second != 2)
			throw EXCEPTION("Canvas expected two updates for each size");
}

void testflashwiththeme(const testmainwindowoptions &options)
{
	runtestflashwiththeme(options);

	sausage_factory_t::lock lock(sausages);

	if (lock->number_of_calls != 5 || lock->number_of_areas != 5)
		throw EXCEPTION("There were " << lock->number_of_calls
				<< " clear_to_color() calls, for "
				<< lock->number_of_areas
				<< " rectangles instead of five");
}

int main(int argc, char **argv)
{
	testmainwindowoptions options;

	options.parse(argc, argv);

	try {
		if (options.state->value)
		{
			teststate(false);
			teststate(true);
		}
		else if (options.usetheme->value)
		{
			testflashwiththeme(options);
		}
		else if (options.themescale->value)
		{
			testthemescale(options);
		}
		else
		{
			if (options.showhide->value)
				options.usemain->value=false;
			testflashwithcolor(options);
		}
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
