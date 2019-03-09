/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef main_window_h
#define main_window_h

#include "x/w/main_window.H"
#include "x/w/containerfwd.H"
#include "x/w/layoutmanagerfwd.H"
#include "x/w/buttonfwd.H"
#include "x/w/text_paramfwd.H"
#include "x/w/dialogfwd.H"
#include "generic_window.H"
#include "xim_generic_window.H"

#include <functional>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

//! Parameters to main_windowObj::implObj's constructor.

struct LIBCXX_HIDDEN main_window_impl_args {
	ref<main_windowObj::handlerObj> handler;
	container menu_and_app_container;
	container menubar_container;
	container app_container;
};

//! Main application window implementation object.

class LIBCXX_HIDDEN main_windowObj::implObj
	: public generic_windowObj::implObj,
	  public xim_generic_windowObj {

 public:

	//! Constructor
	implObj(const main_window_impl_args &args);

	//! Destructor
	~implObj();

	//! The handler for the connection thread events.
	const ref<handlerObj> handler;

	//! The container installed in the main window's peephole

	//! The main window's peephole has a container with two elements,
	//! a menu bar, which is a container with the menu bar layout manager,
	//! and the app_container.

	const container menu_and_app_container;

	//! The container with the menubar.

	const container menubar_container;

	//! The container with the main window's specified layout manager.

	//! The main window is the real top level container, but the requested
	//! layout manager gets created for this container.
	//!
	//! main_window overrides get_layout_impl() and returns the
	//! app_container's layout manager, making it look like it's
	//! the layout manager for the top level main_window, but it's really
	//! the layout manager for the app_container.

	const container app_container;

	//! Install a callback to invoke upon close.

	void on_delete(const functionref<void (THREAD_CALLBACK,
					       const busy &)> &callback);

	//! Container for this main window's dialogs.

	typedef mpobj<std::unordered_map<std::string, dialog>> all_dialogs_t;

	all_dialogs_t all_dialogs;
};

//! Factory used by create_main_window_impl().

typedef ref<main_windowObj::implObj
	    > make_window_impl_factory_t(const main_window_impl_args &);

struct LIBCXX_HIDDEN create_main_window_impl_ret_t;

//! Return type from create_main_window_impl().

//! The main_window_impl object is the first member of this structure.
//! If an exception gets thrown, the main window's layout manager, which
//! is the grid layout manager, gets destroyed first releasing an internal
//! lock, before the main window implementation object gets destroyed.

struct create_main_window_impl_ret_t {
	ref<main_windowObj::implObj> main_window_impl;
	layoutmanager main_window_lm;
};

//! Helper for constructing a new main_windowObj::implObj.

//! The requested layout manager for the top level window actually gets
//! shoved into the main window's peephole that gets created. Returns
//! the real layout manager for the top level window.
//!
//! Regular main windows have no explicit border. A border gets provided
//! for splash windows.

template<typename functor>
inline create_main_window_impl_ret_t
create_main_window_impl(const ref<main_windowObj::handlerObj> &handler,
			const std::optional<border_arg> &border,
			const std::optional<color_arg>
			&peephole_background_color,
			const std::optional<color_arg>
			&scrollbars_background_color,
			const color_arg &menubar_background_color,
			const new_layoutmanager &layout_factory,
			functor &&f)
{
	return do_create_main_window_impl
		(handler, border,
		 peephole_background_color,
		 scrollbars_background_color,
		 menubar_background_color,
		 layout_factory,
		 make_function<make_window_impl_factory_t>
		 (std::forward<functor>(f)));
}

//! Type-erased create_main_window_impl().

create_main_window_impl_ret_t
do_create_main_window_impl(const ref<main_windowObj::handlerObj> &handler,
			   const std::optional<border_arg> &border,
			   const std::optional<color_arg>
			   &peephole_background_color,
			   const std::optional<color_arg>
			   &scrollbars_background_color,
			   const color_arg &menubar_background_color,
			   const new_layoutmanager &layout_factory,
			   const function<make_window_impl_factory_t> &factory)
	LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
