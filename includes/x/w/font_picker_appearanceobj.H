/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_picker_appearanceobj_h
#define x_w_font_picker_appearanceobj_h

#include <x/w/font_picker_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/combobox_appearancefwd.H>
#include <x/w/element_popup_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/w/text_param.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref font_picker_appearance "INSERT_LIBX_NAMESPACE::w::font_picker_appearance".

struct font_picker_appearance_properties {

	//! Constructor
	font_picker_appearance_properties();

	//! Destructor
	~font_picker_appearance_properties();

#include <x/w/font_picker_appearance.inc.H>

};

//! Implementation object.

//! \see font_picker_appearance

class font_picker_appearanceObj: public font_picker_appearance_properties,
				 public appearanceObj {

public:
	//! Constructor
	font_picker_appearanceObj();

	//! Destructor
	~font_picker_appearanceObj();

	//! Copy constructor
	font_picker_appearanceObj(const font_picker_appearanceObj &);

	//! Create a copy of this object.

	template<typename F> inline const_font_picker_appearance modify(F &&f)
		const
	{
		return do_modify(make_function<
				 void(const font_picker_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_font_picker_appearance
	do_modify(const function<void(const font_picker_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
