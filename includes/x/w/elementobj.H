/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementobj_h
#define x_w_elementobj_h

#include <x/w/elementfwd.H>
#include <x/w/containerfwd.H>
#include <x/w/main_windowfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/focusablefwd.H>
#include <x/w/tooltipfwd.H>
#include <x/w/element_statefwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/picturefwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/busyfwd.H>
#include <x/w/labelfwd.H>
#include <x/w/types.H>
#include <x/w/focus.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/shortcutfwd.H>
#include <x/obj.H>
#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <string_view>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Methods common to all display elements.

//! \see element

class elementObj : virtual public obj {

public:
	class LIBCXX_PUBLIC implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Application data.

	//! The library doesn't use this opaque pointer in any way.

	ptr<obj> appdata;

	//! Constructor
	elementObj(const ref<implObj> &impl);

	//! Destructor
	~elementObj();

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

	void set_background_color(const color_arg &name);

	//! Remove this display element's custom background color.

	void remove_background_color();

	//! Install a state update callback.

	void on_state_update(const functionref<element_state_callback_t> &);

	//! Install a metrics update callback.
	void on_metrics_update(const functionref<metrics_update_callback_t> &);

	//! Return the mcguffin for this display element's connection.

	//! \see connection
	ref<obj> connection_mcguffin() const;

	//! Block all keyboard and pointer button processing, till further notice.

	ref<obj> get_shade_busy_mcguffin() const;

	//! Block all keyboard and pointer button processing, till further notice.

	ref<obj> get_wait_busy_mcguffin() const;

	//! This display element is a label for another focusable element.

	void label_for(const focusable &f);

	//! Install a pointer focus callback.
	void on_pointer_focus(const functionref<focus_callback_t> &);

	//! Install a button event callback

	void on_button_event(const functionref<button_event_callback_t> &);

	//! Create a standard tooltip consisting of an unwrapped label.

	void create_tooltip(const text_param &text);

	//! Create a standard tooltip consisting of a word-wrapped text.

	//! The specified width, in millimeters results in word-wrapped text.
	void create_tooltip(const text_param &text,
			    const label_config &config);

	//! Create a custom tooltip.

	//! Installs a factory for the tooltip.
	void create_custom_tooltip
	(const functionref<void (THREAD_CALLBACK, const tooltip_factory &)>
	 &tooltip_factory) const;

	//! Remove the tooltip from this display element.
	void remove_tooltip() const;

	//! Create a popup context menu for this display element.

	template<typename functor>
	inline container create_popup_menu(functor &&f) const
	{
		return do_create_popup_menu(make_function
					    <void (const listlayoutmanager &)>
					    (std::forward<functor>(f)));
	}

	//! Forwarded to stop_message() in my main_window.

	virtual void stop_message(const text_param &);

	//! Forwarded to exception_message() in my main_window.

	virtual void exception_message(const exception &);

	//! Type-erased create_popup_menu().
	container do_create_popup_menu(const function
				       <void (const listlayoutmanager&)> &)
		const;

	//! Install a context popup callback.

	void install_contextpopup_callback(const functionref
					   <install_contextpopup_callback_t> &);

	//! Install a context popup callback.
	void install_contextpopup_callback(const functionref
					   <install_contextpopup_callback_t> &,
					   const shortcut &);

	//! Remove a previously installed context menu.
	void remove_contextpopup_callback() const;

	//! Return the implementation object whose size should be overridden.

	//! \internal

	virtual ref<implObj> get_minimum_override_element_impl();

	//! Return the top level main window object.

	//! May return a null ptr in a rare race condition when the main window
	//! is being simultaneously destroyed by another execution thread.
	//! This display element will also be destroyed, its days are numbered,
	//! might as well give up the ghost.

	main_windowptr get_main_window() const;

	//! Invoke the callback in the execution thread.

	void in_thread(const functionref<void (THREAD_CALLBACK)> &cb);

	//! Invoke the callback in the execution thread, after all other callbacks.

	void in_thread_idle(const functionref<void (THREAD_CALLBACK)> &cb);

	//! Whether a selection is available.

	//! This gets forwarded to this window's
	//! \ref connection "connection"'s selection_has_owner(), for
	//! convenience.
	bool selection_has_owner(const std::string_view &selection="SECONDARY")
		const;
};

//! Alias
typedef ref<elementObj::implObj> element_impl;

//! Alias
typedef ptr<elementObj::implObj> element_implptr;

LIBCXXW_NAMESPACE_END

#endif
