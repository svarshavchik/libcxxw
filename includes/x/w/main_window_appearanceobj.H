/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_window_appearanceobj_h
#define x_w_main_window_appearanceobj_h

#include <x/w/main_window_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/w/font_arg.H>
#include <x/w/types.H>
#include <x/functional.H>
#include <x/obj.H>

#include <tuple>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref main_window_appearance "INSERT_LIBX_NAMESPACE::w::main_window_appearance".

struct main_window_appearance_properties {

	//! Constructor
	main_window_appearance_properties();

	//! Destructor
	~main_window_appearance_properties();

#include <x/w/main_window_appearance.inc.H>

};

//! Implementation object.

//! \see main_window_appearance

class main_window_appearanceObj: public main_window_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	main_window_appearanceObj();

	//! Destructor
	~main_window_appearanceObj();

	//! Copy constructor
	main_window_appearanceObj(const main_window_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_main_window_appearance modify(F &&f)
		const
	{
		return do_modify(make_function<void
				 (const main_window_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_main_window_appearance
	do_modify(const function<void(const main_window_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
