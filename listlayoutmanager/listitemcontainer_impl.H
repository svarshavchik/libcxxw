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

	//! Mutex-protected shared state container.
	typedef mpobj<ref<listitem_sharedstateObj>> listitem_shared_state_t;

	//! Shared state

	//! For all practical purposes the protected ref is const. The
	//! constructor initializes this to a new object. At the time the
	//! constructor is called it is not known what are all the elements
	//! that make up a single list item. When the entire list item gets
	//! created, create_item_or_separator() replaces all elements'
	//! listitem_sharedstate with a reference to the same object.
	//!
	//! The end result is that all elements comrpising a single list item
	//! have the same sharedstate object.

	mutable listitem_shared_state_t listitem_sharedstate;

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

	//! Read the enabled flag
	bool enabled() const;

	//! Set the enabled flag
	void enabled(bool);

	//! Override enabled()
	bool enabled(IN_THREAD_ONLY) override;

	//! Whether this list item is selectable.
	bool selectable() const;

	//! Used during construction.
	ref<listitem_sharedstateObj> get_shared_state() const;

	//! Used during construction.
	void set_shared_state(const ref<listitem_sharedstateObj> &);

	//! Set the status change callback.

	void set_status_change_callback(const std::function<
					list_item_status_change_callback_t
					> &status_change_callback);

	//! Get the status change callback.
	std::function<list_item_status_change_callback_t
		      > get_status_change_callback() const;
};

LIBCXXW_NAMESPACE_END

#endif
