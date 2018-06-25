/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagetabsingletonlayoutmanager_implobj_h
#define x_w_pagetabsingletonlayoutmanager_implobj_h

#include <booklayoutmanager/pagetab_impl.H>
#include "x/w/impl/singletonlayoutmanager.H"

LIBCXXW_NAMESPACE_START

//! Subclass of singletonlayoutmanagerObj::implObj used by pagetabObj::implObj

//! Subclass provides padding for the tab element.

class LIBCXX_HIDDEN pagetabsingletonlayoutmanager_implObj :
	public singletonlayoutmanagerObj::implObj {

public:

	//! My container
	const ref<pagetabObj::implObj> pagetab_container;

	//! Constructor
	pagetabsingletonlayoutmanager_implObj
		(const ref<pagetabObj::implObj> &pagetab_container,
		 const element &initial_element);

	//! Destructor
	~pagetabsingletonlayoutmanager_implObj();

	//! Override get_left_padding().

	dim_t get_left_padding(ONLY IN_THREAD) override;

	//! Override get_right_padding().

	dim_t get_right_padding(ONLY IN_THREAD) override;

	//! Override get_top_padding().

	dim_t get_top_padding(ONLY IN_THREAD) override;

	//! Override get_bottom_padding().

	dim_t get_bottom_padding(ONLY IN_THREAD) override;
};

LIBCXXW_NAMESPACE_END

#endif
