/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/dim_arg_hash.H"
#include "x/w/rgb_hash.H"
#include "x/w/border_infomm_hash.H"
#include "x/w/border_arg_hash.H"
#include <x/refptr_hash.H>
#include <x/visitor.H>
#include <variant>
#include <cmath>

namespace std {
#if 0
}
#endif

size_t hash<LIBCXX_NAMESPACE::w::dim_arg>
::operator()(const LIBCXX_NAMESPACE::w::dim_arg &d)
	const noexcept
{
	return visit(LIBCXX_NAMESPACE::visitor{
			[]
			(double v)
			{
				return (size_t)(v*10);
			},
			[this]
			(const string &s)
			{
				return this->hash<string>::operator()(s);
			}}, d);
}


size_t hash<LIBCXX_NAMESPACE::w::color_arg>
::operator()(const LIBCXX_NAMESPACE::w::color_arg &r) const noexcept
{
	return visit(LIBCXX_NAMESPACE::visitor{
			[]
			(const LIBCXX_NAMESPACE::w::rgb &r)
			{
				return (size_t)r.value();
			},
			[this]
			(const string &s)
			{
				return this->hash<string>::operator()(s);
			},
			[this]
			(const LIBCXX_NAMESPACE::w::linear_gradient &g)
			{
				return this->hash<LIBCXX_NAMESPACE::w
						  ::rgb_gradient>::operator()
					(g.gradient) +
					(size_t)(std::round(g.x1*256)) +
					(size_t)(std::round(g.y1*65536)) +
					(size_t)(std::round(g.x2*
							    (65536.0*256.0)))
					+
					(size_t)(std::round(g.y2*
							    (65536.0*65536.0)))
					+ (size_t)(std::round(g.fixed_width
							      * 64))
					+ (size_t)(std::round(g.fixed_height
							      * 16));
			},
			[this]
			(const LIBCXX_NAMESPACE::w::radial_gradient &g)
			{
				return this->hash<LIBCXX_NAMESPACE::w
						  ::rgb_gradient>::operator()
					(g.gradient) +
					(size_t)(std::round(g.inner_center_x
							    *256)) +
					(size_t)(std::round(g.inner_center_y
							    *65536)) +
					(size_t)(std::round(g.outer_center_x*
							    (65536.0*256.0)))
					+
					(size_t)(std::round(g.outer_center_y*
							    (65536.0*65536.0)))
					+ (size_t)(std::round(g.inner_radius
							      * 128))
					+ (size_t)(std::round(g.outer_radius
							      * 16384))
					+ (size_t)(std::round(g.fixed_width
							      * 64))
					+ (size_t)(std::round(g.fixed_height
							      * 16));

			}},
		r);
}


size_t hash<LIBCXX_NAMESPACE::w::border_infomm>
::operator()(const LIBCXX_NAMESPACE::w::border_infomm &b) const noexcept
{
	auto s=hash<LIBCXX_NAMESPACE::w::dim_arg>::operator()(b.width) * 10000
		* b.width_scale
		+ hash<LIBCXX_NAMESPACE::w::dim_arg>::operator()(b.height)
		* 1000 * b.height_scale
		+ hash<LIBCXX_NAMESPACE::w::dim_arg>::operator()(b.hradius)
		* 100 * b.hradius_scale
		+ hash<LIBCXX_NAMESPACE::w::dim_arg>::operator()(b.vradius)
		* 10 * b.vradius_scale
		+ (b.rounded ? 1:0);

	for (const auto dash:b.dashes)
		s += dash;

	size_t h=s;

	h += hash<LIBCXX_NAMESPACE::w::color_arg>::operator()(b.color1);

	if (b.color2)
		h += hash<LIBCXX_NAMESPACE::w::color_arg>
			::operator()(*b.color2);

	return h;
}

size_t hash<LIBCXX_NAMESPACE::w::border_arg>
::operator()(const LIBCXX_NAMESPACE::w::border_arg &a) const noexcept
{
	return visit(LIBCXX_NAMESPACE::visitor{
			[this]
			(const LIBCXX_NAMESPACE::w::border_infomm &m)
			{
				return this->
					hash<LIBCXX_NAMESPACE::w::border_infomm>
					::operator()(m);
			},
			[this]
			(const string &s)
			{
				hash<LIBCXX_NAMESPACE::w::color_arg> const &r=
					*this;
				return r.hash<string>::operator()(s);
			}}, a);
}

size_t hash<LIBCXX_NAMESPACE::w::rgb_gradient>
::operator()(const LIBCXX_NAMESPACE::w::rgb_gradient &r)
	const noexcept
{
	size_t i=0;

	std::hash<LIBCXX_NAMESPACE::w::rgb_gradient::key_type> k_h{};
	std::hash<LIBCXX_NAMESPACE::w::rgb_gradient::mapped_type> v_h{};

	for (const auto &kv:r)
		i += k_h(kv.first)+v_h(kv.second);

	return i;
}

#if 0
{
#endif
}
