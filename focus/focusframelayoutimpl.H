/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframelayoutimpl_h
#define focusframelayoutimpl_h

#include "focus/focusframelayoutimplfwd.H"
#include "focus/focusframecontainerobj.H"
#include "gridlayoutmanager.H"
#include "current_border_implfwd.H"
#include "themeborder_elementfwd.H"
#include "x/w/containerobj.H"

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN focusoff_border_tag;
struct LIBCXX_HIDDEN focuson_border_tag;

template<typename element_impl>
using focusframe_container_and_borderObj=
	themeborder_elementObj<element_impl,
			       focusoff_border_tag,
			       focuson_border_tag>;

//! Subclass of gridlayoutmanager implementation used by the focusframecontainer

//! \ref focusframecontainer "focusframecontainer" uses this subclass of the
//! grid layout manager.

class LIBCXX_HIDDEN focusframelayoutimplObj
	: public gridlayoutmanagerObj::implObj {


 public:
	//! My container.
	const ref<focusframecontainerObj::implObj> container_impl;

	//! Constructor
	focusframelayoutimplObj(const ref<focusframecontainerObj::implObj> &);

	//! Destructor
	~focusframelayoutimplObj();

	//! Set the appropriate border for our element.

	void rebuild_elements_start(ONLY IN_THREAD,
				    grid_map_t::lock &lock) override;

};

LIBCXXW_NAMESPACE_END

#endif
