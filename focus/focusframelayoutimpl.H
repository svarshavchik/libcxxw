/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframelayoutimpl_h
#define focusframelayoutimpl_h

#include "focus/focusframelayoutimplfwd.H"
#include "gridlayoutmanager.H"
#include "current_border_implfwd.H"
#include "x/w/containerobj.H"

LIBCXXW_NAMESPACE_START

//! Subclass of gridlayoutmanager implementation used by the focusframecontainer

//! \ref focusframecontainer "focusframecontainer" uses this subclass of the
//! grid layout manager.

class LIBCXX_HIDDEN focusframelayoutimplObj
	: public gridlayoutmanagerObj::implObj {


 public:

	//! Convenience alias
	typedef gridlayoutmanagerObj::grid_map_t grid_map_t;

	//! Border we insert around the element.
	const current_border_impl focusoff_border;

	//! Border we insert around the element.
	const current_border_impl focuson_border;

	//! Constructor
	focusframelayoutimplObj(const ref<containerObj::implObj>
				&container_impl,
				const char *off_border,
				const char *on_border);

	//! Destructor
	~focusframelayoutimplObj();

	//! Set the appropriate border for our element.

	void rebuild_elements_start(IN_THREAD_ONLY,
				    grid_map_t::lock &lock) override;

};

LIBCXXW_NAMESPACE_END

#endif
