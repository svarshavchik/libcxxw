/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_buttonobj_h
#define x_w_image_buttonobj_h

#include <x/w/image_buttonfwd.H>
#include <x/w/containerobj.H>
#include <x/w/focusableobj.H>
#include <x/w/factoryfwd.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! A loaded image_button

//! \see image_button

class image_buttonObj : public containerObj,
			public focusableObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	image_buttonObj(const ref<implObj> &impl,
			const container_impl &container_impl,
			const layout_impl &lm_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~image_buttonObj() LIBCXX_HIDDEN;

	//! Implement get_impl().
	focusable_impl get_impl() const override;

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

	//! Replace the label widget next to the image button.

	template<typename functor>
	void update_label(functor &&f)
	{
		do_update_label(make_function<void (const factory &)>
				(std::forward<functor>(f)));
	}

	//! Type-erased update_label()

	void do_update_label(const function<void (const factory &)> &);
};

LIBCXXW_NAMESPACE_END

#endif
