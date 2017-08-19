/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef singletonlayoutmanager_impl_h
#define singletonlayoutmanager_impl_h

#include "layoutmanager.H"
#include "singletonlayoutmanagerobj.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! \ref singletonlayoutmanager "Singleton layout manager" implementation.

class LIBCXX_HIDDEN singletonlayoutmanagerObj::implObj :
	public layoutmanagerObj::implObj {

 public:
	//! The container for the current element.

	typedef mpobj<elementptr> current_element_t;

	//! The current element.

	current_element_t current_element;

	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl,
		const elementptr &initial_element);

	//! Destructor
	~implObj();

	//! Implement create_public_object().

	layoutmanager create_public_object() override;

	//! Inherited from layoutmanagerObj::implObj

	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void (const element &e)> &)
		override;

 private:
	//! Return list_element's implementation object.

	//! Invoking initialize_if_needed()
	ptr<elementObj::implObj> get_list_element_impl(IN_THREAD_ONLY);

 public:
	//! Left margin padding.

	//! The default implementation returns 0
	virtual dim_t get_left_padding(IN_THREAD_ONLY);

	//! Right margin padding.

	//! The default implementation returns 0
	virtual dim_t get_right_padding(IN_THREAD_ONLY);

	//! Top margin padding.

	//! The default implementation returns 0
	virtual dim_t get_top_padding(IN_THREAD_ONLY);

	//! Bottom margin padding.

	//! The default implementation returns 0
	virtual dim_t get_bottom_padding(IN_THREAD_ONLY);

	//! Implement recalculate()

	//! Computes metrics for the container based on the child element's
	//! metrics and padding and calls update_metrics().
	//!
	//! Calls process_updated_position().
	void recalculate(IN_THREAD_ONLY) override;

	//! Set the container's default metrics.

	//! Takes the metrics computed by recalculate(), and sets its as the
	//! container's metrics.
	virtual void update_metrics(IN_THREAD_ONLY,
				    const metrics::axis &horiz,
				    const metrics::axis &vert);

	//! Calculate and position the sole child element.

	//! The container's metrics should include any padding, so this
	//! would normally end up positioning the current element according
	//! to the specified padding. If the container is taller, the
	//! element is vertically centered. If the container is wider, the
	//! element maintains the set left padding, and the right padding
	//! gets increased.

	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position);
};

LIBCXXW_NAMESPACE_END

#endif
