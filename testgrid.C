#include "metrics_grid.H"
#include <x/exception.H>
#include "metrics_horizvert.H"
#include "metrics_grid_axisrange.H"
#include "metrics_grid_pos.H"
#include "rectangle.H"

#include <sstream>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;
using namespace LIBCXX_NAMESPACE::w::metrics;

struct testgrid_info {
	grid_xy x1, x2, y1, y2;
	dim_t min, pref, max;
};

class LIBCXX_HIDDEN myhorizvertObj : public horizvertObj {

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
	std::list<pos_axis> g;

	for (const auto &info: test_info)
	{
		myhorizvert metrics=myhorizvert::create();

		metrics->horiz={info.min, info.pref, info.max};

		auto gp=grid_pos::create();

		gp->horiz_pos.start=info.x1;
		gp->horiz_pos.end=info.x2;
		gp->vert_pos.start=info.y1;
		gp->vert_pos.end=info.y2;

		g.push_back({gp, metrics});
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
		    const std::unordered_map<grid_xy, int> &axis_sizes,
		    const std::vector<dim_t> &expected_result_sizes)
{
	grid_sizes_t expected_result;

	coord_t p=0;
	grid_xy xy=0;

	for (const auto &s:expected_result_sizes)
	{
		expected_result[xy]={p, s};

		p=(coord_squared_t::value_type)(p+s);
		++xy;
	}

	grid_metrics_t m;

	grid_xy c=0;
	std::for_each(axises.begin(), axises.end(),
		      [&]
		      (const axis &a)
		      {
			      m[c++]=a;
		      });

	grid_sizes_t s;

	calculate_grid_size(m, s, target_size,
			    [&]
			    (const auto &xy)
			    {
				    auto iter=axis_sizes.find(xy);

				    if (iter == axis_sizes.end())
					    return -1;

				    return iter->second;
			    });

	if (s != expected_result)
	{
		std::ostringstream o;

		const char *sep="";

		std::for_each(s.begin(), s.end(),
			      [&]
			      (const auto &v)
			      {
				      o << sep << v.first << ": ("
					<< std::get<coord_t>(v.second)
					<< ", "
					<< std::get<dim_t>(v.second)
					<< ")";

				      sep=", ";
			      });

		throw EXCEPTION(testname << ": wrong result: "
				<< o.str());
	}
}

void do_rectmerge(const char *testname,
		  rectangle_set rectangles,
		  const rectangle_set &expected)
{
	merge(rectangles);

	if (rectangles != expected)
	{
		std::ostringstream o;
		const char *sep="";

		std::for_each(rectangles.begin(), rectangles.end(),
			      [&]
			      (const auto &r)
			      {
				      o << sep << r;
				      sep="; ";
			      });

		throw EXCEPTION("testname failed: " << o.str());
	}
}


static dim_squared_t total_area(const rectangle_set &r)
{
	dim_squared_t sum{0};

	std::for_each(r.begin(), r.end(),
		      [&]
		      (const rectangle &r)
		      {
			      sum += r.width * r.height;
		      });

	return sum;
}

void do_rectmergearea(const char *testname,
		      rectangle_set rectangles,
		      size_t nrectangles,
		      dim_squared_t expected_total_area)
{
	merge(rectangles);

	if (rectangles.size() != nrectangles ||
	    total_area(rectangles) != expected_total_area)
	{
		std::ostringstream o;
		const char *sep="";

		std::for_each(rectangles.begin(), rectangles.end(),
			      [&]
			      (const auto &r)
			      {
				      o << sep << r;
				      sep="; ";
			      });

		throw EXCEPTION(testname << " failed: " << o.str());
	}
}

void do_testslice(const char *testname,
		  const rectangle_set &slicee,
		  const rectangle_set &slicer,
		  const rectangle_set &right_result)
{
	rectangle_slicer rslicer{slicee, slicer};

	rslicer.slice_slicee();

	rectangle_set
		result{rslicer.slicee_v.begin(), rslicer.slicee_v.end()};

	if (result != right_result)
	{
		std::ostringstream o;
		const char *sep="";

		std::for_each(result.begin(), result.end(),
			      [&]
			      (const auto &r)
			      {
				      o << sep << r;
				      sep="; ";
			      });

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
		50, {},
		{10, 40});

	do_size("size2",
		{ {10, 20, 30}, {40, 80, 100} },
		50, {},
		{10, 40});

	do_size("size3",
		{ {10, 20, 30}, {40, 80, 100} },
		75, {},
		{15, 60});

	do_size("size4",
		{ {10, 20, 30}, {40, 80, 100} },
		100, {},
		{20, 80});

	do_size("size5",
		{ {10, 20, 30}, {40, 80, 100} },
		115, {},
		{25, 90});

	do_size("size6",
		{ {10, 20, 30}, {40, 80, 100} },
		130, {},
		{30, 100});

	do_size("size7",
		{ {10, 20, 30}, {40, 80, 100} },
		150,
		{{0, 0}, {1, 0}},
		{30, 100});

	do_size("size8",
		{
			{10, 20, 30},
			{40, 80, dim_t::infinite()},
			{50, 90, dim_t::infinite()},
		},
		190, {},
		{20, 80, 90});

	do_size("size9",
		{
			{10, 20, 30},
			{40, 80, dim_t::infinite()},
			{50, 90, dim_t::infinite()},
		},
		200, {},
		{30, 80, 90});

	do_size("size10",
		{
			{10, 20, 30},
			{40, 80, dim_t::infinite()},
			{50, 90, dim_t::infinite()},
		},
		300, {},
		{30, 130, 140});

	do_size("size11",
		{
			{10, 10, 10},
		},
		10, {},
		{10});

	do_size("size12",
		{
			{10, 15, 30},
			{10, 10, 10},
			{10, 10, 10}
		},
		100, {{0, 0}, {1, 0}, {2, 0}},
		{30, 10, 10});

	do_size("size13",
		{
			{10, 15, 30},
			{10, 10, 10},
			{10, 10, 10}
		},
		100,
		{ {0, 5}, {1, 15}, {2, 20} },
		{30, 15, 20});

	do_size("size14",
		{
			{20, 20, 20},
			{20, 20, 20},
			{20, 20, 20},
			{10, 10, 10},
		},
		100,
		{ {0, 5}, {1, 0}, {2, 65}, {3, 100} },
		{20, 20, 30, 30});

	do_size("size15",
		{
			{10, 10, 10},
			{10, 10, 10},
		},
		100,
		{ {0, 25} },
		{25, 75});

	do_rectmerge("rectmerge1",
		     { {0, 0, 1, 1}, {1, 0, 1, 1}, {2, 0, 1, 2} },

		     { {0, 0, 2, 1}, {2, 0, 1, 2} });

	do_rectmerge("rectmerge2",
		     { {0, 0, 2, 1}, {0, 1, 1, 1}, {1, 1, 1, 1} },

		     { {0, 0, 2, 2} });

	do_rectmerge("rectmerge3",
		     { {0, 0, 1, 2}, {1, 0, 1, 1}, {1, 1, 1, 1} },

		     { {0, 0, 2, 2} });

	do_rectmergearea("rectmerge3",
			 { {0, 0, 1, 1}, {0, 1, 1, 1}, {1, 1, 1, 1} },

			 2, 3);


	do_testslice("slice1",
		     { {10, 10, 10, 10} },

		     { {0, 0, 2, 20},
		       {18, 2, 1, 2},
		       {30, 0, 1, 30},
		       {19, 30, 1, 1},

		       {6, 6, 5, 30},
		       {11, 6, 3, 30},
		       {15, 6, 5, 30},
		     },
		     {
			     {10, 10, 1, 10},
			     {11, 10, 3, 10},
			     {14, 10, 1, 10},
			     {15, 10, 5, 10},
		     });

	{
		auto res=add(rectangle_set( {{0, 0, 2, 2}} ),
			     rectangle_set(),
			     1, 2);

		if (res != rectangle_set( {{1, 2, 2, 2}}))
			throw EXCEPTION("Offset in add() don't work");
	}

	{
		auto res=add(rectangle_set( {{10, 10, 10, 10}} ),
			     rectangle_set( {{15, 15, 10, 10}} ));

		if (total_area(res) != 10 * 10 * 2 - 5 * 5)
		{
			std::ostringstream o;
			const char *sep="";

			std::for_each(res.begin(), res.end(),
				      [&]
				      (const auto &r)
				      {
					      o << sep << r;
					      sep="; ";
				      });

			throw EXCEPTION("add failed: " << o.str());
		}
	}

	{
		auto res=subtract(rectangle_set( {{10, 10, 10, 10}} ),
				  rectangle_set( {{10, 10, 5, 10}} ),
				  -15, -10);

		if (res != rectangle_set( {{0, 0, 5, 10}} ))
			throw EXCEPTION("Subtraction offset did not work");
	}

	{
		auto res=subtract(rectangle_set( {{10, 10, 10, 10}} ),
				  rectangle_set( {{15, 15, 10, 10}} ));

		if (total_area(res) != 10 * 10 - 5 * 5)
		{
			std::ostringstream o;
			const char *sep="";

			std::for_each(res.begin(), res.end(),
				      [&]
				      (const auto &r)
				      {
					      o << sep << r;
					      sep="; ";
				      });

			throw EXCEPTION("operator- failed: " << o.str());
		}
	}

	{
		auto res=intersect(rectangle_set( {{10, 10, 10, 10}} ),
				   rectangle_set( {{15, 15, 10, 10}} ),
				   -5, -10);

		if (res != rectangle_set({{10, 5, 5, 5}}))
		{
			std::ostringstream o;
			const char *sep="";

			std::for_each(res.begin(), res.end(),
				      [&]
				      (const auto &r)
				      {
					      o << sep << r;
					      sep="; ";
				      });

			throw EXCEPTION("operator* failed: " << o.str());
		}
	}
}

void testrectangleset()
{
	rectangle_set a={{0, 0, 1, 1}, {1, 0, 1, 1}, {2, 0, 1, 1}};
	rectangle_set b={{3, 0, 0, 0}}; // This should be quietly erased.

	auto result = add(a,b);

	if (result != rectangle_set{ {0, 0, 3, 1}})
		throw EXCEPTION("testrectangleset failed");


	if (bounds({ {1,1,1,1},{0,0,1,1},{2,2,1,1}}) != rectangle{0,0,3,3})
		throw EXCEPTION("bounds() failed");
}

int main()
{
	try {
		testrectangleset();
		testgrid();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
}
