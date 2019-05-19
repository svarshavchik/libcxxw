/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_appearanceobj_h
#define x_w_pane_appearanceobj_h

#include <x/w/pane_appearancefwd.H>
#include <x/w/scrollbarfwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/w/alignment.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref pane_appearance "INSERT_LIBX_NAMESPACE::w::pane_appearance".

struct pane_appearance_properties {

	//! Constructor
	pane_appearance_properties();

	//! Destructor
	~pane_appearance_properties();

#include <x/w/pane_appearance.inc.H>

};

//! Implementation object.

//! \see pane_appearance

class pane_appearanceObj: public pane_appearance_properties,
			  public appearanceObj {

public:
	//! Constructor
	pane_appearanceObj();

	//! Destructor
	~pane_appearanceObj();

	//! Copy constructor
	pane_appearanceObj(const pane_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_pane_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const pane_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_pane_appearance do_modify(const function
					<void(const pane_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
