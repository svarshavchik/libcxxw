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
	v.emplace_back(dim_t{min}, dim_t{pref}, dim_t{max});

	do_check(v, test_name, something_else, and_more...);
}

template<typename ...list_of_values>
void check(const char *test_name,
	   list_of_values ...values)
{
	std::vector<axis> v;

	do_check(v, test_name, values...);
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
		      65, 65, 65);

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
