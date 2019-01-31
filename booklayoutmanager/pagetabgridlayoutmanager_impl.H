/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pagetabgridlayoutmanager_impl_h
#define pagetabgridlayoutmanager_impl_h

#include "gridlayoutmanager.H"
#include "booklayoutmanager/pagetabgridlayoutmanager.H"
#include "booklayoutmanager/pagetabgridcontainer_implfwd.H"

LIBCXXW_NAMESPACE_START

//! The layout manager for a \ref booklayoutmanager "booklayoutmanager"'s page tab.

//! This is a subclass of the grid layout manager implementation object.
//! The page tab is a grid, with a cell that correspond to each page.

class LIBCXX_HIDDEN pagetabgridlayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

	//! Alias.

	typedef gridlayoutmanagerObj::implObj superclass_t;

 public:

	//! My container.
	const pagetabgridcontainer_impl my_container;

	//! The page tab is placed inside a peephole, that scrolls it.
	//! This is the peephole's container, the parent of the container
	//! with the pagetabgridlayoutmanager.

	const container_impl parent_container;

	//! Constructor
	implObj(const pagetabgridcontainer_impl &my_container,
		const container_impl &parent_container);

	//! Destructor
	~implObj();

	//! Implement create_public_object()

	//! Invoke create_pagetabgridlayoutmanager().
	layoutmanager create_public_object() override;

	//! Construct a pagetabgridlayoutmanager.
	pagetabgridlayoutmanager create_pagetabgridlayoutmanager();

	//! Override recalculate().

	//! After the gridlayoutmanager superclass does its recalculations, we
	//! update the parent container's metrics. The peephole's vertical
	//! metrics match the page tab grid's. The peephole's horizontal
	//! metrics's minimum size is the size of the widest individual
	//! tab, and the maximum width is unlimited.
	//!
	//! The peephole is on the first row of the grid that makes up the
	//! notebooklayoutmanager-managed container. In this manner, the
	//! peephole gets all the extra width of the first row, sandwiched
	//! between the scroll buttons.

	void recalculate(ONLY IN_THREAD) override;

};

LIBCXXW_NAMESPACE_END

#endif
