/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_searchobj_h
#define x_w_input_field_searchobj_h

#include "input_field/input_field_searchfwd.H"
#include "popup/popup_attachedto_handler_elementfwd.H"
#include "popup/popup_attachedto_handlerfwd.H"
#include "x/w/impl/popup/popupfwd.H"
#include "peepholed_focusable.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see input_field_search

class input_field_searchObj :
	public popup_attachedto_handler_elementObj<peepholed_container_impl_t> {

	//! Alias
	typedef popup_attachedto_handler_elementObj<peepholed_container_impl_t>
		superclass_t;

 public:
	//! Constructor
	input_field_searchObj(const popup &my_popup,
			      const popup_attachedto_handler &handler,
			      const container_impl &parent);

	//! Destructor
	~input_field_searchObj();

	//! The attached popup.
	const popup my_popup;
};

LIBCXXW_NAMESPACE_END

#endif
