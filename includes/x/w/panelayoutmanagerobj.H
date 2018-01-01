/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panelayoutmanagerobj_h
#define x_w_panelayoutmanagerobj_h

#include <x/w/panelayoutmanagerfwd.H>
#include <x/w/panefactoryfwd.H>
#include <x/w/layoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see panelayoutmanager

class panelayoutmanagerObj : public layoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	panelayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~panelayoutmanagerObj() LIBCXX_HIDDEN;

	//! How many panes currently exist.
	size_t size() const;

	//! Append panes after all existing ones.
	panefactory append_panes();

	//! Insert panes before an existing pane.
	panefactory insert_panes(size_t);

	//! Replace one or more panes.
	panefactory replace_panes(size_t);

	//! Remove a pane.
	void remove_pane(size_t);

	//! Remove all panes.
	void remove_all_panes();

	//! Remove all panes immediately, then create new ones.
	panefactory replace_all_panes();
};

LIBCXXW_NAMESPACE_END

#endif
