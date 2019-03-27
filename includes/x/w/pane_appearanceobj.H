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
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref pane_appearance "INSERT_LIBX_NAMESPACE::w::pane_appearance".

struct pane_appearance_properties {

	//! Constructor
	pane_appearance_properties();

	//! Destructor
	~pane_appearance_properties();

	//! Requested size of the next pane

	dim_arg size;

	//! Requested background color for the next pane.

	std::optional<color_arg> background_color;

	//! Padding
	dim_arg left_padding;

	//! Padding
	dim_arg right_padding;

	//! Padding
	dim_arg top_padding;

	//! Padding
	dim_arg bottom_padding;

	//! Custom alignment
	halign horizontal_alignment;

	//! Custom alignment
	valign vertical_alignment;

	//! Scrollbar visibility
	scrollbar_visibility pane_scrollbar_visibility;

	//! Scrollbar appearance
	const_scrollbar_appearance horizontal_scrollbar;

	//! Scrollbar appearance
	const_scrollbar_appearance vertical_scrollbar;
};

//! Implementation object.

//! \see pane_appearance

class pane_appearanceObj: public pane_appearance_properties,
				 virtual public obj {

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
