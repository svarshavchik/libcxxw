/*
** Copyright 2017-2019 Double Precision, Inc.
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

//! Implementation object

//! \see popup_attachedto_handler

class LIBCXX_HIDDEN popup_attachedto_handlerObj : public popupObj::handlerObj {

 public:
	//! Constructor
	popup_attachedto_handlerObj(const popup_attachedto_handler_args &);

	//! Destructor
	~popup_attachedto_handlerObj();
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
