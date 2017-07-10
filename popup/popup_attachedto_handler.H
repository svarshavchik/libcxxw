/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_handler_h
#define popup_attachedto_handler_h

#include "popup/popup_attachedto_infofwd.H"
#include "popup/popup_handler.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! A handler for a popup that's attached to an element.

//! Subclass of popupObj::handlerObj implementing recalculate_popup_position()
//! to attach the popup to another element.

class LIBCXX_HIDDEN popup_attachedto_handlerObj : public popupObj::handlerObj {

	//! Popup attachedment information

	const popup_attachedto_info attachedto_info;

 public:
	//! Constructor
	popup_attachedto_handlerObj
		(const ref<generic_windowObj::handlerObj> &parent,
		 const popup_attachedto_info &attachedto_info,
		 size_t nesting_level=0);

	//! Destructor
	~popup_attachedto_handlerObj();

	void update_attachedto_element_position(IN_THREAD_ONLY,
						const rectangle &new_position);

	void recalculate_popup_position(IN_THREAD_ONLY,
					rectangle &r,
					dim_t screen_width,
					dim_t screen_height) override;
};

LIBCXXW_NAMESPACE_END

#endif
