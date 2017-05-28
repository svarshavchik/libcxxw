/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/exception.H>
#include <iostream>
#include <vector>
#include <algorithm>
#include "metrics_axis.H"
#include "x/w/metrics/derivedaxis.H"

using namespace LIBCXX_NAMESPACE::w::metrics;

typedef LIBCXX_NAMESPACE::w::dim_t dim_t;
typedef LIBCXX_NAMESPACE::w::dim_squared_t dim_squared_t;

void do_check(std::vector<axis> &v,
	      const char *test_name,
	      dim_t::value_type minres, dim_t::value_type prefres,
	      dim_t::value_type maxres)
{
	auto d=create_derived_axis_obj();

	std::vector<derived_axis_obj::base::current_value_t> current_values;

	std::for_each(v.begin(), v.end(),
		      [&]
		      (const auto &a)
		      {
			      current_values.push_back(d->create(a));
		      });

	typedef derived_axis_obj::base::vipobj_t vipobj_t;

	auto res=*vipobj_t::readlock(*d);

	if (res.minimum() != minres)
		throw EXCEPTION("Expected minimum " << minres
				<< ", got " << res.minimum()
				<< " (" << test_name << ")");

	if (res.preferred() != prefres)
		throw EXCEPTION("Expected preferred " << prefres
				<< ", got " << res.preferred()
				<< " (" << test_name << ")");

	if (res.maximum() != maxres)
		throw EXCEPTION("Expected maximum " << maxres
				<< ", got " << res.maximum()
				<< " (" << test_name << ")");
}

template<typename ...more_values>
void do_check(std::vector<axis> &v,
	      const char *test_name,
	      dim_t::value_type min,
	      dim_t::value_type pref,
	      dim_t::value_type max,

	      dim_t::value_type something_else,
	      more_values ...and_more)
{
	v.emplace_back(min, pref, max);

	do_check(v, test_name, something_else, and_more...);
}

template<typename ...list_of_values>
void check(const char *test_name,
	   list_of_values ...values)
{
	std::vector<axis> v;

	do_check(v, test_name, values...);
}

static void checkdivide_compare(const char *name,
				size_t n,
				const std::vector<dim_t> &computed_values)
{
}

template<typename ...list_of_values>
static void checkdivide_compare(const char *name,
				size_t n,
				const std::vector<dim_t> &computed_values,
				dim_t::value_type v,
				list_of_values ...values)
{
	if (computed_values[n] != v)
	{
		throw EXCEPTION(name << ": expected " << v << " for value #"
				<< n << ", got " << computed_values[n]);
	}

	checkdivide_compare(name, n+1, computed_values, values...);
}

template<typename ...list_of_values>
void checkdivide(const char *name,
		 dim_t::value_type min,
		 dim_t::value_type pref,
		 dim_t::value_type max,
		 dim_t::value_type divide_into,
		 list_of_values ...values)
{
	axis a{min, pref, max};

	std::vector<dim_t> computed_values;

	a.divide(divide_into, [&]
		 (const axis &a)
		 {
			 computed_values.push_back(a.minimum());
			 computed_values.push_back(a.preferred());
			 computed_values.push_back(a.maximum());
		 });

	if (computed_values.size() != sizeof...(values))
		throw EXCEPTION(name << ": expected "
				<< sizeof...(values) << " values, got "
				<< computed_values.size());

	checkdivide_compare(name, 0, computed_values,
			    values...);
}

static void collect(std::vector<axis> &v)
{
}

template<typename ...list_of_values>
void collect(std::vector<axis> &v,
	     dim_t minimum, dim_t preferred, dim_t maximum,
	     list_of_values ...values)
{
	v.emplace_back(minimum, preferred, maximum);
	collect(v, std::forward<list_of_values>(values)...);
}

template<typename ...list_of_values>
void checkspread(const char *testname,
		 dim_squared_t expected_minimum,
		 std::tuple<dim_squared_t, bool> expected_maximum,
		 list_of_values ...values)
{
	std::vector<axis> axises;

	axises.reserve(sizeof...(values)/3);

	collect(axises, values...);

	auto minimum=axis::total_minimum(axises.begin(), axises.end());

	auto res=axis::total_maximum(axises.begin(), axises.end());

	if (minimum != expected_minimum)
		throw EXCEPTION(testname << ": expected minimum of "
				<< expected_minimum << ", got "
				<< minimum);

	if (res.sum_excluding_infinite != std::get<0>(expected_maximum))
		throw EXCEPTION(testname << ": expected maximum of "
				<< std::get<0>(expected_maximum) << ", got "
				<< res.sum_excluding_infinite);

	if (res.has_infinites != std::get<1>(expected_maximum))
		throw EXCEPTION(testname << ": expected infinite flag of "
				<< std::get<1>(expected_maximum) << ", got "
				<< res.has_infinites);
}

void checksortby(const char *testname,
		 const std::vector<axis> &unsorted_list,
		 const std::vector<axis> &sorted_list)
{
	auto res=axis::sort_sequence_by(unsorted_list.begin(),
					unsorted_list.end(),
					[]
					(const auto &a, const auto &b)
					{
						return a.minimum() < b.minimum();
					});

	std::vector<axis> v;
	v.resize(unsorted_list.size());

	std::transform(res.begin(), res.end(), v.begin(),
		       []
		       (auto iter)
		       {
			       return *iter;
		       });

	if (v != sorted_list)
		throw EXCEPTION(testname << " failed");
}

void test_increase_minimum_by(const char *testname,
			      const axis &from,
			      dim_t by,
			      const axis &to)
{
	auto res=from.increase_minimum_by(by);

	if (res != to)
		throw EXCEPTION(testname << ": expected " << to
				<< ", got " << res);
}

void test_decrease_maximum_by(const char *testname,
			      const axis &from,
			      dim_t by,
			      const axis &to)
{
	auto res=from.decrease_maximum_by(by);

	if (res != to)
		throw EXCEPTION(testname << ": expected " << to
				<< ", got " << res);
}

void test_increase_minimums_by(const char *testname,
			       std::vector<axis> axises,
			       dim_squared_t howmuch,
			       const std::vector<axis> &result)
{
	axis::adjust_minimums_by(axises.begin(),
				 axises.end(),
				 howmuch);

	if (axises != result)
		throw EXCEPTION(testname << " failed");
}

void test_decrease_maximums_by(const char *testname,
			       std::vector<axis> axises,
			       dim_squared_t howmuch,
			       const std::vector<axis> &result)
{
	axis::adjust_maximums_by(axises.begin(),
				 axises.end(),
				 howmuch);

	if (axises != result)
		throw EXCEPTION(testname << " failed");
}

int main()
{
	try {
		check("empty list", 0, 0, dim_t::infinite());

		check("two values",
		      20, 30, 100,
		      40, 80, 120,
		      40, 55, 100);

		check("minimum preferred",
		      20, 20, 80,
		      50, 60, 500,
		      50, 50, 80);

		check("maximum preferred",
		      50, 400, 500,
		      20, 20, 80,
		      50, 80, 80);

		check("noone's happy",
		      10, 20, 30,
		      100, 110, 120,
		      100, 100, 100);

		checkdivide("basic division",
			    101, 108, 115,
			    2,
			    50, 53, 56,
			    51, 55, 59);

		checkspread("spread1",
			    50,
			    {90, false},
			    10, 20, 30,
			    40, 50, 60);

		checkspread("spread2",
			    50,
			    {30, true},
			    10, 20, 30,
			    40, 50, dim_t::infinite());

		checkspread("spread3",
			    50,
			    {dim_t::infinite()-1+60, false},
			    10, 20, dim_t::infinite()-1,
			    40, 50, 60);

		checksortby("sort1",
			    {{30, 40, 50}, {20, 30, 60}, {20, 40, 50}},
			    {{20, 30, 60}, {20, 40, 50}, {30, 40, 50}});

		checksortby("sort2",
			    {{20, 30, 60}, {30, 40, 50}, {20, 40, 50}},
			    {{20, 30, 60}, {20, 40, 50}, {30, 40, 50}});

		checksortby("sort3",
			    {{20, 30, 60}, {20, 40, 50}, {30, 40, 50}},
			    {{20, 30, 60}, {20, 40, 50}, {30, 40, 50}});

		test_increase_minimum_by("increase1",
					 {20, 30, 60},
					 5,
					 {25, 30, 60});

		test_increase_minimum_by("increase2",
					 {20, 30, 60},
					 15,
					 {35, 35, 60});

		test_increase_minimum_by("increase3",
					 {20, 30, 60},
					 45,
					 {65, 65, 65});

		test_increase_minimum_by("increase4",
					 {dim_t::infinite()-1,
					  dim_t::infinite()-1,
					  dim_t::infinite()},
					 15,
					 {dim_t::infinite()-1,
					  dim_t::infinite()-1,
					  dim_t::infinite()}
					 );

		test_decrease_maximum_by("decrease1",
					 {20, 30, 60},
					 5,
					 {20, 30, 55});

		test_decrease_maximum_by("decrease2",
					 {20, 30, 60},
					 35,
					 {20, 25, 25});

		test_decrease_maximum_by("decrease3",
					 {20, 30, 60},
					 45,
					 {20, 20, 20});

		test_decrease_maximum_by("decrease4",
					 {20, 30, dim_t::infinite()},
					 45,
					 {20, 30, dim_t::infinite()});

		test_increase_minimums_by
			("incrmin1",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 5,
			 { {15, 20, 30}, {20, 25, 30}, {30, 40, 50} });

		test_increase_minimums_by
			("incrmin2",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 15,
			 { {20, 20, 30}, {20, 25, 30}, {35, 40, 50} });

		test_increase_minimums_by
			("incrmin3",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 20,
			 { {20, 20, 30}, {20, 25, 30}, {40, 40, 50} });

		test_increase_minimums_by
			("incrmin4",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 22,
			 { {21, 21, 30}, {20, 25, 30}, {41, 41, 50} });

		test_increase_minimums_by
			("incrmin5",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 25,
			 { {22, 22, 30}, {21, 25, 30}, {42, 42, 50} });

		test_increase_minimums_by
			("incrmin6",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 50,
			 { {30, 30, 30}, {30, 30, 30}, {50, 50, 50} });

		test_increase_minimums_by
			("incrmin7",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 52,
			 { {31, 31, 31}, {30, 30, 30}, {51, 51, 51} });

		test_increase_minimums_by
			("incrmin8",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 55,
			 { {32, 32, 32}, {31, 31, 31}, {52, 52, 52} });

		test_increase_minimums_by
			("incrmin9",
			 { {10, 20, 30} },
			 dim_t::infinite(),
			 { {dim_t::infinite()-1, dim_t::infinite()-1,
						 dim_t::infinite()-1}});

		test_decrease_maximums_by
			("decrmax1",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 5,
			 { {10, 20, 25}, {20, 25, 30}, {30, 40, 50} });

		test_decrease_maximums_by
			("decrmax2",
			 { {10, 20, 30}, {20, 25, 30}, {30, 40, 50} },
			 dim_t::infinite(),
			 { {10, 10, 10}, {20, 20, 20}, {30, 30, 30} });

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
