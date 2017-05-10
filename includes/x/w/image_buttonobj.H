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

	//! Which image is currently being shown by this image button.

	//! For checkboxes and radio buttons 0 means unselected, 1 means
	//! selected. Values greater than one are possible for custom
	//! configured image buttons.

	size_t get_value() const;

	//! Set the image being shown by this image button.

	//! The image number gets updated by the connection thread, so
	//! immediately calling get_value() may not reflect the new value.

	void set_value(size_t);

	//! Install a new activation callback.

	void on_activate(const image_button_callback_t &callback);
};

LIBCXXW_NAMESPACE_END

#endif
