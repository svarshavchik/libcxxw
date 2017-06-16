/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemcontainer_impl_h
#define listitemcontainer_impl_h

#include "listlayoutmanager/listitemcontainer.H"
#include "listlayoutmanager/listcontainerobj.H"
#include "container_elementfwd.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! "Internal-internal" list item container implementation object.

//! The implementation object overrides pointer_focus() and forwards it
//! to the list container, so that the list item can be properly highlighted.
//!
//! Depending on the configuration of the list there may be more than one
//! listitemcontainer item per row. The first listitemcontainer item
//! implementation object's bool flag indicates whether this item is selected,
//! or not.
//!
//! \see listitemcontainer

class LIBCXX_HIDDEN listitemcontainerObj::implObj
	: public container_elementObj<child_elementObj> {

	//! My superclass.

	typedef container_elementObj<child_elementObj> superclass_t;

	//! Internal flag

	mutable mpobj<bool> flag{false};

 public:

	//! parent container.

	const ref<listcontainerObj::implObj> parent_container;

	//! Constructor
	implObj(const ref<listcontainerObj::implObj> &parent_container,
		const child_element_init_params &params);

	//! Destructor
	~implObj();

	//! Pointer focus change, report it to the list layout manager.
	void pointer_focus(IN_THREAD_ONLY) override;

	//! Read the selected flag
	bool selected() const;

	//! Set the selected flag
	void selected(bool);
};

LIBCXXW_NAMESPACE_END

#endif
