/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef child_elementobj_h
#define child_elementobj_h

#include "element.H"
#include "container.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! An element that has a parent element.

class LIBCXX_HIDDEN child_elementObj : public elementObj::implObj {


 public:

	//! My parent element
	const ref<containerObj::implObj> container;

	//! Constructor.

	//! Save parent element, increase this element's nesting level.

	child_elementObj(const ref<containerObj::implObj> &container,
			 const rectangle &initial_position);

	~child_elementObj();

	//////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj:

	//! Invoke parent_element->get_window_handler().
	generic_windowObj::handlerObj &get_window_handler() override;

	//! Invoke parent_element->get_window_handler().
	const generic_windowObj::handlerObj &get_window_handler() const
		override;

	//! Invoke parent_element->draw_info(), and update the results.
	draw_info get_draw_info(IN_THREAD_ONLY,
				const rectangle &initial_viewport) override;

	/////////////////////////////////////////////////////////////////////
	//
	// Inherited from horizvertObj

	void horizvert_updated(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
