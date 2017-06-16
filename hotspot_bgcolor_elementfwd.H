/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_bgcolor_elementfwd_h
#define hotspot_bgcolor_elementfwd_h

#include "hotspot_bgcolor.H"
#include "hotspot_elementfwd.H"
#include "background_color_elementfwd.H"
#include "x/w/hotspotobj.H"

LIBCXXW_NAMESPACE_START

struct hotspot_cold_color;
struct hotspot_warm_color;
struct hotspot_hot_color;

//! A mixin for inheriting from a display element, and a hotspot_bgcolorObj::implObj.

//! The template parameter must be a subclass of child_elementObj. This
//! multiply-inherits from the template parameter, and
//! hotspot_bgcolorObj::implObj.
//!
//! The first three template parameters are the cold, warm, and hot background
//! colors, the remaining parameters get forwarded to the superclass.

template<typename element_impl=child_elementObj>
class LIBCXX_HIDDEN hotspot_bgcolor_elementObj
	: public background_color_elementObj<hotspot_elementObj<element_impl>,
					     hotspot_cold_color,
					     hotspot_warm_color,
					     hotspot_hot_color>,
	  public hotspot_bgcolorObj::implObj {

	//! Convenience typedef.

	typedef background_color_elementObj<hotspot_elementObj<element_impl>,
					    hotspot_cold_color,
					    hotspot_warm_color,
					    hotspot_hot_color
					    > superclass_t;
 public:

	using superclass_t::superclass_t;

	~hotspot_bgcolor_elementObj();

	void initialize(IN_THREAD_ONLY) override;

	void temperature_changed(IN_THREAD_ONLY) override;

	background_color cold_color(IN_THREAD_ONLY) override;

	background_color warm_color(IN_THREAD_ONLY) override;

	background_color hot_color(IN_THREAD_ONLY) override;

	hotspotObj::implObj &get_hotspot_impl() override;

	const hotspotObj::implObj &get_hotspot_impl() const override;
};


LIBCXXW_NAMESPACE_END

#endif
