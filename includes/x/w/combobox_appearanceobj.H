/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_combobox_appearanceobj_h
#define x_w_combobox_appearanceobj_h

#include <x/w/combobox_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/popup_list_appearanceobj.H>
#include <x/w/border_arg.H>
#include <x/w/combobox_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see combobox_appearance

class combobox_appearanceObj: public popup_list_appearanceObj,
			      public combobox_appearance_properties {

public:
	//! Constructor
	combobox_appearanceObj();

	//! Destructor
	~combobox_appearanceObj();

	//! Copy constructor
	combobox_appearanceObj(const combobox_appearanceObj &);

	//! Clone this object
	template<typename F> inline const_combobox_appearance modify(F &&f)
		const
	{
		return do_modify(make_function<void(const combobox_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_combobox_appearance do_modify(const function
					    <void(const combobox_appearance &)>
					    &) const;
};

LIBCXXW_NAMESPACE_END

#endif
