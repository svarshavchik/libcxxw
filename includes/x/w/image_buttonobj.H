/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_buttonobj_h
#define x_w_image_buttonobj_h

#include <x/w/image_buttonfwd.H>
#include <x/w/containerobj.H>
#include <x/w/focusableobj.H>

LIBCXXW_NAMESPACE_START

//! A loaded image_button

//! \see image_button

class image_buttonObj : public containerObj,
			public focusableObj {

public:
	class implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	image_buttonObj(const ref<implObj> &impl,
			const ref<containerObj::implObj> &container_impl,
			const ref<layoutmanagerObj::implObj> &lm_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~image_buttonObj() LIBCXX_HIDDEN;

	//! Implement get_impl().
	ref<focusableImplObj> get_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
