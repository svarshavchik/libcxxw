/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_handler_h
#define popup_attachedto_handler_h

#include "popup/popup_attachedto_infofwd.H"
#include "popup/popup_handler.H"
#include "connection_threadfwd.H"
#include "shared_handler_data.H"

LIBCXXW_NAMESPACE_START

//! A handler for a popup that's attached to an element.

//! Subclass of popupObj::handlerObj implementing recalculate_popup_position()
//! to attach the popup to another element.

class LIBCXX_HIDDEN popup_attachedto_handlerObj : public popupObj::handlerObj {

	//! Popup attachedment information

	const popup_attachedto_info attachedto_info;

	//! Signature for an opened_popup report.

	typedef ref<obj> (shared_handler_dataObj::*opened_popup_t)
		(IN_THREAD_ONLY, const ref<popupObj::handlerObj> &);

	//! Signature for an closed_popup report.

	typedef void (shared_handler_dataObj::*closed_popup_t)
		(IN_THREAD_ONLY, const popupObj::handlerObj &);

	//! Which shared_handler_dataObj method we report our visibility to.

	const opened_popup_t opened_popup;

	//! Which shared_handler_dataObj method we report our visibility to.

	const closed_popup_t closed_popup;
 public:
	//! Constructor
	popup_attachedto_handlerObj
		(opened_popup_t opened_popup,
		 closed_popup_t closed_popup,
		 const ref<generic_windowObj::handlerObj> &parent,
		 const char *background_color,
		 const popup_attachedto_info &attachedto_info,
		 size_t nesting_level);

	//! Destructor
	~popup_attachedto_handlerObj();

	//! Report the updated position of the element we are attached to.

	void update_attachedto_element_position(IN_THREAD_ONLY,
						const rectangle &new_position);

	//! Implement recalculate_popup_position().

	void recalculate_popup_position(IN_THREAD_ONLY,
					rectangle &r,
					dim_t screen_width,
					dim_t screen_height) override;

 private:
	//! Implement get_opened_mcguffin().
	ref<obj> get_opened_mcguffin(IN_THREAD_ONLY) override;

	//! Implement released_opened_mcguffin().
	void released_opened_mcguffin(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
