/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_handlerobj_h
#define popup_attachedto_handlerobj_h

#include "popup/popup_attachedto_handlerfwd.H"
#include "popup/popup_attachedto_infofwd.H"
#include "popup/popup_handler.H"
#include "connection_threadfwd.H"
#include "shared_handler_data.H"
#include "x/w/rgbfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object

//! \see popup_attachedto_handler

class LIBCXX_HIDDEN popup_attachedto_handlerObj : public popupObj::handlerObj {

	//! Popup attachedment information

	const popup_attachedto_info attachedto_info;

 public:
	//! Signature for an opened_popup report.

	typedef ref<obj> (shared_handler_dataObj::*opened_popup_t)
		(IN_THREAD_ONLY, const ref<popupObj::handlerObj> &);

	//! Signature for an closed_popup report.

	typedef void (shared_handler_dataObj::*closed_popup_t)
		(IN_THREAD_ONLY, const popupObj::handlerObj &);
 private:
	//! Which shared_handler_dataObj method we report our visibility to.

	const opened_popup_t opened_popup;

	//! Which shared_handler_dataObj method we report our visibility to.

	const closed_popup_t closed_popup;

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

	void update_attachedto_element_position(IN_THREAD_ONLY,
						const rectangle &new_position);

	//! Implement recalculate_popup_position().

	popup_position_affinity recalculate_popup_position(IN_THREAD_ONLY,
							   rectangle &r,
							   dim_t screen_width,
							   dim_t screen_height)
		override;

 private:
	//! Implement get_opened_mcguffin().
	ref<obj> get_opened_mcguffin(IN_THREAD_ONLY) override;

	//! Implement released_opened_mcguffin().
	void released_opened_mcguffin(IN_THREAD_ONLY) override;
};

//! Arguments to \ref popup_attachedto_handler "attached popup handler"'s constructor.

struct LIBCXX_HIDDEN popup_attachedto_handler_args {
	//! Which shared_handler_dataObj method we report our visibility to.
	popup_attachedto_handlerObj::opened_popup_t opened_popup;
	//! Which shared_handler_dataObj method we report our visibility to.
	popup_attachedto_handlerObj::closed_popup_t closed_popup;

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
