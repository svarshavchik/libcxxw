/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainer_impl_h
#define listcontainer_impl_h

#include "listcontainer.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/container_elementfwd.H"
#include "hotspot_elementfwd.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/impl/themedimfwd.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/focus.H"
#include "x/w/elementfwd.H"
#include "x/w/impl/focus/focusable.H"

LIBCXXW_NAMESPACE_START

struct listcontainer_selected_color;
struct listcontainer_highlighted_color;
struct listcontainer_current_color;

//! Superclass of the list container implementation object

//! The list container implementation object stores a mixin with
//! theme-based dimensions which set the list items' style, and list colors.

typedef background_color_elementObj<hotspot_elementObj
				    <container_elementObj<child_elementObj>>,
				    listcontainer_selected_color,
				    listcontainer_highlighted_color,
				    listcontainer_current_color
				    > listcontainer_impl_superclass_t;

//! List container implementation object.

//! The implementation object for a container that uses the
//! \ref listlayoutmanager "list layout manager".

class LIBCXX_HIDDEN listcontainerObj::implObj :
	public listcontainer_impl_superclass_t {

 public:

	//! Constructor

	implObj(const container_impl &parent,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Implemented in the subclass.
	virtual const_themedim list_v_padding() const=0;

	//! Implemented in the subclass.
	virtual const_themedim list_left_padding() const=0;

	//! Implemented in the subclass.
	virtual const_themedim list_right_padding() const=0;
	//! Implemented in the subclass.

	virtual const_themedim list_inner_padding() const=0;
};

LIBCXXW_NAMESPACE_END

#endif
