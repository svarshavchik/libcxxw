/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontlistobj_H
#define fontlistobj_H

#include <x/ref.H>
#include <x/obj.H>

#include "fonts/fontpatternfwd.H"
#include "fonts/fontlistfwd.H"

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! A list of fonts.

//! The fontconfig library creates a list of fonts, we wrap it and provide
//! iterators for them.

class listObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	listObj(const ref<implObj> &implArg);

	//! Destructor
	~listObj();

	//! Number of fonts in this list.

	size_t size() const;

	class LIBCXX_PUBLIC const_iterator;
	class LIBCXX_PUBLIC iterator;

	//! A iterator over a constant font list.

	class const_iterator : public std::iterator<std::random_access_iterator_tag, pattern> {

	protected:
		//! List reference
		ref<implObj> l;

		//! Iterator value
		size_t p;

	public:
		//! Constructor
		const_iterator(const ref<implObj> &lArg, size_t pArg);

		//! Destructor
		~const_iterator();

		//! Copy constructor
		const_iterator(const const_iterator &);

		//! Move constructor
		const_iterator(const_iterator &&);

		//! Assignment operator
		const_iterator &operator=(const const_iterator &);

		//! Assignment operator
		const_iterator &operator=(const_iterator &&);

		//! Iterator operator
		bool operator==(const const_iterator &c) const
		{
			return p == c.p;
		}

		//! Iterator operator
		bool operator!=(const const_iterator &c) const
		{
			return p != c.p;
		}

		//! Iterator operator
		bool operator<(const const_iterator &c) const
		{
			return p < c.p;
		}

		//! Iterator operator
		bool operator>(const const_iterator &c) const
		{
			return p > c.p;
		}

		//! Iterator operator
		bool operator<=(const const_iterator &c) const
		{
			return p <= c.p;
		}

		//! Iterator operator
		bool operator>=(const const_iterator &c) const
		{
			return p >= c.p;
		}

		//! Iterator operator
		pattern operator*() const;

		//! Iterator operator
		const_iterator &operator++()
		{
			++p;
			return *this;
		}

		//! Iterator operator
		const_iterator operator++(int);

		//! Iterator operator
		const_iterator &operator--()
		{
			--p;
			return *this;
		}

		//! Iterator operator
		const_iterator operator--(int);

		//! Iterator operator
		const_iterator operator+(std::ptrdiff_t o) const;

		//! Iterator operator
		const_iterator operator-(std::ptrdiff_t o) const;

		//! Iterator operator
		const_iterator &operator+=(std::ptrdiff_t o)
		{
			p += o;

			return *this;
		}
		//! Iterator operator
		const_iterator &operator-=(std::ptrdiff_t o)
		{
			p -= o;
			return *this;
		}

		//! Iterator operator
		std::ptrdiff_t operator-(const const_iterator &o) const
		{
			return p-o.p;
		}

		//! Iterator operator
		pattern operator[](std::ptrdiff_t o) const;
	};

	//! An iterator over a mutable list of fonts.

	class iterator : public const_iterator {

	public:
		//! Constructor
		iterator(const ref<implObj> &lArg, size_t pArg);

		//! Destructor
		~iterator();

		//! Copy constructor
		iterator(const iterator &);

		//! Move constructor
		iterator(iterator &&);

		//! Assignment operator
		iterator &operator=(const iterator &);

		//! Assignment operator
		iterator &operator=(iterator &&);

		using const_iterator::operator==;
		using const_iterator::operator!=;
		using const_iterator::operator<;
		using const_iterator::operator<=;
		using const_iterator::operator>;
		using const_iterator::operator>=;
		using const_iterator::operator-;
		using const_iterator::operator[];
		using const_iterator::operator*;

		//! Iterator operator
		iterator &operator++()
		{
			const_iterator::operator++();
			return *this;
		}

		//! Iterator operator
		iterator operator++(int);

		//! Iterator operator
		iterator &operator--()
		{
			const_iterator::operator--();
			return *this;
		}

		//! Iterator operator
		iterator operator--(int);

		//! Iterator operator
		iterator operator+(std::ptrdiff_t o) const;
		//! Iterator operator
		iterator operator-(std::ptrdiff_t o) const;

		//! Iterator operator
		iterator &operator+=(std::ptrdiff_t o)
		{
			p += o;
			return *this;
		}

		//! Iterator operator
		iterator &operator-=(std::ptrdiff_t o)
		{
			p -= o;
			return *this;
		}
	};

	//! Return the beginning iterator
	iterator begin() LIBCXX_PUBLIC;

	//! Return the ending iterator
	iterator end() LIBCXX_PUBLIC;

	//! Return the beginning iterator
	const_iterator begin() const;

	//! Return the ending iterator
	const_iterator end() const;
};

//! Iterator operator
inline listObj::const_iterator operator+(std::ptrdiff_t o,
					 const listObj::const_iterator &i)
{
	return i+o;
}

//! Iterator operator
inline listObj::const_iterator operator-(std::ptrdiff_t o,
					 const listObj::const_iterator &i)
{
	return i+o;
}

//! Iterator operator
inline listObj::iterator operator+(std::ptrdiff_t o,
				   const listObj::iterator &i)
{
	return i+o;
}

//! Iterator operator
inline listObj::iterator operator-(std::ptrdiff_t o,
				   const listObj::iterator &i)
{
	return i+o;
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
