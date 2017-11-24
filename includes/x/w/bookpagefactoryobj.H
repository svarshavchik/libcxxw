/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpagefactoryobj_h
#define x_w_bookpagefactoryobj_h

#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/bookpagefactoryfwd.H>
#include <x/w/factoryfwd.H>
#include <x/w/alignment.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see bookpagefactory

class bookpagefactoryObj : virtual public obj {

public:

	//! Constructor
	bookpagefactoryObj() LIBCXX_HIDDEN;

	//! Destructor
	~bookpagefactoryObj() LIBCXX_HIDDEN;

	//! Add another page to the book.

	template<typename functor>
	inline auto &add(functor &&f)
	{
		do_add(make_function<void (const factory &, const factory &)>
		       (std::forward<functor>(f)));

		return *this;
	}

	//! Set next page's alignment

	virtual bookpagefactoryObj &halign(LIBCXXW_NAMESPACE::halign)=0;

	//! Set next page's alignment

	virtual bookpagefactoryObj &valign(LIBCXXW_NAMESPACE::valign)=0;

	//! Type-erased add().

	virtual void do_add(const function<void (const factory &,
						 const factory &)> &f)=0;
};

LIBCXXW_NAMESPACE_END

#endif
