/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_bgcolor_elementfwd_h
#define hotspot_bgcolor_elementfwd_h

#include "hotspot_bgcolor.H"
#include "hotspot_elementfwd.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/hotspotobj.H"

LIBCXXW_NAMESPACE_START

struct hotspot_cold_color;
struct hotspot_warm_color;
struct hotspot_hot_color;

template<typename superclass_t>
class LIBCXX_HIDDEN hotspot_bgcolor_elementBaseObj;

//! Common code shared by hotspot_bgcolor_elementObj and hotspot_transparent_bgcolor_elementObj

//! This template implements functionality that's common to these two
//! mixin templates. This template inherits from the template parameter
//! and hotspot_bgcolorObj::implObj, and implements all overrides except
//! for cold_color(), which are left to either hotspot_bgcolor_elementObj
//! or hotspot_transparent_bgcolor_elementObj.

template<typename superclass_t>
class hotspot_bgcolor_elementBaseObj
	: public superclass_t,
	  public hotspot_bgcolorObj::implObj {

 public:

	using superclass_t::superclass_t;

	~hotspot_bgcolor_elementBaseObj();

	void initialize(ONLY IN_THREAD) override;

	void temperature_changed(ONLY IN_THREAD,
				 const callback_trigger_t &trigger) override;

	background_color warm_color(ONLY IN_THREAD) override;

	background_color hot_color(ONLY IN_THREAD) override;

	hotspotObj::implObj &get_hotspot_impl() override;

	const hotspotObj::implObj &get_hotspot_impl() const override;
};

template<typename element_impl>
class LIBCXX_HIDDEN hotspot_bgcolor_elementObj;

//! A mixin for inheriting from a display element, and a hotspot_bgcolorObj::implObj.

//! The template parameter must be a subclass of child_elementObj. This
//! implements a hotspot mixin with its own background color.
//!
//! The first three template parameters are the cold, warm, and hot background
//! colors, the remaining parameters get forwarded to the superclass.
//!
//! \see hotspot_transparent_bgcolor_elementObj.

template<typename element_impl>
class hotspot_bgcolor_elementObj
	: public hotspot_bgcolor_elementBaseObj<background_color_elementObj
						<hotspot_elementObj
						 <element_impl>,
						 hotspot_cold_color,
						 hotspot_warm_color,
						 hotspot_hot_color
						 >> {

	//! Alias

	typedef hotspot_bgcolor_elementBaseObj<background_color_elementObj
					       <hotspot_elementObj
						<element_impl>,
						hotspot_cold_color,
						hotspot_warm_color,
						hotspot_hot_color
						>> superclass_t;

public:

	using superclass_t::superclass_t;

	~hotspot_bgcolor_elementObj();

	void initialize(ONLY IN_THREAD) override;

	std::optional<background_color> cold_color(ONLY IN_THREAD) override;
};

template<typename element_impl>
class LIBCXX_HIDDEN hotspot_transparent_bgcolor_elementObj;

//! A mixin for inheriting from a display element, and a hotspot_bgcolorObj::implObj.

//! The template parameter must be a subclass of child_elementObj. This
//! implements a hotspot mixin with its own background color.
//!
//! The first two template parameters are the warm and hot background
//! colors, the remaining parameters get forwarded to the superclass.
//!
//! This is similar to hotspot_bgcolor_elementObj, except that there is no
//! explicit background color when the hotspot is cold, and it inherits
//! the parent container's background color.
//!
//! \see hotspot_bgcolor_elementObj.

template<typename element_impl>
class hotspot_transparent_bgcolor_elementObj
	: public hotspot_bgcolor_elementBaseObj<background_color_elementObj
						<hotspot_elementObj
						 <element_impl>,
						 hotspot_warm_color,
						 hotspot_hot_color
						 >> {

	//! Alias

	typedef hotspot_bgcolor_elementBaseObj<background_color_elementObj
					       <hotspot_elementObj
						<element_impl>,
						hotspot_warm_color,
						hotspot_hot_color
						>> superclass_t;

public:

	using superclass_t::superclass_t;

	~hotspot_transparent_bgcolor_elementObj();

	std::optional<background_color> cold_color(ONLY IN_THREAD) override;
};

LIBCXXW_NAMESPACE_END

#endif
