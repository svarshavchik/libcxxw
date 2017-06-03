/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_main_window_impl_h
#define peepholed_toplevel_main_window_impl_h

#include "peepholed_toplevel_main_windowobj.H"
#include "peephole/peepholed_toplevel_elementfwd.H"
#include "fonts/current_fontcollectionfwd.H"
#include "container_elementfwd.H"
#include "child_element.H"
#include "always_visiblefwd.H"

LIBCXXW_NAMESPACE_START

//! The implementation object of the top level peephole container.

//! The metrics get calculated in the implementation object, when it is
//! either initialize()d, or when explicitly invoked from the "public"
//! object's recalculate_metrics().
//!
//! \see peepholed_toplevel_main_window

class LIBCXX_HIDDEN peepholed_toplevel_main_windowObj::implObj
	: public always_visibleObj<container_elementObj<child_elementObj>> {

	//! Parent class.

	typedef always_visibleObj<container_elementObj<child_elementObj>
				  > superclass_t;

	//! The calculated metrics.
	struct data_t {
		dim_t max_width;
		dim_t max_height;
		dim_t horizontal_increment;
		dim_t vertical_increment;
	} data_thread_only;

	//! Reference font used to compute the increments.

	const current_fontcollection reference_font;

 public:
	THREAD_DATA_ONLY(data);

	implObj(const ref<containerObj::implObj> &parent_container);
	~implObj();

	//! Inherited from elementObj::implObj

	void initialize(IN_THREAD_ONLY) override;

	//! Recalculate the data

	//! Invoked by initialize() and from the pseudo-public object.
	void recalculate_metrics(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
