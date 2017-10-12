/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_showhide_elementfwd_h
#define popup_showhide_elementfwd_h

#include "element.H"
#include "x/w/hotspotfwd.H"

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
	const ref<elementObj::implObj> popup_element_impl;

	template<typename ...Args>
	popup_showhide_elementObj(const ref<elementObj::implObj>
				  &popup_element_impl,
				  Args && ...args);

	~popup_showhide_elementObj();

	void activated(IN_THREAD_ONLY,
		       const callback_trigger_t &trigger) override;
};

LIBCXXW_NAMESPACE_END

#endif
