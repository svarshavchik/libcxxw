/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panelayoutmanager_h
#define x_w_panelayoutmanager_h

#include <x/w/panelayoutmanagerfwd.H>
#include <x/w/panelayoutmanagerobj.H>
#include <x/w/element.H>
#include <x/w/layoutmanager.H>
#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/w/grid_map_infofwd.H>

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
//! \c create_focusable_container() to create a container that uses the
//! pane layout manager.

class new_panelayoutmanager :
	public new_focusable_layoutmanager,
	public pane_style {

public:

	//! Possible pane layouts.

	enum class orientation_t {horizontal, vertical};

	//! The new pane's layout.

	orientation_t orientation;

	//! Constructor
	new_panelayoutmanager(orientation_t orientation=
			      orientation_t::vertical) LIBCXX_PUBLIC;

	//! Destructor
	~new_panelayoutmanager() LIBCXX_PUBLIC;

	//! Set horizontal pane layout.
	inline void horizontal() { orientation=orientation_t::horizontal; }

	//! Set vertical pane layout.
	inline void vertical() { orientation=orientation_t::vertical; }

	//! Create a container with the pane layout manager.

	//! \internal
	focusable_container
		create(const container_impl &) const override;

};

LIBCXXW_NAMESPACE_END

#endif
