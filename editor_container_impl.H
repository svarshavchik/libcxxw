/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef editor_container_impl_H
#define editor_container_impl_H

#include "editor_containerobj.H"
#include "peephole_impl.H"
#include "nonrecursive_visibilityfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal-internal editor container implementation object.

class LIBCXX_HIDDEN editor_containerObj::implObj
	: public nonrecursive_visibilityObj<peepholeObj::implObj> {

 public:

	using nonrecursive_visibilityObj<peepholeObj::implObj>
		::nonrecursive_visibilityObj;

	//! We now have everything that's needed to set our metrics.

	//! Invoke recalculate().
	void initialize(IN_THREAD_ONLY) override;

	//! Invoke recalculate(), after the theme has been updated().
	void theme_updated(IN_THREAD_ONLY) override;

	//! Button 1 sets focus to the element.

	bool process_button_event(IN_THREAD_ONLY,
				  int button,
				  bool press,
				  xcb_timestamp_t timestamp,
				  const input_mask &mask);
 private:

	//! Compute updated metrics.
	void recalculate(IN_THREAD_ONLY);
 public:

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
