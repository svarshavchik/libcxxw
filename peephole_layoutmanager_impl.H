/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_layoutmanager_impl_H
#define peephole_layoutmanager_impl_H

#include "peepholeobj.H"
#include "layoutmanager.H"
#include "x/w/elementfwd.H"
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

//! \ref peephole "peephole" implementation object.

class LIBCXX_HIDDEN peepholeObj::layoutmanager_implObj
	: public layoutmanagerObj::implObj {

	LOG_CLASS_SCOPE;

 public:
	//! The element in the peephole.

	const element peephole_element;

 private:
	//! The peephole element's most recent requested visibility.

	rectangle requested_visibility;

 public:

	//! Constructor
	layoutmanager_implObj(const ref<containerObj::implObj> &container_impl,
			      const element &peephole_element);

	//! Destructor
	~layoutmanager_implObj();

	//! Implement child_metrics_updated()

	//! My element's metrics must've been updated. Resize the element
	//! according to its preferred size.

	void child_metrics_updated(IN_THREAD_ONLY) override;

	//! Implement do_for_each_child().

	//! We always have one child element.
	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void (const element &e)> &)
		override;

	//! Implement create_public_object().

	//! We just construct a base layoutmanager object.
	layoutmanager create_public_object() override;

	//! Implement recalculate().

	void recalculate(IN_THREAD_ONLY) override;

	//! Implement process_updated_position().

	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position) override;

	//! Implement ensure_visibility().

	//! We can only be called to ensure peephole_element's visibility.

	void ensure_visibility(IN_THREAD_ONLY,
			       elementObj::implObj &e,
			       const rectangle &r) override;
};

LIBCXXW_NAMESPACE_END

#endif
