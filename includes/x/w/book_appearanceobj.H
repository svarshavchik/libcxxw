/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_book_appearanceobj_h
#define x_w_book_appearanceobj_h

#include <x/w/book_appearancefwd.H>
#include <x/w/image_button_appearance.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref book_appearance "INSERT_LIBX_NAMESPACE::w::book_appearance".

struct book_appearance_properties {

	//! Constructor
	book_appearance_properties();

	//! Destructor
	~book_appearance_properties();

#include <x/w/book_appearance.inc.H>

};

//! Implementation object.

//! \see book_appearance

class book_appearanceObj: public book_appearance_properties,
			  public appearanceObj {

public:
	//! Constructor
	book_appearanceObj();

	//! Destructor
	~book_appearanceObj();

	//! Copy constructor
	book_appearanceObj(const book_appearanceObj &);

	//! Create a copy of this object.

	template<typename F> inline const_book_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const book_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_book_appearance do_modify(const function<void(const
							    book_appearance &)>
					&) const;
};

LIBCXXW_NAMESPACE_END

#endif
