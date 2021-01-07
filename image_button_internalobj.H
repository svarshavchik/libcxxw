/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_button_internalobj_h
#define image_button_internalobj_h

#include "x/w/impl/iconfwd.H"
#include "image_button_internalfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/imageobj.H"
#include "x/w/focusableobj.H"

LIBCXXW_NAMESPACE_START

//! A loaded image_button_internal

//! \see image_button_internal

class LIBCXX_HIDDEN image_button_internalObj : public imageObj,
					       public focusableObj {

public:
	class implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	image_button_internalObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~image_button_internalObj() LIBCXX_HIDDEN;

	focusable_impl get_impl() const override;

	//! Resize the icon images.
	void resize(ONLY IN_THREAD,
		    dim_t w, dim_t h, icon_scale scale);
};

LIBCXXW_NAMESPACE_END

#endif
