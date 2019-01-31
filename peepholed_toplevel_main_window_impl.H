/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_main_window_impl_h
#define peepholed_toplevel_main_window_impl_h

#include "peepholed_toplevel_main_windowobj.H"
#include "peephole/peepholed_toplevel_elementfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/always_visiblefwd.H"
#include "x/w/impl/theme_font_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! The implementation object of the top level peephole container.

//! The metrics get calculated in the implementation object, when it is
//! either initialize()d, or when explicitly invoked from the "public"
//! object's recalculate_metrics().
//!
//! \see peepholed_toplevel_main_window

class LIBCXX_HIDDEN peepholed_toplevel_main_windowObj::implObj
	: public theme_font_elementObj<always_visibleObj
					   <container_elementObj
					    <child_elementObj>>> {

	//! Parent class.

	typedef theme_font_elementObj<always_visibleObj
					   <container_elementObj
					    <child_elementObj>>> superclass_t;

	//! The calculated metrics.
	struct data_t {
		dim_t max_width;
		dim_t max_height;
	} data_thread_only;

 public:
	THREAD_DATA_ONLY(data);

	//! Constructor
	implObj(const container_impl &parent_container);

	//! Destructor
	~implObj();

	//! Convenient typedef alias.
	typedef theme_font_tag<> reference_font;

	//! Inherited from elementObj::implObj

	void initialize(ONLY IN_THREAD) override;

	//! Recalculate the data

	//! Invoked by initialize() and from the pseudo-public object.
	void recalculate_metrics(ONLY IN_THREAD);

	//! Override get_focus_first()

	//! A focusable that once to be first in the tabbing order will
	//! be rudely repositioned after the last menu element in the menu bar.

	void get_focus_first(ONLY IN_THREAD, const focusable &) override;
};

LIBCXXW_NAMESPACE_END

#endif
