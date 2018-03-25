/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_handler_elementfwd_h
#define popup_attachedto_handler_elementfwd_h

#include "popup/popup_attachedto_handlerfwd.H"
#include "connection_threadfwd.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

//! A mixin for a displayed element with a popup_attachedto_handler.

//! There's a popup attached to this display element.
//! The template parameter is a subclass of an element.
//! The first parameter to the constructor is a popup_attachedto_handler
//! object, the handler for the attached popup. The remaining parameters
//! get forwarded to the superclass's constructor.

template<typename element_impl>
class LIBCXX_HIDDEN popup_attachedto_handler_elementObj : public element_impl {

 public:

	//! The handler for the popup attached to this display element.

	const popup_attachedto_handler attachedto_handler;

	template<typename ...Args>
		popup_attachedto_handler_elementObj(const popup_attachedto_handler &attachedto_handler, Args && ...args);

	~popup_attachedto_handler_elementObj();

	void initialize(ONLY IN_THREAD) override;

	void process_updated_position(ONLY IN_THREAD) override;

	void absolute_location_updated(ONLY IN_THREAD) override;

 private:

	void update_attachedto_info(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
