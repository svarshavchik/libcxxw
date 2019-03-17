/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpagefactoryobj_h
#define x_w_bookpagefactoryobj_h

#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/bookpagefactoryfwd.H>
#include <x/w/factoryfwd.H>
#include <x/w/alignment.H>
#include <x/w/shortcutfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/alignment.H>
#include <x/w/rgb.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see bookpagefactory

class bookpagefactoryObj : virtual public obj {

public:

	//! Constructor
	bookpagefactoryObj() LIBCXX_HIDDEN;

	//! Destructor
	~bookpagefactoryObj() LIBCXX_HIDDEN;

	//! Inactive tab color
	color_arg noncurrent_color;

	//! Active tab color
	color_arg current_color;

	//! Color of the tab when it has keyboard or pointer focus
	color_arg warm_color;

	//! Color of the tab when it's been activated.
	color_arg active_color;

	//! Default font for the tab
	font_arg label_font;

	//! Default font's color
	color_arg label_foreground_color;

	//! Tab padding.
	dim_arg horiz_padding;

	//! Padding for an individual tab.
	dim_arg vert_padding;

	//! Add another page to the book.

	template<typename functor>
	inline auto &add(functor &&f)
	{
		do_add(make_function<void (const factory &, const factory &)>
		       (std::forward<functor>(f)));

		return *this;
	}

	//! Add another page to the book.

	template<typename functor>
	inline auto &add(const text_param &label,
			 functor &&f,
			 halign h=halign::center)
	{
		do_add(label, make_function<void (const factory &)>
		       (std::forward<functor>(f)), h);

		return *this;
	}

	//! Add another page to the book, with a shortcut.

	template<typename functor>
	inline auto &add(functor &&f, const shortcut &s)
	{
		do_add(make_function<void (const factory &, const factory &)>
		       (std::forward<functor>(f)), s);

		return *this;
	}

	//! Add another page to the book, with a shortcut.

	template<typename functor>
	inline auto &add(const text_param &label,
			 functor &&f, const shortcut &s,
			 halign h=halign::center)
	{
		do_add(label,
		       make_function<void (const factory &)>
		       (std::forward<functor>(f)), s, h);

		return *this;
	}

	//! Type-erased add().

	void do_add(const text_param &,
		    const function<void (const factory &)> &f,
		    halign);

	//! Type-erased add().

	void do_add(const text_param &,
		    const function<void (const factory &)> &f,
		    const shortcut &,
		    halign);

	//! Set next page's alignment

	virtual bookpagefactoryObj &halign(LIBCXXW_NAMESPACE::halign)=0;

	//! Set next page's alignment

	virtual bookpagefactoryObj &valign(LIBCXXW_NAMESPACE::valign)=0;

	//! Type-erased add().

	void do_add(const function<void (const factory &,
					 const factory &)> &f);
	//! Type-erased add().

	virtual void do_add(const function<void (const factory &,
						 const factory &)> &f,
			    const shortcut &sc)=0;
};

LIBCXXW_NAMESPACE_END

#endif
