/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_table_appearanceobj_h
#define x_w_table_appearanceobj_h

#include <x/w/table_appearancefwd.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref table_appearance "INSERT_LIBX_NAMESPACE::w::table_appearance".

struct table_appearance_properties {

	//! Constructor
	table_appearance_properties();

	//! Destructor
	~table_appearance_properties();

#include <x/w/table_appearance.inc.H>

};

//! Implementation object.

//! \see table_appearance

class table_appearanceObj: public table_appearance_properties,
			   public appearanceObj {

public:
	//! Constructor
	table_appearanceObj();

	//! Destructor
	~table_appearanceObj();

	//! Copy constructor
	table_appearanceObj(const table_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_table_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const table_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_table_appearance do_modify(const function
					<void(const table_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
