/*
** Copyright 2017 Double Precision, Inc.
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
			}},
		r);
}


size_t hash<LIBCXX_NAMESPACE::w::border_infomm>
::operator()(const LIBCXX_NAMESPACE::w::border_infomm &b) const noexcept
{
	auto s=b.width * 10000 + b.height * 100 +
		b.radius * 10 + (b.rounded ? 1:0);

	for (const auto dash:b.dashes)
		s += dash;

	size_t h=s;

	for (const auto &color:b.colors)
		h += hash<LIBCXX_NAMESPACE::w::color_arg>::operator()(color);

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
				return this->hash<string>::operator()(s);
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
