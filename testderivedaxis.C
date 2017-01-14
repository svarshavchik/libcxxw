/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "x/w/metrics/derivedaxis.H"
#include <x/exception.H>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace LIBCXX_NAMESPACE::w::metrics;

typedef LIBCXX_NAMESPACE::w::dim_t dim_t;
typedef LIBCXX_NAMESPACE::w::dim_squared_t dim_squared_t;

void do_check(std::vector<axis> &v,
	      const char *test_name,
	      dim_t::value_type minres, dim_t::value_type prefres,
	      dim_t::value_type maxres)
{
	axis res=std::for_each(v.begin(), v.end(), derivedaxis());

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

int main()
{
	try {
		check("empty list",
		      0, 0, 0);

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
			    {dim_t::infinite(), false},
			    10, 20, dim_t::infinite()-1,
			    40, 50, 60);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
