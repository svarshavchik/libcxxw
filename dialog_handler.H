/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef dialog_handler_h
#define dialog_handler_h

#include "x/w/dialogobj.H"
#include "main_window_handler.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal handler.

class LIBCXX_HIDDEN dialogObj::handlerObj : public main_windowObj::handlerObj {

	//! If modal, and visible, the acquired busy mcguffin.

	ptr<obj> acquired_busy_mcguffin_thread_only;

 public:
	THREAD_DATA_ONLY(acquired_busy_mcguffin);

	//! Whether this is a modal dialog.

	const bool modal;

	//! Parent display element.
	const ref<generic_windowObj::handlerObj> parent_handler;

	//! Constructor

	handlerObj(IN_THREAD_ONLY,
		   const ref<generic_windowObj::handlerObj> &parent_handler,
		   const char *background_color,
		   bool modal);

	//! Override set_inherited_visibility()

	//! If modal, acquire or release the busy mcguffin, accordingly.
	//!
	//! If we need to handle_our_own_placement(), figure out what it
	//! should be, if we're made visible.

	void set_inherited_visibility(IN_THREAD_ONLY,
				      inherited_visibility_info &) override;

	//! Override compute_size_hints.

	//! If we need to handle_our_own_placement() then we set the P_POSITION
	//! hint (and set_inherited_visibility() will calculate what it is).

	xcb_size_hints_t compute_size_hints(IN_THREAD_ONLY) override;

 private:

	//! Check if the window manager support _NET_WM_FULL_PLACEMENT, or not.

	bool handle_our_own_placement();

 public:
	//! Destructor
	~handlerObj();
};

LIBCXXW_NAMESPACE_END

#endif
