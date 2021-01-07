/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontcharset_impl.H"
#include "fonts/fontconfig_impl.H"

#include <x/exception.H>
LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

charsetObj::charsetObj(const ref<implObj> &implArg) : impl(implArg)
{
}

charsetObj::~charsetObj()
{
}

// Convenience typedef

typedef charsetObj::implObj::charset_t::lock charset_lock;

// Lock two different charset objects.

// Be careful and use a deterministic order to acquire the underlying locks.
//
// The constructor takes two charset implementation objects, and sets up
// "first" and "second" accordingly, but the actual locks gets acquired in
// deterministic order.

class LIBCXX_HIDDEN twocharset_lock {

	charset_lock a;
	charset_lock b;
 public:

	charset_lock &first;
	charset_lock &second;

	twocharset_lock(const ref<charsetObj::implObj> &one,
			const ref<charsetObj::implObj> &two)
		: a(one < two ? one->charset:two->charset),
		b(one < two ? two->charset:one->charset),
		first(one < two ? a:b),
		second(one < two ? b:a)
		{
		}
	~twocharset_lock()=default;
};

void charsetObj::add(char32_t c)
{
	charset_lock lock{impl->charset};

	FcCharSetAddChar(*lock, c);
}

void charsetObj::del(char32_t c)
{
	charset_lock lock{impl->charset};

	FcCharSetDelChar(*lock, c);
}

bool charsetObj::has(char32_t c) const
{
	charset_lock lock{impl->charset};

	return FcCharSetHasChar(*lock, c) == FcTrue;
}

size_t charsetObj::count() const
{
	charset_lock lock{impl->charset};

	return FcCharSetCount(*lock);
}

charset charsetObj::copy() const
{
	charset_lock lock{impl->charset};

	return charset::create(ref<implObj>
			       ::create(impl->c, FcCharSetCopy(*lock), true));
}

bool charsetObj::equal(const const_charset &cs) const
{
	twocharset_lock lock{impl, cs->impl};

	return FcCharSetEqual(*lock.first, *lock.second) == FcTrue;
}

charset charsetObj::intersect(const const_charset &cs) const
{
	twocharset_lock lock{impl, cs->impl};

	return charset::create(ref<implObj>
			       ::create(impl->c,
					FcCharSetIntersect(*lock.first,
							   *lock.second),
					true));
}

size_t charsetObj::intersect_count(const const_charset &cs) const
{
	twocharset_lock lock{impl, cs->impl};

	return FcCharSetIntersectCount(*lock.first, *lock.second);
}

bool charsetObj::is_subset(const const_charset &cs) const
{
	twocharset_lock lock{impl, cs->impl};

	return FcCharSetIsSubset(*lock.first, *lock.second) == FcTrue;
}

charset charsetObj::union_of(const const_charset &cs) const
{
	twocharset_lock lock{impl, cs->impl};

	return charset::create(ref<implObj>
			       ::create(impl->c,
					FcCharSetUnion(*lock.first,
						       *lock.second),
					true));
}

charset charsetObj::subtract(const const_charset &cs) const
{
	twocharset_lock lock{impl, cs->impl};

	return charset::create(ref<implObj>
			       ::create(impl->c,
					FcCharSetSubtract(*lock.first,
							  *lock.second),
					true));
}

size_t charsetObj::subtract_count(const const_charset &cs) const
{
	twocharset_lock lock{impl, cs->impl};

	return FcCharSetSubtractCount(*lock.first, *lock.second);
}

void charsetObj::merge(const const_charset &cs) const
{
	twocharset_lock lock{impl, cs->impl};

	FcCharSetMerge(*lock.first, *lock.second, NULL);
}

void charsetObj::do_enumerate(const function<enumerate_callback_t> &callback)
	const
{
	FcChar32 map[FC_CHARSET_MAP_SIZE];
	FcChar32 cur, next;
	std::vector<char32_t> chars;

	chars.reserve(FC_CHARSET_MAP_SIZE*32);

	{
		charset_lock lock{impl->charset};
		cur=FcCharSetFirstPage(*lock, map, &next);
	}

	while (1)
	{
		chars.clear();

		for (size_t i=0; i<FC_CHARSET_MAP_SIZE; ++i)
		{
			char32_t j=cur + i*32;

			while (map[i])
			{
				if (map[i] & 1)
					chars.push_back(j);
				++j;
				map[i] >>= 1;
			}
		}

		if (!callback(chars))
			break;
		if (next == FC_CHARSET_DONE)
			break;

		charset_lock lock{impl->charset};
		cur=FcCharSetNextPage(*lock, map, &next);
	}
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
