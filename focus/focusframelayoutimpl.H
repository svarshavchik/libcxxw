/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframelayoutimpl_h
#define focusframelayoutimpl_h

#include "focus/focusframelayoutimplfwd.H"
#include "x/w/impl/focus/focusframecontainer_impl.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/current_border_implfwd.H"
#include "x/w/impl/themeborder_elementfwd.H"
#include "x/w/containerobj.H"

LIBCXXW_NAMESPACE_START

//! Subclass of gridlayoutmanager implementation used by the focusframecontainer

//! \ref focusframecontainer "focusframecontainer" uses this subclass of the
//! grid layout manager.

class LIBCXX_HIDDEN focusframelayoutimplObj
	: public gridlayoutmanagerObj::implObj {


 public:
	//! My container.
	const ref<focusframecontainer_implObj> focusframe_container_impl;

	//! Constructor
	focusframelayoutimplObj(const ref<focusframecontainer_implObj> &,
				const element &);

	//! Destructor
	~focusframelayoutimplObj();

	//! Set the appropriate border for our element.

	void rebuild_elements_start(ONLY IN_THREAD,
				    grid_map_t::lock &lock) override;

};

LIBCXXW_NAMESPACE_END

#endif
