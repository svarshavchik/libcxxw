#include "libcxxw_config.h"
#include "scrollbar/scrollbar_metrics.H"
#include "x/w/rectangle.H"
#include <x/exception.H>
#include <iostream>
#include <vector>

using namespace LIBCXX_NAMESPACE::w;

void testscrollbar_metrics()
{
	static const struct {
		const char *test_name;

		scroll_v_t scroll_low_size;
		scroll_v_t scroll_high_size;
		scroll_v_t virtual_size;
		scroll_v_t page_size;

		scroll_v_t pixel_size;

		scroll_v_t minimum_handlebar_pixel_size;
		bool too_small, no_slider;


		scroll_v_t handlebar_pixel_size;

		std::vector<std::pair<int32_t, scroll_v_t>> value_to_pixel;

		std::vector<scroll_v_t> los;
		std::vector<scroll_v_t> his;
		std::vector<std::pair<scroll_v_t, scroll_v_t>> pixel_to_value;
	} tests[]={
		{"test0",
		 10, 10,
		 100, 20,
		 15,
		 1,
		 true, true,
		 0},

		{"test1",
		 10, 10,
		 100, 20,
		 25,
		 10,
		 false, true,
		 5,

		 { {0, 0}, {99, 0}},

		},

		{"test2",
		 10, 10,
		 100, 20,

		 220,

		 10,

		 false, false,

		 40,
		 {{0, 0}, {1, 2}, {80, 160}, {81, 160}},
		 {0, 9},
		 {210, 211, 219, 220},

		 {{10, 0}, {11, 0}, {30, 0}, {31, 0}, {32, 1},
		  {189, 79}, {190, 80}, {191, 80}, {192, 80}},
		},
	};

	for (const auto &test:tests)
	{
		scrollbar_metrics sm;

		sm.calculate(test.scroll_low_size,
			     test.scroll_high_size,
			     test.virtual_size,
			     test.page_size,
			     test.pixel_size,
			     test.minimum_handlebar_pixel_size);

		if (test.too_small)
		{
			if (!sm.too_small)
				throw EXCEPTION(test.test_name
						<< " is not too small");
		}
		else if (sm.too_small)
			throw EXCEPTION(test.test_name
					<< " is too small");

		if (test.no_slider)
		{
			if (!sm.no_slider)
				throw EXCEPTION(test.test_name
						<< " does has slider");
		}
		else if (sm.no_slider)
			throw EXCEPTION(test.test_name
					<< " does not have slider");
		else
		{
			if (sm.handlebar_pixel_size!=test.handlebar_pixel_size)
				throw EXCEPTION(test.test_name
						<< " handlebar_pixel_size is "
						<< sm.handlebar_pixel_size);
		}

		for (const auto &vp:test.value_to_pixel)
		{
			auto res=sm.value_to_pixel(vp.first);

			if (res != vp.second)
				throw EXCEPTION(test.test_name << ": "
						<< "value_to_pixel("
						<< vp.first << ")="
						<< res
						<< " instead of "
						<< vp.second);
		}

		for (const auto &lo:test.los)
		{
			auto res=sm.pixel_to_value(lo);

			if (!res.lo)
				throw EXCEPTION(test.test_name << ": "
						<< "pixel_to_value("
						<< lo << ") is not lo");
		}

		for (const auto &hi:test.his)
		{
			auto res=sm.pixel_to_value(hi);

			if (!res.hi)
				throw EXCEPTION(test.test_name << ": "
						<< "pixel_to_value("
						<< hi << ") is not hi");
		}

		for (const auto &pv:test.pixel_to_value)
		{
			auto res=sm.pixel_to_value(pv.first);

			if (res.lo)
				throw EXCEPTION(test.test_name << ": "
						<< "pixel_to_value("
						<< pv.first
						<< ") should not be lo");
			if (res.hi)
				throw EXCEPTION(test.test_name << ": "
						<< "pixel_to_value("
						<< pv.first
						<< ") should not be hi");

			if (res.value != pv.second)
				throw EXCEPTION(test.test_name << ": "
						<< "pixel_to_value("
						<< pv.first
						<< ") is "
						<< res.value
						<< " instead of "
						<< pv.second);
		}
	}

	scrollbar_metrics sm;

	sm.calculate(10, 10, 100, 10, 100, 20);

	rectangle low{0, 0, 5, 100}, slider=low, high=low;

	sm.regions(low, slider, high,
		   &rectangle::y,
		   &rectangle::height);

	if (low != rectangle{0, 0, 5, 10} ||
	    slider != rectangle{0, 10, 5, 80} ||
	    high != rectangle{0, 90, 5, 10})
		throw EXCEPTION("regions() failed");
}

int main(int argc, char **argv)
{
	try {
		testscrollbar_metrics();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
