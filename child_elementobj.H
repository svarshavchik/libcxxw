/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef child_elementobj_h
#define child_elementobj_h

#include "element.H"
#include "container.H"
#include "background_colorfwd.H"
#include "connection_threadfwd.H"
#include "x/w/metrics/axis.H"
#include "x/w/picturefwd.H"

LIBCXXW_NAMESPACE_START

//! An element that has a parent element.

class LIBCXX_HIDDEN child_elementObj : public elementObj::implObj {

	//! Set the background color in the draw_info for this display element.

	//! After obtaining the draw_info from the parent element, if this
	//! display element has a background color, the corresponding
	//! members in draw_info get updated accordingly.

	typedef std::function<void (IN_THREAD_ONLY, draw_info &,
				    child_elementObj &)>
		update_background_color_t;

	//! This display element's background color

	update_background_color_t current_background_color_thread_only=
		no_background();

	//! Whether this display element uses its own background color

	bool own_background_color_thread_only=false;

	//! Return a no-op update_background_color_t;

	static update_background_color_t no_background();

 public:

	THREAD_DATA_ONLY(current_background_color);
	THREAD_DATA_ONLY(own_background_color);

	//! My parent element
	const ref<containerObj::implObj> container;

	//! Constructor.

	//! Save parent element, increase this element's nesting level.

	child_elementObj(const ref<containerObj::implObj> &container,
			 const metrics::axis &horiz=metrics::axis(),
			 const metrics::axis &vert=metrics::axis());

	//! Destructor
	~child_elementObj();

	//////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj:

	//! Invoke parent_element->get_window_handler().
	generic_windowObj::handlerObj &get_window_handler() override;

	//! Invoke parent_element->get_window_handler().
	const generic_windowObj::handlerObj &get_window_handler() const
		override;

	//! Use parent display element's background color.

	void remove_background_color(IN_THREAD_ONLY) override;

	//! Use its own background color.

	void set_background_color(IN_THREAD_ONLY,
				  const background_color &) override;

	//! Whether set_background_color() is in force.

	bool has_own_background_color(IN_THREAD_ONLY) override;

	//! Invoke parent_element->draw_info(), and update the results.
	draw_info get_draw_info(IN_THREAD_ONLY,
				const rectangle &initial_viewport) override;

	//! Set this child element's background color
	void prepare_draw_info(IN_THREAD_ONLY,
			       draw_info &) override;

	//! update_visibility() invokes this callback.

	//! This element's inherited visibility is visible only if
	//! both this element is explicitly visible, and its parent is.

	void visibility_updated(IN_THREAD_ONLY, bool flag) override;

	/////////////////////////////////////////////////////////////////////
	//
	// Inherited from horizvertObj

	void horizvert_updated(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
