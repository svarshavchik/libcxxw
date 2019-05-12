/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_file_dialog_appearanceobj_h
#define x_w_file_dialog_appearanceobj_h

#include <x/w/file_dialog_appearancefwd.H>
#include <x/w/pane_layout_appearancefwd.H>
#include <x/w/pane_appearancefwd.H>
#include <x/w/list_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref file_dialog_appearance "INSERT_LIBX_NAMESPACE::w::file_dialog_appearance".

struct file_dialog_appearance_properties {

	//! Constructor
	file_dialog_appearance_properties();

	//! Destructor
	~file_dialog_appearance_properties();

#include <x/w/file_dialog_appearance.inc.H>

};

//! Implementation object.

//! \see file_dialog_appearance

class file_dialog_appearanceObj: public file_dialog_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	file_dialog_appearanceObj();

	//! Destructor
	~file_dialog_appearanceObj();

	//! Copy constructor
	file_dialog_appearanceObj(const file_dialog_appearanceObj &);

	//! Create a copy of this object.

	template<typename F> inline const_file_dialog_appearance modify(F &&f)
		const
	{
		return do_modify(make_function<void(const
						    file_dialog_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_file_dialog_appearance
	do_modify(const function<void(const file_dialog_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
