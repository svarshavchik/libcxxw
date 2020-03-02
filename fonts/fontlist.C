/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontlist_impl.H"
#include "fonts/fontconfig_impl.H"
#include "fonts/fontpattern_impl.H"
#include "fonts/fontobjectset.H"
#include "fonts/fontconfig.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

listObj::listObj(const ref<implObj> &implArg) : impl(implArg)
{
}

listObj::~listObj()
{
}

size_t listObj::size() const
{
	implObj::s_t::lock lock(impl->s);

	return (*lock)->nfont;
}


listObj::iterator listObj::begin()
{
	return iterator(impl, 0);
}

listObj::iterator listObj::end()
{
	return iterator(impl, size());
}

listObj::const_iterator listObj::begin() const
{
	return const_iterator(impl, 0);
}

listObj::const_iterator listObj::end() const
{
	return const_iterator(impl, size());
}

///////////////////////////////////////////////////////////////////////////////

listObj::const_iterator::const_iterator(const ref<implObj> &lArg, size_t pArg)
	: l(lArg), p(pArg)
{
}

listObj::const_iterator::~const_iterator()
{
}

listObj::const_iterator::const_iterator(const const_iterator &)=default;

listObj::const_iterator::const_iterator(const_iterator &&)=default;

listObj::const_iterator &listObj::const_iterator
::operator=(const const_iterator &)=default;

listObj::const_iterator &listObj::const_iterator::operator=(const_iterator &&)
=default;

pattern listObj::const_iterator::operator*() const
{
	return pattern::create(l->get_pattern(p));
}

listObj::const_iterator listObj::const_iterator::operator++(int)
{
	auto cpy= *this;

	operator++();
	return cpy;
}

listObj::const_iterator listObj::const_iterator::operator--(int)
{
	auto cpy=*this;

	operator--();
	return cpy;
}

listObj::const_iterator listObj::const_iterator::operator+(std::ptrdiff_t o)
	const
{
	auto cpy=*this;

	cpy += o;
	return cpy;
}

listObj::const_iterator listObj::const_iterator::operator-(std::ptrdiff_t o)
	const
{
	auto cpy=*this;

	cpy -= o;
	return cpy;
}

//! Iterator listObj::const_iterator::operator
pattern listObj::const_iterator::operator[](std::ptrdiff_t o) const
{
	auto cpy=*this;

	cpy += o;

	return *cpy;
}

listObj::iterator::iterator(const ref<implObj> &lArg, size_t pArg)
	: const_iterator(lArg, pArg)
{
}

listObj::iterator::~iterator()
{
}

listObj::iterator::iterator(const iterator &)=default;

listObj::iterator::iterator(iterator &&)=default;

listObj::iterator &listObj::iterator::operator=(const iterator &)=default;

listObj::iterator &listObj::iterator::operator=(iterator &&)=default;

listObj::iterator listObj::iterator::operator++(int)
{
	auto cpy= *this;

	operator++();

	return cpy;
}

listObj::iterator listObj::iterator::operator--(int)
{
	auto cpy= *this;

	operator--();

	return cpy;
}

listObj::iterator listObj::iterator::operator+(std::ptrdiff_t o) const
{
	auto cpy= *this;

	cpy += o;

	return cpy;
}

listObj::iterator listObj::iterator::operator-(std::ptrdiff_t o) const
{
	auto cpy= *this;

	cpy -= o;

	return cpy;
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

