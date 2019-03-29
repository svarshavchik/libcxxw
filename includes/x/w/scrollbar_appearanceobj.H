/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_appearanceobj_h
#define x_w_scrollbar_appearanceobj_h

#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Various of a \ref scrollbar_appearance "scrollbar appearance"
struct scrollbar_images {

	//! Image for the "scroll to the beginning" button
	std::string scroll_low;

	//! Head end part of the scrollbar's knob.
	std::string knob_start;

	//! Middle part of the scrollbar's knob.
	std::string knob_handle;

	//! Tail end part of the scrollbar's knob.
	std::string knob_end;

	//! Image for the "scroll to the end" button
	std::string scroll_high;
};

//! Actual properties inherited by \ref scrollbar_appearance "INSERT_LIBX_NAMESPACE::w::scrollbar_appearance".

struct scrollbar_appearance_properties {

	//! Constructor
	scrollbar_appearance_properties();

	//! Destructor
	~scrollbar_appearance_properties();

	//! Scroll-bar's background color
	color_arg background_color;

	//! No focus around the scrollbar
	border_arg focusoff_border;

	//! Focus around the scrollbar
	border_arg focuson_border;

	//! Horizontal scrollbar image
	scrollbar_images horizontal1;

	//! Horizontal scrollbar clicked image
	scrollbar_images horizontal2;

	//! Vertical scrollbar image
	scrollbar_images vertical1;

	//! Vertical scrollbar clicked image
	scrollbar_images vertical2;
};

//! Implementation object.

//! \see scrollbar_appearance

class scrollbar_appearanceObj: public scrollbar_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	scrollbar_appearanceObj();

	//! Destructor
	~scrollbar_appearanceObj();

	//! Copy constructor
	scrollbar_appearanceObj(const scrollbar_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_scrollbar_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const scrollbar_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_scrollbar_appearance
	do_modify(const function<void(const scrollbar_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
