/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainer_impl_h
#define listcontainer_impl_h

#include "listcontainer.H"
#include "child_element.H"
#include "container_elementfwd.H"
#include "themedim_elementfwd.H"
#include "hotspot_elementfwd.H"
#include "background_color_elementfwd.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/focus.H"
#include "x/w/elementfwd.H"
#include "focus/focusable.H"

LIBCXXW_NAMESPACE_START

struct listcontainer_dim_v;
struct listcontainer_dim_left;
struct listcontainer_dim_inner;
struct listcontainer_dim_right;

struct listcontainer_selected_color;
struct listcontainer_highlighted_color;
struct listcontainer_current_color;

//! Superclass of the list container implementation object

//! The list container implementation object stores a mixin with
//! theme-based dimensions which set the list items' style, and list colors.

typedef themedim_elementObj<background_color_elementObj<
				    hotspot_elementObj
				    <container_elementObj<child_elementObj>>,
				    listcontainer_selected_color,
				    listcontainer_highlighted_color,
				    listcontainer_current_color
				    >,
			    listcontainer_dim_v,
			    listcontainer_dim_left,
			    listcontainer_dim_inner,
			    listcontainer_dim_right
			    > listcontainer_impl_superclass_t;

//! List container implementation object.

//! The implementation object for a container that uses the
//! \ref listlayoutmanager "list layout manager".

class LIBCXX_HIDDEN listcontainerObj::implObj :
	public listcontainer_impl_superclass_t {

 public:

	//! Constructor

	implObj(const ref<containerObj::implObj> &parent,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Create an intermediate container for any new child element.

	//! When a new child element gets created for this container,
	//! its actual parent container is an intermediate container
	//! with a listitemlayoutmanager.
	//!
	//! The listlayoutmanager's factory collects each created element,
	//! and when there's enough of them for a new list item, they
	//! get added to the list layout manager.

	ref<containerObj::implObj>
		parent_for_new_child(const ref<containerObj::implObj> &parent)
		override;

	//! An element in the list container has gained or lost pointer focus

	void pointer_focus(IN_THREAD_ONLY, const ref<elementObj::implObj> &e);

	//! Override temperature_changed
	void temperature_changed(IN_THREAD_ONLY) override;

	//! Clicked.
	void activated(IN_THREAD_ONLY) override;

	//! Override set_focus_and_ensure_visibility()

	void set_focus_and_ensure_visibility(IN_THREAD_ONLY) override;

	//! Gained/lost keyboard focus.
	void keyboard_focus(IN_THREAD_ONLY) override;

	//! Handle cursor down/up
	bool process_key_event(IN_THREAD_ONLY, const key_event &ke) override;
};

LIBCXXW_NAMESPACE_END

#endif
