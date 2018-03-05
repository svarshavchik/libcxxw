/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <vector>
#include <iostream>
#include <unordered_set>
#include "x/w/rectangle.H"

struct sausage_factory {

	int number_of_calls=0;
	int number_of_areas=0;
	int rebuild_elements=0;
	int created_straight_border=0;
	int changed_corner_border=0;

	LIBCXXW_NAMESPACE::rectangle_set drawn_rectangles;
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

#define LOG_CLEAR_TO_COLOR_AREAS()	do {				\
		std::cout << "CLEAR: " << objname() << std::endl;	\
		for (const auto &aa:areas)				\
			std::cout << "    " << aa << std::endl;		\
	} while(0)

#undef LOG_CLEAR_TO_COLOR_AREAS
#define LOG_CLEAR_TO_COLOR_AREAS() do {} while(0)


#define CLEAR_TO_COLOR_LOG() do {					\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		lock->number_of_areas += areas.size();			\
		LOG_CLEAR_TO_COLOR_AREAS();				\
		lock->number_of_calls++;				\
		lock.notify_all();					\
	} while(0)

#define CLEAR_TO_COLOR_RECT() do {					\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		auto cpy=area;						\
		cpy.x = coord_t::truncate(cpy.x+di.absolute_location.x); \
		cpy.y = coord_t::truncate(cpy.y+di.absolute_location.y); \
		lock->drawn_rectangles.insert(cpy);			\
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

#define IS_MAIN_GRID \
	(container_impl->objname() == LIBCXX_NAMESPACE_STR	\
	 "::w::app_container_implObj")

#define GRID_REBUILD_ELEMENTS() do {					\
		if (!IS_MAIN_GRID) break;				\
		sausage_factory_t::lock					\
			lock(sausages);					\
		std::cout << "REBUILD ON: "				\
			  << container_impl->objname() << std::endl;	\
		++lock->rebuild_elements;				\
	} while (0)

static void foobar()
{
}

#define GRID_REBUILD_ELEMENTS_DONE() do {				\
		if (!IS_MAIN_GRID) break;				\
		sausage_factory_t::lock					\
			lock(sausages);					\
		for (auto &l:lookup)					\
		{							\
			lock->border_elements_survey			\
				.push_back(l.second			\
					   ->border_elements.size());	\
			std::cout << "GOT BORDER " <<			\
				l.second->border_elements.size()	\
				  << " FROM " << l.first->objname()	\
				  << std::endl;				\
			foobar();					\
		}							\
	} while (0)

#define CREATE_STRAIGHT_BORDER() do {					\
		if (!IS_MAIN_GRID) break;				\
		sausage_factory_t::lock					\
			lock(sausages);					\
		++lock->created_straight_border;			\
	} while (0)

#define CALLING_RECALCULATE() do {					\
		if (!IS_MAIN_GRID) break;				\
		sausage_factory_t::lock					\
			lock(sausages);					\
									\
		if (flag)						\
		{							\
			++lock->called_recalculate_with_true_flag;	\
			if (lock->called_recalculate_with_true_flag == 2) \
			{						\
				IN_THREAD->run_as(\
						  [me=ref<implObj>(this)] \
						  (IN_THREAD_ONLY) {	\
							  me->child_metrics_updated(IN_THREAD);	\
						  });			\
			}						\
			if (lock->called_recalculate_with_true_flag == 5) \
			{						\
				IN_THREAD->run_as(\
						  [me=ref<implObj>(this)] \
						  (IN_THREAD_ONLY) {	\
							  grid_map_t::lock \
								  lock(me->grid_map); \
							  (*lock)->elements_have_been_modified(); \
									\
							  me->child_metrics_updated(IN_THREAD);	\
						  });			\
			}						\
		}							\
		else							\
			++lock->called_recalculate_with_false_flag;	\
	} while (0)

#define CHANGED_CORNER_BORDER() do {					\
		if (!IS_MAIN_GRID) break;				\
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
#include "main_window_handler.C"
#include "generic_window_handler.C"
#include "gridlayoutmanager_impl.C"
#include "gridlayoutmanager_impl_elements.C"
#include "straight_border_impl.C"
#include "corner_border_impl.C"
#include "rectangle.C"

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
		  { return lock->number_of_calls >= current_number_of_calls; });

	return *lock;
}

void set_filler_color(const LIBCXX_NAMESPACE::w::element &e)
{
	e->set_background_color(LIBCXX_NAMESPACE::w::rgb{0, 0, 0});
}

auto runteststate(testmainwindowoptions &options,
		  bool individual_show)
{
	std::cout << "*** runteststate" << std::endl << std::flush;
	{
		sausage_factory_t::lock lock(sausages);

		*lock=sausage_factory();
	}

	alarm(15);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([individual_show, &options]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 auto e=m->append_row()->padding(options.nopadding->value ? 0:1).create_canvas
				 ([&]
				  (const auto &c) {
					 set_filler_color(c);
					 if (individual_show)
						 c->show();
				 },
		{10}, {10});

				 main_window->appdata=e;
			 },
			 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	guard(main_window->connection_mcguffin());

	countstateupdate c=countstateupdate::create();

	main_window->on_state_update
		([c]
		 (const auto &what, const auto &ignore)
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

	// When there's no padding, we expect one clear event, for the canvas
	// element that takes up the entire window. When we use padding, we
	// expect an extra call to clear_to_color, for the padding area around
	// the element.

	wait_until_clear(options.nopadding->value ? 1:2);

	return c->get();
}

void teststate(testmainwindowoptions &options, bool flag)
{
	auto c=runteststate(options, flag);

	alarm(0);

	if (c != 4)
		throw EXCEPTION("Expected 4 state updates, got " << c);

	sausage_factory_t::lock lock(sausages);

	// No padding, a single clear_to_color call.

	int number_of_calls=1;
	int number_of_areas=1;

	// With padding, one more clear to color, for the padding area,
	// comprising of four rectangles

	if (!options.nopadding->value)
	{
		number_of_calls=2;
		number_of_areas=5;
	}

	if (lock->number_of_calls != number_of_calls ||
	    lock->number_of_areas != number_of_areas)
		throw EXCEPTION("There were " << lock->number_of_calls
				<< " clear_to_color() calls, for "
				<< lock->number_of_areas
				<< " rectangles instead of "
				<< number_of_calls << " and "
				<< number_of_areas);

	if (!lock->correct_metrics_set_when_mapping_window)
		throw EXCEPTION("Didn't get the right initial window size");

	LIBCXXW_NAMESPACE::rectangle_slicer
		slicer{lock->drawn_rectangles,
			lock->drawn_rectangles};

	slicer.slice_slicee();
	slicer.slice_slicer();

	LIBCXXW_NAMESPACE::rectangle_set test_set{slicer.slicee_v.begin(),
			slicer.slicee_v.end()
			};

	if (test_set == lock->drawn_rectangles)
	{
		LIBCXXW_NAMESPACE::merge(test_set);

		if (test_set.size() == 1)
			return;
	}
	std::ostringstream o;

	for (const auto &r:lock->drawn_rectangles)
		o << " " << r << std::endl;

	throw EXCEPTION("Bad set of cleared rectangles:" << o.str());
}

void runtestflashwithcolor(const testmainwindowoptions &options)
{
	{
		sausage_factory_t::lock lock(sausages);

		*lock=sausage_factory();
	}

	alarm(30);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 LIBCXX_NAMESPACE::w::border_infomm b;

				 b.colors.push_back(LIBCXX_NAMESPACE::w::rgb
		{LIBCXX_NAMESPACE::w::rgb::maximum,
				LIBCXX_NAMESPACE::w::rgb::maximum,
				LIBCXX_NAMESPACE::w::rgb::maximum});

				 b.width=3;
				 b.height=3;
				 b.rounded=true;
				 b.radius=3;

				 auto e=m->append_row()->padding(options.nopadding->value ? 0:1).border(b).create_canvas
				 ([&]
				  (const auto &c) {
					 if (options.showhide->value)
						 set_filler_color(c);
				 },
		{10.0}, {10.0});

				 main_window->appdata=e;
			 });

	guard(main_window->connection_mcguffin());

	LIBCXX_NAMESPACE::w::element e=main_window->appdata;

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

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 m->append_row()->padding(options.nopadding->value ? 0:1).create_canvas
				 ([]
				  (const auto &ignore) {},
		{10}, {10});
			 });

	auto [original_theme, original_scale, original_options]=
		main_window->get_screen()->get_connection()
		->current_theme();

	std::string alternate_theme;

	for (const auto &theme:LIBCXX_NAMESPACE::w::connection::base
		     ::available_themes())
	{
		if (theme.identifier == original_theme)
			continue;
		alternate_theme=theme.identifier;
		break;
	}

	if (alternate_theme.empty())
		throw EXCEPTION("Couldn't find an alternate theme to use");

	guard(main_window->connection_mcguffin());

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
			->set_theme(flag ? alternate_theme:original_theme,
				    original_scale,
				    original_options,
				    true);

		flag= !flag;
		{
			sausage_factory_t::lock lock(sausages);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}
	}
}

void verify_clears(const testmainwindowoptions &options)
{
	sausage_factory_t::lock lock(sausages);

	// No padding: there's one call to clear_to_color() from the initial
	// exposure, then four more calls for each color change. Each color
	// change draws a single rectangle.

	int number_of_calls=5;
	int number_of_areas=5;

	if (!options.nopadding->value)
	{
		// With padding, there are five additional calls, for the
		// padding area. Each call clears for rectangles, for the
		// padding area around the center cell.

		number_of_calls += 5;
		number_of_areas += 20;

	}

	if (lock->number_of_calls != number_of_calls ||
	    lock->number_of_areas != number_of_areas)
		throw EXCEPTION("There were " << lock->number_of_calls
				<< " clear_to_color() calls, for "
				<< lock->number_of_areas
				<< " rectangles instead of "
				<< number_of_calls << " and "
				<< number_of_areas);
}

void testflashwithcolor(const testmainwindowoptions &options)
{
	runtestflashwithcolor(options);
	alarm(0);
	verify_clears(options);
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

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();

				 LIBCXX_NAMESPACE::w::border_infomm b;

				 b.colors.push_back(LIBCXX_NAMESPACE::w
						    ::rgb{0, 0, 0});
				 b.width=1;
				 b.height=1;
				 b.dashes.push_back(2);
				 b.dashes.push_back(3);
				 b.dashes.push_back(2);

				 auto c=m->append_row()->border(b).create_canvas
				 ([]
				  (const auto &ignore) {},
		{30}, {30});

				 main_window->appdata=c;
			 });

	auto [original_theme, original_scale, original_options]=
		main_window->get_screen()->get_connection()
		->current_theme();

	guard(main_window->connection_mcguffin());

	countsizeupdate cmain=countsizeupdate::create();

	main_window->on_state_update
		([cmain]
		 (const auto &what, const auto &ignore)
		 {
			 std::cout << "Main window: " << what << std::endl;

			 if (what.shown &&
			     what.state_update == what.current_state)
				 cmain->increment(what.current_position.width);
		 });

	countsizeupdate ccanvas=countsizeupdate::create();

	LIBCXX_NAMESPACE::w::element(main_window->appdata)
		->on_state_update
		([ccanvas, first_time=true]
		 (const auto &what, const auto &ignore)
		 mutable
		 {
			 if (first_time)
			 {
				 first_time=false;
				 return;
			 }

			 std::cout << "Canvas: " << what << std::endl;
			 if (what.shown &&
			     what.state_update == what.current_state)
			 {
				 ccanvas->increment(what.current_position.width);
			 }
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
			->set_theme(original_theme,
				    (i % 2) ? 100:200,
				    original_options,
				    true);

		flag= !flag;
		{
			sausage_factory_t::lock lock(sausages);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}
	}

	main_window->get_screen()->get_connection()
		->set_theme(original_theme,
			    original_scale,
			    original_options,
			    true);
	return {cmain, ccanvas};
}

void testthemescale(const testmainwindowoptions &options)
{
	auto c=runtestthemescale(options);
	alarm(0);

	{
		sausage_factory_t::lock lock(sausages);

		// Initial rebuild, plus rebuild forced by manually setting
		// elements_have_been_modified(), plus for more rebuilts
		// triggered by padding_recalculated().

		if (lock->rebuild_elements != 6)
			throw EXCEPTION("Rebuilt elements " <<
					lock->rebuild_elements <<
					" times instead of six times.");
		// Check that rebuild_elements created a single grid_element
		// with 8 borders.
		if (lock->border_elements_survey != std::vector<int>({8, 8, 8, 8, 8, 8}))
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
	alarm(0);
	verify_clears(options);
}

int main(int argc, char **argv)
{
	testmainwindowoptions options;

	options.parse(argc, argv);

	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);

		if (options.state->value)
		{
			teststate(options, false);
			teststate(options, true);
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
