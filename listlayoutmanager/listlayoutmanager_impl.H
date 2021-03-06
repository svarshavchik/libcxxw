/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listlayoutmanager_impl_h
#define listlayoutmanager_impl_h

#include "x/w/listlayoutmanager.H"
#include "x/w/impl/singletonlayoutmanager.H"
#include "listlayoutmanager/list_elementfwd.H"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"
#include "peepholed_listcontainerobj.H"
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! The implementation object for the listlayoutmanager is a subclass
//! of the singleton layout manager. The container contains a single element,
//! a textlistObj::implObj, which handles the list functions.

class LIBCXX_HIDDEN listlayoutmanagerObj::implObj
	: public singletonlayoutmanagerObj::implObj {

 public:

	//! Our container.

	const ref<listcontainer_pseudo_implObj> container_impl;

	//! Our only singleton element.

	//! Stores the actual contents of the list.

	const list_element list_element_singleton;

	//! Constructor
	implObj(const ref<listcontainer_pseudo_implObj> &container_impl,
		const list_element &list_element_singleton);

	//! Destructor
	~implObj();

	//! Override create_public_object
	layoutmanager create_public_object() override;

	//! Create our public object.

	listlayoutmanager create_listlayoutmanager();

	//! Implement recalculate().

	//! Forward recalculate() to textlist_impl's.

	void recalculate(ONLY IN_THREAD) override;

	//! Implement child_metrics_updated().

	//! textlist_element's metrics get copied to our metrics.

	void child_metrics_updated(ONLY IN_THREAD) override;

	//! Override recalculate()

	//! textlist_element() is sized to match our dimensions.
	void recalculate(ONLY IN_THREAD,
			 const element_impl &impl) override;

	//! Implement theme_updated().

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
		override;

	//! Report the tallest row height.

	//! Used to size the combo-box current element. The default
	//! implementation does nothing.
	virtual void update_tallest_row_height(ONLY IN_THREAD,
					       const tallest_row_height_t &);
};

LIBCXXW_NAMESPACE_END

#endif
