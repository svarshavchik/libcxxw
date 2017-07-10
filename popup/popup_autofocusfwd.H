/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_autofocusfwd_h
#define popup_autofocusfwd_h

#include "x/w/namespace.H"
#include "elementfwd.H"
#include "connection_threadfwd.H"
#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START

//! Automatically request input focus when the popup is visible.

//! The template parameter is popupObj::handlerObj or its subclass.
//!
//! This mixin automatically request input focus to the popup window when it
//! becomes visible.
//!
//! If parent_element_window is set, the input focus is returned to that
//! element's window when this element is hidden.

template<typename popup_handler>
class popup_autofocusObj : public popup_handler {

	weakptr<elementimplptr> parent_element_window_thread_only;

public:
	THREAD_DATA_ONLY(parent_element_window);

	using popup_handler::popup_handler;

	//! Default destructor
	~popup_autofocusObj();

	//! Call set_input_focus() if the popup window is now visible.

	void inherited_visibility_updated(IN_THREAD_ONLY,
					  inherited_visibility_info &info)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
