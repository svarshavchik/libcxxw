/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef dialog_handler_h
#define dialog_handler_h

#include "x/w/dialogobj.H"
#include "x/w/rgbfwd.H"
#include "main_window_handler.H"
#include "x/w/connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal handler.

class LIBCXX_HIDDEN dialogObj::handlerObj : public main_windowObj::handlerObj {

	//! Alias

	typedef main_windowObj::handlerObj superclass_t;

	//! If modal, and visible, the acquired busy mcguffin.

	ptr<obj> acquired_busy_mcguffin_thread_only;

	//! Override default_wm_class_instance()

	//! This is a "dialog".
	const char *default_wm_class_instance() const override;

 public:
	THREAD_DATA_ONLY(acquired_busy_mcguffin);

	//! Whether this is a modal dialog.

	const bool modal;

	//! Parent display element.
	const ref<generic_windowObj::handlerObj> parent_handler;

	//! Constructor

	handlerObj(ONLY IN_THREAD,
		   const ref<generic_windowObj::handlerObj> &parent_handler,
		   const color_arg &background_color,
		   bool modal);

	//! Override set_inherited_visibility()

	//! If modal, acquire or release the busy mcguffin, accordingly.

	void set_inherited_visibility(ONLY IN_THREAD,
				      inherited_visibility_info &) override;

	//! Override set_inherited_visibility_mapped)(.

	//! If we need to handle_our_own_placement(), figure out what it
	//! should be, if we're made visible.
	//!
	//! Also call our parent's shared handler data's opening_dialog().

	void set_inherited_visibility_mapped(ONLY IN_THREAD) override;

	//! Override set_inherited_visibility_unmapped().

	//! Calls our parent's shared handler data's closing_dialog().

	void set_inherited_visibility_unmapped(ONLY IN_THREAD) override;

	//! Override compute_size_hints.

	//! If we need to handle_our_own_placement() then we set the P_POSITION
	//! hint (and set_inherited_visibility() will calculate what it is).

	xcb_size_hints_t compute_size_hints(ONLY IN_THREAD) override;

	//! Override default_wm_class_resource().

	//! Use the same one as the parent, by default.

	std::string default_wm_class_resource(ONLY IN_THREAD) override;

 private:

	//! Check if the window manager support _NET_WM_FULL_PLACEMENT, or not.

	bool handle_our_own_placement();

 public:
	//! Destructor
	~handlerObj();
};

LIBCXXW_NAMESPACE_END

#endif
