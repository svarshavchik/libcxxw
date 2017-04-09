/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef editor_container_impl_impl_H
#define editor_container_impl_impl_H

#include "editor_containerobj.H"
#include "peephole_impl.H"

LIBCXXW_NAMESPACE_START

//! Internal-internal editor container implementation object.

class LIBCXX_HIDDEN editor_containerObj::implObj : public peepholeObj::implObj {

 public:

	using peepholeObj::implObj::implObj;

	//! We now have everything that's needed to set our metrics.

	//! Invoke recalculate().
	void initialize(IN_THREAD_ONLY) override;

	//! Invoke recalculate(), after the theme has been updated().
	void theme_updated(IN_THREAD_ONLY) override;

 private:

	//! Compute updated metrics.
	void recalculate(IN_THREAD_ONLY);
 public:
	//! Override request_visibility_recursive().

	//! The editor element in this container is always visible, and
	//! request_visibility_recursive punts to request_visibility().

	void request_visibility_recursive(IN_THREAD_ONLY, bool flag)
		override;

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
