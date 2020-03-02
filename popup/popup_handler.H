/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_handler_h
#define popup_handler_h

#include "popup/popup.H"
#include "popup/popup_attachedto_infofwd.H"
#include "override_redirect.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/generic_window_appearancefwd.H"
#include "x/w/impl/grabbed_pointerfwd.H"

#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START

struct popup_visibility_semantics;

//! Arguments to \ref popupObj::handlerObj "popup handler"'s constructor.

struct LIBCXX_HIDDEN popup_handler_args {

	//! The new popup's semantics
	const popup_visibility_semantics &attachedto_type;

	//! What WM_CLASS instance this is
	const char *wm_class_instance;

	//! Parent top level display element
	const ref<generic_windowObj::handlerObj> &parent;

	//! Popup attachedment information
	const popup_attachedto_info &attachedto_info;

	//! Appearance
	const const_generic_window_appearance &appearance;

	//! Initial nesting level of the popup.
	size_t nesting_level;

	//! Window type for this popup.
	const char *window_type;

	//! Window type for this popup.
	const char *window_state;
};

//! Menu popup semantics

//! Close any "exclusive" popup when this popup opens, and any other menu
//! popups with a higher nesting level.

extern const popup_visibility_semantics LIBCXX_HIDDEN menu_popup_type;

//! "Exclusive" popup semantics.

//! Close all menu popups when this "exclusive" popup opens, and any other
//! exclusive popups at the same nesting level.

extern const popup_visibility_semantics LIBCXX_HIDDEN exclusive_popup_type;

//! Return value from recalculate_attached_popup_position

//! Indicates where recalculate_attached_popup_position() placed the popup in relation
//! to its attached display element.
//!
//! \see popup

enum class popup_position_affinity {
	above,		//!< Popup positioned above its display element
	below,		//!< Popup positioned below its display element
	left,		//!< Popup positioned left of its display element
	right		//!< Popup positioned right of its display element
};

//! Popup handler implementation object.

//! Subclass generic_window's handler. Implement metrics methods to
//! invoke recalculate_attached_popup_position(), then proceeds and sets the
//! top level display element's position accordingly.
//!
//! \see popup

class LIBCXX_HIDDEN popupObj::handlerObj :
	public override_redirect_elementObj<generic_windowObj::handlerObj> {

	//! Alias
	typedef override_redirect_elementObj<generic_windowObj::handlerObj>
		superclass_t;

public:
	//! Popup attachedment information

	const popup_attachedto_info attachedto_info;

	//! When popup semantics this popup belongs to.
	const popup_visibility_semantics &attachedto_type;

	//! What WM_CLASS instance this is

	const char * const wm_class_instance;

	//! Constructor
	handlerObj(const popup_handler_args &);

	//! Destructor
	~handlerObj();

	//! Override installed().
	void installed(ONLY IN_THREAD) override;

	//! Implement get_main_window()

	main_windowptr get_main_window() override;


	//! Override set_default_wm_hints

	void set_default_wm_hints(ONLY IN_THREAD,
				  xcb_icccm_wm_hints_t &hints) override;

	//! Implement recalculate_popup_position().

	//! Implements the callback from override_redirectObj by invoking
	//! recalculate_attached_popup_position().

	void recalculate_window_position(ONLY IN_THREAD,
					 rectangle &r,
					 dim_t screen_width,
					 dim_t screen_height) override;

	//! Implement recalculate_window_position()

	//! Implement based on the location of the attached-to element, and
	//! also indicate how the window was positioned with respect to the
	//! element.

	virtual popup_position_affinity
		recalculate_attached_popup_position(ONLY IN_THREAD,
						    rectangle &r,
						    dim_t screen_width,
						    dim_t screen_height);

 public:

	//! Override button_press_event

	//! Pressing a button outside of the popup hides it.
	void do_button_event(ONLY IN_THREAD,
			     const xcb_button_release_event_t *event,
			     const button_event &be,
			     const motion_event &me) override;

	//! Perform any needed grab/pointer processing due to visibility change.
	void set_inherited_visibility_mapped(ONLY IN_THREAD) override;

	//! Perform any needed grab/pointer processing due to visibility change.
	void set_inherited_visibility_unmapped(ONLY IN_THREAD) override;


	//! Override absolute_location_updated

	void absolute_location_updated(ONLY IN_THREAD)
		override;

 private:

	//! The pointer is grabbed while the popup is open.
	ptr<obj> current_grab;

	//! The parent window, which is used for the pointer grab.

	weakptr<ptr<generic_windowObj::handlerObj>> popup_parent;

	//! Override default_wm_class_resource().

	//! Look to the popup parent's.

	std::string default_wm_class_resource(ONLY IN_THREAD) override;

	//! Override wm_class_instance.

	const char *default_wm_class_instance() const override;

	//! Return the popup's parent main window.

	ptr<generic_windowObj::handlerObj> get_popup_parent(ONLY IN_THREAD)
		override;

	//! Mcguffin for a visible popup.

	//! This is returned by get_opened_mcguffin().

	ptr<obj> opened_mcguffin;

 public:

	//! Perform processing after a popup is opened.

	//! Grab the pointer.
	virtual void popup_opened(ONLY IN_THREAD);

	//! The popup is about to be closed.

	//! Release the pointer grab.

	virtual void closing_popup(ONLY IN_THREAD);

	//! Override keep_passive_grab().

	//! Forward to parent element, that grabbed the pointer for us.

	bool keep_passive_grab(ONLY IN_THREAD) override;

	//! Override ungrab().

	//! Forward to parent element, that grabbed the pointer for us.
	void ungrab(ONLY IN_THREAD) override;

	//! Override grab_pointer().

	//! Forwarded to parent element.
	grabbed_pointerptr grab_pointer(ONLY IN_THREAD,
					const element_implptr &) override;

	//! Whether this popup accepts key events.

	//! The default implementation returns requested_visibility.
	//!
	//! When this popup is visible it accepts key events forwarded from
	//! the main window.
	virtual bool popup_accepts_key_events(ONLY IN_THREAD);

	//! Whether this popup accepts button events.

	//! The default implementation returns requested_visibility.

	//! When this popup is visible it accepts button and motion events
	//! forwarded from the main window.

	virtual bool popup_accepts_button_events(ONLY IN_THREAD);

 protected:
	//! Override process_key_event()

	//! If the ESC key is pressed, close the combo-box popup.

	bool process_key_event(ONLY IN_THREAD, const key_event &ke)
		override;

 public:
	//! Report the updated position of the element we are attached to.

	void update_attachedto_element_position(ONLY IN_THREAD,
						const rectangle &new_position);
};

LIBCXXW_NAMESPACE_END

#endif
