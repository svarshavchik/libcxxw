/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_combobox_appearanceobj_h
#define x_w_combobox_appearanceobj_h

#include <x/w/combobox_appearancefwd.H>
#include <x/w/popup_list_appearanceobj.H>
#include <x/w/border_arg.H>

LIBCXXW_NAMESPACE_START

//! All properties inherited by a \ref combobox_appearance "popup list appearance object".

//! That's in addition to the ones that are inherited from the
//! \ref popup_list_appearance "popup_list appearance properties" object.

struct combobox_appearance_properties {

	//! Constructor

	combobox_appearance_properties();

	//! Destructor

	~combobox_appearance_properties();

	//! Border around the combo-box's current element and button

	border_arg combobox_border;

	//! Focus off border around the combo-box's button
	border_arg combobox_button_focusoff_border;

	//! Focus on border around the combo-box's button
	border_arg combobox_button_focuson_border;
};

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
