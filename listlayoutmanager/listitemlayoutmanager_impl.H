/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemlayoutmanager_impl_h
#define listitemlayoutmanager_impl_h

#include "listlayoutmanager/listitemlayoutmanager.H"
#include "themedimfwd.H"
#include "connection_threadfwd.H"
#include "singletonlayoutmanager_impl.H"
#include "x/w/elementfwd.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! List item layout manager implementation object.

//! \see listlayoutmanager

class LIBCXX_HIDDEN listitemlayoutmanagerObj::implObj
	: public singletonlayoutmanagerObj::implObj {

	//! Horizontal padding.
	const themedim left_padding;

	//! Horizontal padding.
	const themedim right_padding;

	//! Top and bottom padding
	const themedim v_padding;

 public:
	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl,
		const element &initial_element,
		const themedim &left_padding,
		const themedim &right_padding,
		const themedim &v_padding);

	//! Destructor
	~implObj();

	//! Inherited from singletonlayoutmanagerObj
	dim_t get_left_padding(IN_THREAD_ONLY) override;

	//! Inherited from singletonlayoutmanagerObj
	dim_t get_right_padding(IN_THREAD_ONLY) override;

	//! Inherited from singletonlayoutmanagerObj
	dim_t get_top_padding(IN_THREAD_ONLY) override;

	//! Inherited from singletonlayoutmanagerObj
	dim_t get_bottom_padding(IN_THREAD_ONLY) override;

	//! Inherited from layoutmanagerObj::implObj

	layoutmanager create_public_object() override;

	//! Inherited from layoutmanagerObj::implObj

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;
};

LIBCXXW_NAMESPACE_END

#endif
