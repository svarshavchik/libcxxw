#include "metrics_grid.H"
#include <x/exception.H>
#include "metrics_element.H"
#include "metrics_grid_axisrange.H"
#include "metrics_grid_pos.H"

#include <sstream>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;
using namespace LIBCXX_NAMESPACE::w::metrics;

struct testgrid_info {
	grid_xy x1, x2, y1, y2;
	dim_t min, pref, max;
};

static void do_test(const char *testname,
		    const std::vector<testgrid_info> &test_info,
		    const grid_metrics_t &res)
{
	grid_t g;

	for (const auto &info: test_info)
	{
		element metrics=element::create(info.min, info.pref, info.max);

		g.push_back(grid_pos::create(grid_axisrange{info.x1, info.x2},
					     grid_axisrange{info.y1, info.y2},
					     metrics));
	}

	auto metrics=calculate_grid_horiz_metrics(g);

	if (metrics != res)
	{
		std::ostringstream o;
		const char *sep="";

		for (const auto &m:metrics)
		{
			o << sep << m.first << ": " << m.second;
			sep="; ";
		}

		throw EXCEPTION(testname << " failed: " << o.str());
	}
}

void testgrid()
{
	do_test("test1",
		{
			{ 0, 0, 0, 0, 10, 20, 30},
			{ 2, 2, 0, 0, 30, 40, 50},
		},
		{
			{0, {10, 20, 30}},
			{2, {30, 40, 50}},
		});

	do_test("test2",
		{
			{ 0, 0, 0, 0, 10, 20, 30},
			{ 0, 2, 1, 1, 30, 50, dim_t::infinite()},
		},
		{
			{0, {10, 20, 30}},
			{2, {20, 30, dim_t::infinite()}},
		});

	do_test("test3",
		{
			{ 0, 0, 0, 0, 10, 20, 30},
			{ 0, 2, 1, 1, 5, 50, dim_t::infinite()},
		},
		{
			{0, {10, 20, 30}},
			{2, {0, 30, dim_t::infinite()}},
		});

	do_test("test4",
		{
			{ 2, 2, 0, 0, 10, 20, 30},
			{ 0, 2, 1, 1, 5, 50, dim_t::infinite()},
		},
		{
			{0, {0, 30, dim_t::infinite()}},
			{2, {10, 20, 30}},
		});

	do_test("test5",
		{
			{ 1, 1, 0, 0, 10, 20, 30},
			{ 0, 2, 1, 1, 5, 50, dim_t::infinite()},
		},
		{
			{0, {0, 15, dim_t::infinite()}},
			{1, {10, 20, 30}},
			{2, {0, 15, dim_t::infinite()}},
		});

	do_test("test6",
		{
			{ 1, 1, 0, 0, 10, 20, 30},
			{ 0, 1, 1, 1, 15, 15, 20},
		},
		{
			{0, {5, 5, 10}},
			{1, {10, 10, 10}},
		});

}

int main()
{
	try {
		testgrid();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
}
