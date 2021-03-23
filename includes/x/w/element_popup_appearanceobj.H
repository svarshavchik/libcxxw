/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_popup_appearanceobj_h
#define x_w_element_popup_appearanceobj_h

#include <x/w/element_popup_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/element_popup_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see element_popup_appearance

class element_popup_appearanceObj: public element_popup_appearance_properties,
				   public appearanceObj {

public:
	//! Constructor
	element_popup_appearanceObj();

	//! Destructor
	~element_popup_appearanceObj();

	//! Copy constructor
	element_popup_appearanceObj(const element_popup_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_element_popup_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const element_popup_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_element_popup_appearance do_modify(const function
					<void(const element_popup_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
