/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_showhide_elementfwd_h
#define popup_showhide_elementfwd_h

#include "x/w/impl/element.H"
#include "x/w/hotspotfwd.H"
#include "x/w/impl/popup/popupobj.H"

LIBCXXW_NAMESPACE_START

//! A mixin that show/hides another element.

//! The template parameter is a subclass of hotspot_elementObj, that
//! the template inherits from.
//!
//! The first parameter to the constructor is an elementObj::implObj, and
//! the mixin overrides activated() and shows or hides this element.

template<typename element_impl>
class popup_showhide_elementObj : public element_impl {

public:
	//! The handler of the popup that's attached to this element.
	const ref<popupObj::handlerObj> my_popup_handler;

	template<typename ...Args>
	popup_showhide_elementObj(const ref<popupObj::handlerObj>
				  &my_popup_handler,
				  Args && ...args);

	~popup_showhide_elementObj();

	void activated(ONLY IN_THREAD,
		       const callback_trigger_t &trigger) override;
};

LIBCXXW_NAMESPACE_END

#endif
