#include "metrics_grid.H"
#include <x/exception.H>
#include "metrics_horizvert.H"
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

class myhorizvertObj : public horizvertObj {

public:
	using horizvertObj::horizvertObj;

	void horizvert_updated(IN_THREAD_ONLY) override
	{
	}
};

typedef ref<myhorizvertObj> myhorizvert;

static void do_test(const char *testname,
		    const std::vector<testgrid_info> &test_info,
		    const grid_metrics_t &res)
{
	grid_t g;

	for (const auto &info: test_info)
	{
		myhorizvert metrics=myhorizvert::create(info.min, info.pref, info.max);

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

static void do_add(const char *testname,
		   axis a,
		   axis b,
		   axis sum)
{
	auto res=a+b;

	if (res != sum)
		throw EXCEPTION(testname << " failed: got "
				<< res << ", expected " << sum);
}

static void do_size(const char *testname,
		    const std::vector<axis> &axises,
		    dim_t target_size,
		    const grid_sizes_t &expected_result)
{
	grid_metrics_t m;

	grid_xy c=0;
	std::for_each(axises.begin(), axises.end(),
		      [&]
		      (const axis &a)
		      {
			      m[c++]=a;
		      });

	grid_sizes_t s;

	calculate_grid_size(m, s, target_size);

	if (s != expected_result)
	{
		std::ostringstream o;

		const char *sep="";

		std::for_each(s.begin(), s.end(),
			      [&]
			      (auto v)
			      {
				      o << sep << v;
				      sep=", ";
			      });

		throw EXCEPTION(testname << ": wrong result: "
				<< o.str());
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

	do_add("add1",
	       {10, 20, 30},
	       {30, 40, 50},
	       {40, 60, 80});

	do_add("add2",
	       {dim_t::infinite()-1, dim_t::infinite()-1, dim_t::infinite()-1},
	       {30, 40, 50},
	       {dim_t::infinite()-1, dim_t::infinite()-1, dim_t::infinite()-1});

	do_add("add3",
	       {10, 20, dim_t::infinite()},
	       {30, 40, 50},
	       {40, 60, dim_t::infinite()});
	do_add("add4",
	       {10, 20, 30},
	       {30, 40, dim_t::infinite()},
	       {40, 60, dim_t::infinite()});

	do_size("size1",
		{ {10, 20, 30}, {40, 80, 100} },
		25,
		{5, 20});

	do_size("size2",
		{ {10, 20, 30}, {40, 80, 100} },
		50,
		{10, 40});

	do_size("size3",
		{ {10, 20, 30}, {40, 80, 100} },
		75,
		{15, 60});

	do_size("size4",
		{ {10, 20, 30}, {40, 80, 100} },
		100,
		{20, 80});

	do_size("size5",
		{ {10, 20, 30}, {40, 80, 100} },
		115,
		{25, 90});

	do_size("size6",
		{ {10, 20, 30}, {40, 80, 100} },
		130,
		{30, 100});

	do_size("size7",
		{ {10, 20, 30}, {40, 80, 100} },
		150,
		{30, 100});

	do_size("size8",
		{
			{10, 20, 30},
			{40, 80, dim_t::infinite()},
			{50, 90, dim_t::infinite()},
		},
		190,
		{20, 80, 90});

	do_size("size9",
		{
			{10, 20, 30},
			{40, 80, dim_t::infinite()},
			{50, 90, dim_t::infinite()},
		},
		200,
		{30, 80, 90});

	do_size("size10",
		{
			{10, 20, 30},
			{40, 80, dim_t::infinite()},
			{50, 90, dim_t::infinite()},
		},
		300,
		{30, 130, 140});
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
