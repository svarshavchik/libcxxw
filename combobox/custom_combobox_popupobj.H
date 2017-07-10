/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_combobox_popupobj_h
#define x_w_custom_combobox_popupobj_h

#include "popup/popup_handler.H"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_autofocusfwd.H"

LIBCXXW_NAMESPACE_START

//! Custom combobox popup implementation object.

//! \see custom_combobox_popup

class LIBCXX_HIDDEN custom_combobox_popupObj
	: public popup_autofocusObj<popup_attachedto_handlerObj> {

	//! The superclass

	typedef popup_autofocusObj<popup_attachedto_handlerObj> superclass_t;
public:

	//! Constructor
	custom_combobox_popupObj(const ref<generic_windowObj::handlerObj>
				 &parent,
				 const popup_attachedto_info &attachedto_info,
				 size_t nesting_level);

	//! Destructor
	~custom_combobox_popupObj();
};

LIBCXXW_NAMESPACE_END

#endif
