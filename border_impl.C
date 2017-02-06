/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "border_impl.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

border_implObj::border_implObj()=default;

border_implObj::border_implObj(const border_info &b)
	: border_info(b)
{
}

border_implObj::~border_implObj()=default;

border_impl border_implObj::clone() const
{
	const border_info &me=*this;

	return border_impl::create(me);
}

void border_implObj::calculate()
{
	calculated_border_width=dim_t::truncate(width + inner_radius()*2);
	calculated_border_height=dim_t::truncate(height + inner_radius()*2);

	calculated_dashes_sum=0;

	for (const auto &dash:dashes)
		calculated_dashes_sum=dim_t::truncate(calculated_dashes_sum
						      + dash);

	if (dashes.size() % 2)
		calculated_dashes_sum=dim_t::truncate(calculated_dashes_sum*2);

	if (calculated_dashes_sum == 0)
		// Some wise guy gave me a dash
		// pattern that added up to 65536.
		calculated_dashes_sum=1;

	for (const auto &dash:dashes)
		if (dash == 0)
			throw EXCEPTION("Dash size specified as 0 pixels");
}

LIBCXXW_NAMESPACE_END
