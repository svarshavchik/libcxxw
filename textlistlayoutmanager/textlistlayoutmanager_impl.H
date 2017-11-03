/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef textlistlayoutmanager_impl_h
#define textlistlayoutmanager_impl_h

#include "x/w/textlistlayoutmanager.H"
#include "singletonlayoutmanager_impl.H"
#include "textlistlayoutmanager/textlistfwd.H"
#include "textlistlayoutmanager/textlist_container_impl.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! The implementation object for the textlistlayoutmanager is a subclass
//! of the singleton layout manager. The container contains a single element,
//! a textlistObj::implObj, which handles the list functions.

class LIBCXX_HIDDEN textlistlayoutmanagerObj::implObj
	: public singletonlayoutmanagerObj::implObj {

 public:

	//! Our only singleton element.

	//! Stores the actual contents of the list.

	const textlist textlist_element;

	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl,
		const textlist &textlist_element);

	//! Destructor
	~implObj();

	//! Override create_public_object
	layoutmanager create_public_object() override;

	//! Implement recalculate().

	//! Forward recalculate() to textlist_impl's.

	void recalculate(IN_THREAD_ONLY) override;

	//! Implement child_metrics_updated().

	//! textlist_element's metrics get copied to our metrics.

	void child_metrics_updated(IN_THREAD_ONLY) override;

	//! Implement process_updated_position().

	//! textlist_element() is sized to match our dimensions.
	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position) override;

	//! Implement theme_updated().

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
		override;

	//! Report the tallest row height.

	//! Used to size the combo-box current element. The default
	//! implementation does nothing.
	virtual void update_tallest_row_height(IN_THREAD_ONLY, dim_t);
};

LIBCXXW_NAMESPACE_END

#endif
