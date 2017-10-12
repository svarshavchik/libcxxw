/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementobj_h
#define x_w_elementobj_h

#include <x/w/elementfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/focusablefwd.H>
#include <x/w/tooltipfwd.H>
#include <x/w/element_statefwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/picturefwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/busyfwd.H>
#include <x/w/types.H>
#include <x/w/focus.H>
#include <x/obj.H>

#include <string_view>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Methods common to all display elements.

//! \see element

class elementObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Application data.

	//! The library doesn't use this opaque pointer in any way.

	ptr<obj> appdata;

	//! Constructor
	elementObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~elementObj() LIBCXX_HIDDEN;

	//! Return my screen

	screen get_screen();

	//! Return my screen

	const_screen get_screen() const;

	//! Show this display element, and all elements inside it.

	void show_all();

	//! Hide this display element, and all elements inside it.

	void hide_all();

	//! Show this display element only.

	void show();

	//! Hide this display element only.

	void hide();

	//! Use a standard background color for this display element.

	void set_background_color(const std::string_view &name);

	//! Use a custom background color for this display element.

	void set_background_color(const const_picture &background_color);

	//! Create a solid color fill picture object.

	//! To be used as a background. Forwarded to the screen object.

	const_picture create_solid_color_picture(const rgb &color) const;

	//! Remove this display element's custom background color.

	void remove_background_color();

	//! Install a state update callback.
	void on_state_update(const std::function<element_state_callback_t> &);

	//! Return the mcguffin for this display element's connection.

	//! \see connection
	ref<obj> connection_mcguffin() const;

	//! Block all keyboard and pointer button processing, till further notice.

	ref<obj> get_busy_mcguffin() const;

	//! This display element is a label for another focusable element.

	void label_for(const focusable &f);

	//! Install a pointer focus callback.
	void on_pointer_focus(const std::function<focus_callback_t> &callback);

	//! Create a standard tooltip consisting of an unwrapped label.

	void create_tooltip(const text_param &text);

	//! Create a standard tooltip consisting of a word-wrapped text.

	//! The specified width, in millimeters results in word-wrapped text.
	void create_tooltip(const text_param &text,
			    double widthmm);

	//! Create a custom tooltip.

	//! Installs a factory for the tooltip.
	void create_custom_tooltip
	(const std::function<void (const tooltip_factory &)>
	 &tooltip_factory) const;


	//! Remove the tooltip from this display element.
	void remove_tooltip() const;

	//! Return the implementation object whose size should be overridden.

	//! \internal

	virtual ref<implObj> get_minimum_override_element_impl();
};

LIBCXXW_NAMESPACE_END

#endif
