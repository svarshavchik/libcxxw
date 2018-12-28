/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_handlerobj_h
#define popup_attachedto_handlerobj_h

#include "popup/popup_attachedto_handlerfwd.H"
#include "popup/popup_attachedto_infofwd.H"
#include "popup/popup_handler.H"
#include "x/w/connection_threadfwd.H"
#include "shared_handler_data.H"
#include "x/w/rgbfwd.H"

LIBCXXW_NAMESPACE_START

struct popup_visibility_semantics;

//! Menu popup semantics

//! Close any "exclusive" popup when this popup opens, and any other menu
//! popups with a higher nesting level.

extern const popup_visibility_semantics LIBCXX_HIDDEN menu_popup_type;

//! "Exclusive" popup semantics.

//! Close all menu popups when this "exclusive" popup opens, and any other
//! exclusive popups at the same nesting level.

extern const popup_visibility_semantics LIBCXX_HIDDEN exclusive_popup_type;

//! Implementation object

//! \see popup_attachedto_handler

class LIBCXX_HIDDEN popup_attachedto_handlerObj : public popupObj::handlerObj {

 protected:
	//! Popup attachedment information

	const popup_attachedto_info attachedto_info;

	//! When popup semantics this popup belongs to.
	const popup_visibility_semantics &attachedto_type;

	//! What WM_CLASS instance this is

	const char * const wm_class_instance;

	//! Override default_wm_class_instance()

	//! Returns wm_class_instance.
	const char *default_wm_class_instance() const override;

 public:
	//! Constructor
	popup_attachedto_handlerObj(const popup_attachedto_handler_args &);

	//! Destructor
	~popup_attachedto_handlerObj();

	//! Report the updated position of the element we are attached to.

	void update_attachedto_element_position(ONLY IN_THREAD,
						const rectangle &new_position);

	//! Implement recalculate_popup_position().

	popup_position_affinity recalculate_popup_position(ONLY IN_THREAD,
							   rectangle &r,
							   dim_t screen_width,
							   dim_t screen_height)
		override;

 private:
	//! Implement get_opened_mcguffin().
	ref<obj> get_opened_mcguffin(ONLY IN_THREAD) override;

	//! Implement released_opened_mcguffin().
	void released_opened_mcguffin(ONLY IN_THREAD) override;
};

//! Arguments to \ref popup_attachedto_handler "attached popup handler"'s constructor.

struct LIBCXX_HIDDEN popup_attachedto_handler_args {

	//! The new popup's semantics
	const popup_visibility_semantics &attachedto_type;

	//! What WM_CLASS instance this is
	const char *wm_class_instance;

	//! Parent top level display element
	const ref<generic_windowObj::handlerObj> &parent;

	//! Popup attachedment information
	const popup_attachedto_info &attachedto_info;

	//! Initial nesting level of the popup.
	size_t nesting_level;
};

LIBCXXW_NAMESPACE_END

#endif
