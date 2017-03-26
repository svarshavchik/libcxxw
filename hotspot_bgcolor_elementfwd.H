/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_bgcolor_elementfwd_h
#define hotspot_bgcolor_elementfwd_h

#include "hotspot_bgcolor.H"
#include "hotspot_elementfwd.H"
#include "background_colorfwd.H"
#include "x/w/hotspotobj.H"

LIBCXXW_NAMESPACE_START

//! A mixin for inheriting from a display element, and a hotspot_bgcolorObj::implObj.

//! The template parameter must be a subclass of child_elementObj. This
//! multiply-inherits from the template parameter, and
//! hotspot_bgcolorObj::implObj.

template<typename element_impl=child_elementObj>
class LIBCXX_HIDDEN hotspot_bgcolor_elementObj
	: public hotspot_elementObj<element_impl>,
	  public hotspot_bgcolorObj::implObj {

 public:

	template<typename ...Args>
		hotspot_bgcolor_elementObj(const background_color &cold_color,
					   const background_color &warm_color,
					   const background_color &hot_color,
					   Args && ...args);

	~hotspot_bgcolor_elementObj();

	void temperature_changed(IN_THREAD_ONLY) override;

	hotspotObj::implObj &get_hotspot_impl() override;

	const hotspotObj::implObj &get_hotspot_impl() const override;
};


LIBCXXW_NAMESPACE_END

#endif
