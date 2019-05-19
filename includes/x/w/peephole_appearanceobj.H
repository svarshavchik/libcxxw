/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peephole_appearanceobj_h
#define x_w_peephole_appearanceobj_h

#include <x/w/peephole_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/dim_arg.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref peephole_appearance "INSERT_LIBX_NAMESPACE::w::peephole_appearance".

struct peephole_appearance_properties {

	//! Constructor
	peephole_appearance_properties();

	//! Destructor
	~peephole_appearance_properties();

#include <x/w/peephole_appearance.inc.H>

};

//! Implementation object.

//! \see peephole_appearance

class peephole_appearanceObj: public peephole_appearance_properties,
			      public appearanceObj {

public:
	//! Constructor
	peephole_appearanceObj();

	//! Destructor
	~peephole_appearanceObj();

	//! Copy constructor
	peephole_appearanceObj(const peephole_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_peephole_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const peephole_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_peephole_appearance
	do_modify(const function<void(const peephole_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
