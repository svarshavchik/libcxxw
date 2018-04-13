/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef singletonlayoutmanager_impl_h
#define singletonlayoutmanager_impl_h

#include "layoutmanager.H"
#include "singletonlayoutmanagerobj.H"
#include <x/mpobj.H>
#include <vector>

LIBCXXW_NAMESPACE_START

//! \ref singletonlayoutmanager "Singleton layout manager" implementation.

class LIBCXX_HIDDEN singletonlayoutmanagerObj::implObj :
	public layoutmanagerObj::implObj {

	//! The container for the current element, and/or new elements.

	typedef mpobj<elementptr> current_element_t;

	//! The current element.

	current_element_t current_element;

 public:
	//! Constructor
	implObj(const container_impl &container_impl,
		const elementptr &initial_element);

	//! Destructor
	~implObj();

	//! Implement create_public_object().

	layoutmanager create_public_object() override;

	//! Inherited from layoutmanagerObj::implObj

	void do_for_each_child(ONLY IN_THREAD,
			       const function<void (const element &e)> &)
		override;

	//! New element, replace the existing one.
	void created(const element &e);

	//! Return the existing element.

	elementptr get();
 private:
	//! Return list_element's implementation object.

	//! Invoking initialize_if_needed()
	element_implptr get_list_element_impl(ONLY IN_THREAD);

 public:
	//! Left margin padding.

	//! The default implementation returns 0
	virtual dim_t get_left_padding(ONLY IN_THREAD);

	//! Right margin padding.

	//! The default implementation returns 0
	virtual dim_t get_right_padding(ONLY IN_THREAD);

	//! Top margin padding.

	//! The default implementation returns 0
	virtual dim_t get_top_padding(ONLY IN_THREAD);

	//! Bottom margin padding.

	//! The default implementation returns 0
	virtual dim_t get_bottom_padding(ONLY IN_THREAD);

	//! Implement recalculate()

	//! Computes metrics for the container based on the child element's
	//! metrics and padding and calls update_metrics().
	//!
	//! Calls process_updated_position().
	void recalculate(ONLY IN_THREAD) override;

	//! Set the container's default metrics.

	//! Takes the metrics computed by recalculate(), and sets its as the
	//! container's metrics.
	virtual void update_metrics(ONLY IN_THREAD,
				    const metrics::axis &horiz,
				    const metrics::axis &vert);

	//! Calculate and position the sole child element.

	//! The container's metrics should include any padding, so this
	//! would normally end up positioning the current element according
	//! to the specified padding. If the container is taller, the
	//! element is vertically centered. If the container is wider, the
	//! element maintains the set left padding, and the right padding
	//! gets increased.

	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position) override;
};

LIBCXXW_NAMESPACE_END

#endif
