/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_borderlayoutmanager_h
#define x_w_borderlayoutmanager_h

#include <x/w/borderlayoutmanagerfwd.H>
#include <x/w/borderlayoutmanagerobj.H>
#include <x/w/frame_appearancefwd.H>
#include <x/w/new_layoutmanager.H>
#include <x/w/text_param.H>
#include <x/ref.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

/*!
Border layout manager properties.

Specifies the properties for a new
\ref borderlayoutmanager "border layout manager".
*/

class new_borderlayoutmanager : public new_layoutmanager {

public:
	//! Constructor
	new_borderlayoutmanager();

	//! Destructor
	~new_borderlayoutmanager();

	//! The new border and frame's appearance

	const_frame_appearance appearance;

	//! Optional title. Results in a visible frame.

	text_param title;

	//! If no title is set, automatically sets the element's background
	//! color to the theme default. Borders with titles are not visually
	//! "coherent" if the widget inside the border has a custom
	//! background color, because the border is interrupted by the
	//! title.
	//!
	//! Set this to true to prevent the widget from having its background
	//! set, even if there's no title.

	bool no_background;

private:
	//! Implement create()
	layout_impl create(const container_impl &) const override LIBCXX_HIDDEN;

	//! Implement create()
	container create(const container_impl &,
			 const function<void(const container &)> &)
		const override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
