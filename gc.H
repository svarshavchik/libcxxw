/*
** Copyright 2017 Double Precision, Inc.
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

class LIBCXX_HIDDEN gcObj::implObj : public xidObj {

 public:

	inline xcb_gcontext_t gc_id() const { return id_; }

	const ref<drawableObj::implObj> drawable;

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

		ref<implObj> impl;
		const gcObj::properties &props;

		configured_properties_t::lock lock;

	public:
		configure_gc(const ref<implObj> &implArg,
			     const gcObj::properties &propsArg);

		~configure_gc();
	};

	implObj(const ref<drawableObj::implObj> &drawable);
	~implObj();

	// Specify value change in a map, keyed by GC property, value is the
	// GC value

	void change_gc_locked(const gcObj::properties &props);

	void fill_rectangles(const xcb_rectangle_t *, size_t);

	void segments(const xcb_segment_t *, size_t);

	void points(const xcb_point_t *, size_t,
		    gcObj::polyfill fill_type);

	void draw_arcs(const xcb_arc_t *, size_t);

	void fill_arcs(const xcb_arc_t *, size_t);
};

LIBCXXW_NAMESPACE_END

#endif
