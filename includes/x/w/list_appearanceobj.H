/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_appearanceobj_h
#define x_w_list_appearanceobj_h

#include <x/w/list_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/w/dim_arg.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! All properties inherited by a \ref list_appearance "list appearance object".

struct list_appearance_properties {

	//! Constructor
	list_appearance_properties();

	//! Destructor
	~list_appearance_properties();

#include <x/w/list_appearance.inc.H>

};

//! Implementation object.

//! \see list_appearance

class list_appearanceObj: public list_appearance_properties,
			  virtual public obj {

public:
	//! Constructor
	list_appearanceObj();

	//! Destructor
	~list_appearanceObj();

	//! Copy constructor
	list_appearanceObj(const list_appearanceObj &);

	//! Clone this object.

	template<typename F> inline const_list_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const list_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_list_appearance
	do_modify(const function<void(const list_appearance &)> &) const;

	//! Set the focusoff_border to a visible border.

	//! This gets called automatically if this is a superclass of
	//! \ref new_tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager"
	//! because it typically has visible borders. If a plain list has
	//! borders, this should be invoked to install a lesser intensity
	//~ border that's
	//! visible, but does not indicate input focus, and provides a visual
	//! connection with column_borders.

	void visible_focusoff_border();
};

LIBCXXW_NAMESPACE_END

#endif
