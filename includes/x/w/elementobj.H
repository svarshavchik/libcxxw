/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementobj_h
#define x_w_elementobj_h

#include <x/w/elementfwd.H>
#include <x/w/containerfwd.H>
#include <x/w/main_windowfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/focusablefwd.H>
#include <x/w/element_statefwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/picturefwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/busyfwd.H>
#include <x/w/labelfwd.H>
#include <x/w/types.H>
#include <x/w/focus.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/popup_list_appearancefwd.H>
#include <x/w/shortcutfwd.H>
#include <x/w/tooltipfwd.H>
#include <x/obj.H>
#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <string_view>
#include <functional>
#include <utility>

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

	//! Show this display element, and all elements inside it.

	void show_all(ONLY IN_THREAD);

	//! Hide this display element, and all elements inside it.

	void hide_all(ONLY IN_THREAD);

	//! Show this display element only.

	void show(ONLY IN_THREAD);

	//! Hide this display element only.

	void hide(ONLY IN_THREAD);

	//! Moves this display element into view, if it's in a peephole.
	void ensure_entire_visibility();

	//! Moves this display element into view, if it's in a peephole.
	void ensure_entire_visibility(ONLY IN_THREAD);

	//! Use a standard background color for this display element.

	void set_background_color(const color_arg &name);

	//! Remove this display element's custom background color.

	void remove_background_color();

	//! Install a state update callback.
	void on_state_update(const functionref<element_state_callback_t> &);

	//! Install a state update callback.
	void on_state_update(ONLY IN_THREAD,
			     const functionref<element_state_callback_t> &);

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

	//! Install a motion event callback

	void on_motion_event(const functionref<motion_event_callback_t> &);

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

	//! Create a static tooltip.

	template<typename creator,
		 typename=decltype(std::declval<creator &&>()
				   (std::declval<const container &>()))>
	inline container create_static_tooltip(creator &&c)
	{
		return do_create_static_tooltip(make_function<void
						(const container &)>
						(std::forward<creator>(c)));
	}

	//! Create a static tooltip.

	template<typename creator,
		 typename=decltype(std::declval<creator &&>()
				   (std::declval<const container &>()))>
	inline container create_static_tooltip(creator &&c,
					       const
					       create_static_tooltip_args_t
					       &args)
	{
		return do_create_static_tooltip(make_function<void
						(const container &)>
						(std::forward<creator>(c)),
						args);
	}

	//! Type-erased create_static_tooltip().
	container do_create_static_tooltip(const function<void
					   (const container &)> &creator);

	//! Type-erased create_static_tooltip().
	container do_create_static_tooltip(const function<void
					   (const container &)> &creator,
					   const create_static_tooltip_args_t
					   &args);

	//! Create a static tooltip

	template<typename creator,
		 typename=decltype(std::declval<creator &&>()
				   (std::declval<const container &>()))>
	inline container create_static_tooltip(ONLY IN_THREAD,
					       creator &&c)
	{
		return do_create_static_tooltip(IN_THREAD,
						make_function<void
						(const container &)>
						(std::forward<creator>(c)));
	}

	//! Create a static tooltip

	template<typename creator,
		 typename=decltype(std::declval<creator &&>()
				   (std::declval<const container &>()))>
	inline container create_static_tooltip(ONLY IN_THREAD,
					       creator &&c,
					       const
					       create_static_tooltip_args_t
					       &args)
	{
		return do_create_static_tooltip(IN_THREAD,
						make_function<void
						(const container &)>
						(std::forward<creator>(c)),
						args);
	}

	//! Type-erased create_static_tooltip().

	container do_create_static_tooltip(ONLY IN_THREAD,
					   const function<void
					   (const container &)> &creator);

	//! Type-erased create_static_tooltip().

	container do_create_static_tooltip(ONLY IN_THREAD,
					   const function<void
					   (const container &)> &creator,
					   const
					   create_static_tooltip_args_t &args);

	//! Remove the tooltip from this display element.

	//! Removes a standard or a static tooltip.
	void remove_tooltip() const;

	//! Remove the tooltip from this display element.

	//! Removes a standard or a static tooltip.
	void remove_tooltip(ONLY IN_THREAD) const;

	//! Create a popup context menu for this display element.

	template<typename functor,
		 typename ...Args>
	inline container create_popup_menu(functor &&f,
					   Args && ...args) const
	{
		return do_create_popup_menu(make_function
					    <void (const listlayoutmanager &)>
					    (std::forward<functor>(f)),
					    std::forward<Args>(args)...);
	}

	//! Forwarded to stop_message() in my main_window.

	virtual void stop_message(const text_param &);

	//! Forwarded to exception_message() in my main_window.

	virtual void exception_message(const exception &);

	//! Type-erased create_popup_menu().
	container do_create_popup_menu(const function
				       <void (const listlayoutmanager&)> &)
		const;

	//! Type-erased create_popup_menu().
	container do_create_popup_menu(const function
				       <void (const listlayoutmanager&)> &,
				       const const_popup_list_appearance
				       &appearance)
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

	void in_thread(const functionref<void (THREAD_CALLBACK)> &cb) const;

	//! Invoke the callback in the execution thread, after all other callbacks.

	void in_thread_idle(const functionref<void (THREAD_CALLBACK)> &cb)
		const;

	//! Whether the current theme's default selection is available.

	//! The default selection clipboard is a configuration setting.
	bool selection_has_owner() const;

	//! Whether a selection is available.

	//! This gets forwarded to this window's
	//! \ref connection "connection"'s selection_has_owner(), for
	//! convenience.
	bool selection_has_owner(const std::string_view &selection)
		const;

	//! Whether a selection can be received right now.

	//! This gets forwarded to this element's
	//! \ref generic_window "top level window"'s
	//! selection_can_be_received(), so the element that gets to receive
	//! the selection may not necessarily be this display element.
	virtual bool selection_can_be_received() const;

	//! Receive a selection

	//! This gets forwarded to this element's
	//! \ref generic_window "top level window"'s
	//! receive_selection(), so the element that gets to receive
	//! the selection may not necessarily be this display element.
	void receive_selection(const std::string_view &selection);

	//! Receive the current theme's default selection .

	//! The default selection clipboard is a configuration setting.
	void receive_selection();

	//! Receive a selection

	//! This gets forwarded to this element's
	//! \ref generic_window "top level window"'s
	//! receive_selection(), so the element that gets to receive
	//! the selection may not necessarily be this display element.
	virtual void receive_selection(ONLY IN_THREAD,
				       const std::string_view &selection);

	//! Receive the current theme's default selection .

	//! The default selection clipboard is a configuration setting.
	void receive_selection(ONLY IN_THREAD);

	//! Cut or copy the selection

	//! This gets forwarded to this element's
	//! \ref generic_window "top level window"'s
	//! cut_or_copy_selection(), so the element whose selection gets cut
	//! or copied may not necessarily be this display element.

	virtual bool cut_or_copy_selection(cut_or_copy_op,
					   const std::string_view &selection);

	//! Cut or copy the theme's default selection

	//! The default selection clipboard is a configuration setting.
	bool cut_or_copy_selection(cut_or_copy_op);

	//! Cut or copy the selection

	//! This gets forwarded to this element's
	//! \ref generic_window "top level window"'s
	//! cut_or_copy_selection(), so the element whose selection gets cut
	//! or copied may not necessarily be this display element.

	virtual bool cut_or_copy_selection(ONLY IN_THREAD,
					   cut_or_copy_op,
					   const std::string_view &selection);
	//! Cut or copy the theme's default selection

	//! The default selection clipboard is a configuration setting.
	bool cut_or_copy_selection(ONLY IN_THREAD, cut_or_copy_op);
};

//! Alias
typedef ref<elementObj::implObj> element_impl;

//! Alias
typedef ptr<elementObj::implObj> element_implptr;

LIBCXXW_NAMESPACE_END

#endif
