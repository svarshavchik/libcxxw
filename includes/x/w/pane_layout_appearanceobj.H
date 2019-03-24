/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_layout_appearanceobj_h
#define x_w_pane_layout_appearanceobj_h

#include <x/w/pane_layout_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref pane_layout_appearance "INSERT_LIBX_NAMESPACE::w::pane_layout_appearance".

struct pane_layout_appearance_properties {

	//! Constructor
	pane_layout_appearance_properties();

	//! Destructor
	~pane_layout_appearance_properties();

	//! Pane border.
	border_arg border;

	//! Border used to draw the slider.
	border_arg slider;

	//! Slider's background color
	color_arg slider_background_color;

	//! Slider's focusoff border
	border_arg slider_focusoff_border;

	//! Slider's focuson border.
	border_arg slider_focuson_border;
};

//! Implementation object.

//! \see pane_layout_appearance

class pane_layout_appearanceObj: public pane_layout_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	pane_layout_appearanceObj();

	//! Destructor
	~pane_layout_appearanceObj();

	//! Copy constructor
	pane_layout_appearanceObj(const pane_layout_appearanceObj &);

	//! Create a copy of this object.
	pane_layout_appearance clone() const;
};

LIBCXXW_NAMESPACE_END

#endif
