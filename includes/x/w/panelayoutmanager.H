/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panelayoutmanager_h
#define x_w_panelayoutmanager_h

#include <x/w/panelayoutmanagerfwd.H>
#include <x/w/panelayoutmanagerobj.H>
#include <x/w/layoutmanager.H>
#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>

LIBCXXW_NAMESPACE_START

//! Custom pane appearance.

//! \see panelayoutmanager

struct pane_style {

	//! Pane border.
	border_arg border;

	//! Border used to draw the slider.
	border_arg slider;

	//! Slider's background color
	color_arg slider_background_color;
};

//! Create a new container with a \ref panelayoutmanager "pane layout manager".

//! This parameter gets passed to a \ref factory "factory"'s
//! \c create_container()
//! when creating a main window, or a container that
//! uses the pane layout manager.

class LIBCXX_PUBLIC new_panelayoutmanager :
	public new_focusable_layoutmanager,
	public pane_style {

public:
	new_panelayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_panelayoutmanager() LIBCXX_PUBLIC;

	//! Create a container with the pane layout manager.

	//! \internal
	focusable_container
		create(const ref<containerObj::implObj> &) const override;
};

LIBCXXW_NAMESPACE_END

#endif
