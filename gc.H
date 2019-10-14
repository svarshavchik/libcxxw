/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef gc_H
#define gc_H

#include <x/ref.H>
#include <x/mpobj.H>
#include "x/w/screenfwd.H"
#include "x/w/gc.H"
#include "x/w/drawableobj.H"
#include "xid_t.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! The GC handler object

//! Implement the underlying GC protocol functions.
//!
//! The subclass implements get_drawable_impl() and returns the GC's drawable.
//!
//! The constructor gets a reference to the drawable that's not stashed anywhere
//! and is used only by the constructor.

class gcObj::handlerObj : public xidObj {

public:

	inline xcb_gcontext_t gc_id() const { return id_; }

	connection_thread thread() const;
	xcb_connection_t *conn() const;

	// Most recently-installed GC properties.

	// We cache them to avoid making unnecessary requests.

	struct configured_t {
		values_and_mask configured_values;
		std::vector<uint8_t> configured_dashes;
		dim_t configured_dashes_offset=0;
	};

	typedef mpobj<configured_t> configured_properties_t;

	configured_properties_t configured_properties;

	// Acquire a lock on mutex_t, call change_gc()

	class configure_gc {

		ref<handlerObj> impl;
		const gcObj::properties &props;

		configured_properties_t::lock lock;

	public:
		configure_gc(const ref<handlerObj> &implArg,
			     const gcObj::properties &propsArg);

		~configure_gc();
	};

	//! Constructor
	handlerObj(const drawableObj::implObj &drawable);

	//! Destructor
	~handlerObj();

	//! Return my drawable
	virtual drawableObj::implObj &get_drawable_impl()=0;

	//! Return my drawable
	virtual const drawableObj::implObj &get_drawable_impl() const=0;

	// Specify value change in a map, keyed by GC property, value is the
	// GC value

	void change_gc_locked(const gcObj::properties &props);

	void fill_rectangles(const xcb_rectangle_t *, size_t);

	void segments(const xcb_segment_t *, size_t);

	void points(const xcb_point_t *, size_t,
		    gcObj::polyfill fill_type);

	void draw_arcs(const xcb_arc_t *, size_t);

	void fill_arcs(const xcb_arc_t *, size_t);

	//! Configure properties, then invoke copy_configured().

	//! All users call this, to lock the GC and configure it.
	//! flush_redrawn_areas uses the GC that's attached to the window
	//! and is preconfigured with the default values when
	//! it is installed(); so it calls copy_configured() directly.
	void copy(const rectangle &, coord_t, coord_t,
		  const const_ref<drawableObj::implObj> &src,
		  const ref<drawableObj::implObj> &dst,
		  const gcObj::properties &props);

	//! Invoke xcb_copy_area().
	void copy_configured(const rectangle &, coord_t, coord_t,
			     const const_ref<drawableObj::implObj> &src,
			     const ref<drawableObj::implObj> &dst);

};

//! GC implementation object

//! Inherits from the handler object, that implements all functionality.
//! Owns a reference to our drawable, and implements get_drawable_impl().

class gcObj::implObj : public handlerObj {

public:

	//! Our held reference on the drawable

	const ref<drawableObj::implObj> drawable;

	//! Constructor
	implObj(const ref<drawableObj::implObj> &drawable);

	//! Destructor
	~implObj();

	//! Implement get_drawable_impl().
	drawableObj::implObj &get_drawable_impl() override;

	//! Implement get_drawable_impl().
	const drawableObj::implObj &get_drawable_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
