/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_layout_appearanceobj_h
#define x_w_pane_layout_appearanceobj_h

#include <x/w/pane_layout_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/pane_layout_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pane_layout_appearance

class pane_layout_appearanceObj: public pane_layout_appearance_properties,
				 public appearanceObj {

public:
	//! Constructor
	pane_layout_appearanceObj();

	//! Destructor
	~pane_layout_appearanceObj();

	//! Copy constructor
	pane_layout_appearanceObj(const pane_layout_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_pane_layout_appearance modify(F &&f)
		const
	{
		return do_modify(make_function<void(const pane_layout_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_pane_layout_appearance
	do_modify(const function<void(const pane_layout_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
