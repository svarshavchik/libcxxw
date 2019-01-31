/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panelayoutmanagerobj_h
#define x_w_panelayoutmanagerobj_h

#include <x/w/panelayoutmanagerfwd.H>
#include <x/w/panefactoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/gridlayoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! Do not use grid layout manager's methods directly. Only use methods
//! defined for the pane layout manager.
//!
//! \see panelayoutmanager

class panelayoutmanagerObj : public gridlayoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	panelayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~panelayoutmanagerObj() LIBCXX_HIDDEN;

	//! How many panes currently exist.
	size_t size() const;

	//! Return an existing element in the pane.
	elementptr get(size_t) const;

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
