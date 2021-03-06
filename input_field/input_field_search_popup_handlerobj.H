/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_search_popup_handlerobj_h
#define x_w_input_field_search_popup_handlerobj_h

#include "input_field/input_field_search_popup_handlerfwd.H"
#include "peepholed_toplevel_listcontainer/handlerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see input_field_search_popup_handler

class input_field_search_popup_handlerObj
	: public peepholed_toplevel_listcontainer_handlerObj {

	//! Alias
	typedef peepholed_toplevel_listcontainer_handlerObj superclass_t;

	//! Whether the popup has been activated

	//! Cursor-down in the input field enables this popup.
	bool search_popup_activated_thread_only=false;
public:

	THREAD_DATA_ONLY(search_popup_activated);

	//! For forwarding cursor-down key event that enters the popup.
	using peepholed_toplevel_listcontainer_handlerObj::handle_key_event;

	//! Constructor
	input_field_search_popup_handlerObj
		(const peepholed_toplevel_listcontainer_handler_args &args);

	//! Destructor
	~input_field_search_popup_handlerObj();

	//! Override popup_accepts_key_events().

	//! Returns search_popup_activated
	bool popup_accepts_key_events(ONLY IN_THREAD) override;

	//! Override set_inherited_visibility_unmapped()

	//! Reset search_popup_activated to false, when the popup becomes
	//! hidden.
	//!
	//! We must clear it when it becomes unmapped, rather than mapped.
	//! This is because request_or_search_abort() checks this flag
	//! to determine whether to hide this popup. When cursor-down gets
	//! pressed, the popup is activated, then the

	void set_inherited_visibility_unmapped(ONLY IN_THREAD) override;

	//! Override recalculate_attached_popup_position()

	//! Force popup width to be always equal to the main input field
	//! element's width.

	popup_position_affinity recalculate_attached_popup_position(ONLY IN_THREAD,
							   rectangle &r,
							   dim_t screen_width,
							   dim_t screen_height)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
