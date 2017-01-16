/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "metrics_axis.H"
#include "screen.H"
#include "messages.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif

exception axis::invalid_infinite()
{
	return EXCEPTION(_("Cannot specify an infinite minimum or preferred metric."));

}

exception axis::invalid_minimum()
{
	return EXCEPTION(_("Preferred metric less than the minimum one."));
}

exception axis::invalid_maximum()
{
	return EXCEPTION(_("Preferred metric is more than the maximum one."));
}

static dim_t convert_from_double(axis::hv which, const screen &s,
				 double n)
{
	if (n < 0)
		throw EXCEPTION("Metric cannot be negative");

	return (which == axis::horizontal ?
		s->impl->compute_width(n):s->impl->compute_height(n));
}

axis::axis(hv which,
	   const screen &s,
	   double minimum,
	   double preferred,
	   double maximum)
	: axis(convert_from_double(which, s, minimum),
	       convert_from_double(which, s, preferred),
	       std::isinf(maximum) ? dim_t::infinite()
	       : convert_from_double(which, s, maximum))
{
}

axis axis::increase_minimum_by(dim_t howmuch) const
{
	auto m=max_increase_minimum_by();

	if (m < howmuch)
		howmuch=m;

	dim_t new_minimum=(dim_squared_t::value_type)(howmuch + minimum());

	auto new_preferred=preferred();

	if (new_preferred < new_minimum)
		new_preferred=new_minimum;

	auto new_maximum=maximum();

	if (new_maximum < new_preferred)
		new_maximum=new_preferred;

	return {new_minimum, new_preferred, new_maximum};
}

axis axis::decrease_maximum_by(dim_t howmuch) const
{
	auto m=max_decrease_maximum_by();

	if (m < howmuch)
		howmuch=m;

	dim_t new_maximum=maximum()-howmuch;

	auto new_preferred=preferred();

	if (new_preferred > new_maximum)
		new_preferred=new_maximum;

	return {minimum(), new_preferred, new_maximum};
}

axis &axis::operator &= (const axis &other)
{
	dim_t target_preferred=other.preferred() > preferred()
		? other.preferred() - (other.preferred() - preferred())/2
		: preferred() - (preferred()-other.preferred())/2;

	if (other.minimum() > minimum())
		*this=increase_minimum_by(other.minimum() - minimum());

	if (other.maximum() != dim_t::infinite())
	{
		if (maximum() == dim_t::infinite())
			--maximum_; // So that decrease_maximum_by() works.

		if (other.maximum() < maximum())
			*this=decrease_maximum_by(maximum() - other.maximum());
	}

	if (target_preferred < minimum())
		target_preferred=minimum();

	if (target_preferred > maximum())
		target_preferred=maximum();

	if (target_preferred == dim_t::infinite())
		--target_preferred; // ?

	preferred_=target_preferred;
	return *this;
}

axis axis::operator+(const axis &other) const
{
	// If the sum total of both minimums' exceeds the largest possible
	// minimum value, return the largest possible minimum value

	dim_t min=dim_t::infinite()-1;

	if (min-minimum() > other.minimum())
	{
		min=(dim_t::value_type)minimum() +
			(dim_t::value_type)other.minimum();
	}

	// Ditto for preferred

	dim_t pref=dim_t::infinite()-1;

	if (pref-preferred() > other.preferred())
	{
		pref=(dim_t::value_type)preferred() +
			(dim_t::value_type)other.preferred();
	}

	// Ditto for maximums, unless one or the other is infinite, in which
	// case the result is infinite.

	dim_t max=dim_t::infinite();

	if (maximum() != dim_t::infinite() &&
	    other.maximum() != dim_t::infinite())
	{
		--max;

		if (max-maximum() > other.maximum())
		{
			max=(dim_t::value_type)maximum() +
				(dim_t::value_type)other.maximum();
		}
	}

	return {min, pref, max};
}

std::ostream &operator<<(std::ostream &o,
			 const axis &a)
{
	return o << "minimum=" << a.minimum()
		 << ", preferred=" << a.preferred()
		 << ", maximum=" << a.maximum();
}

derived_axis_obj create_derived_axis_obj()
{
	return x::derivedvalues<axis>
		::create( []
			  {
				  return derivedaxis();
			  },
			  []
			  (auto &derived, const auto &axis)
			  {
				  derived(axis);
			  },
			  []
			  (const auto &derived)
			  {
				  return static_cast<axis>(derived);
			  });
}
#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
