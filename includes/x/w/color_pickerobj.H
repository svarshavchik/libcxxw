/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_pickerobj_h
#define x_w_color_pickerobj_h

#include <x/w/color_pickerfwd.H>
#include <x/w/focusable_containerobj.H>
#include <x/w/rgbfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see color_picker

class color_pickerObj : public focusable_containerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	color_pickerObj(const ref<implObj> &impl,
			const layout_impl &container_layoutmanager)
		LIBCXX_HIDDEN;

	//! Destructor
	~color_pickerObj() LIBCXX_HIDDEN;

	//! Override get_impl().
	focusable_impl get_impl() const override LIBCXX_HIDDEN;

	//! Install a new callback that gets invoked to report a new color.

	void on_color_update(const functionref<color_picker_callback_t> &cb);

	//! Return the currently selected color.

	rgb current_color() const;

	//! Set the current color

	void current_color(const rgb &);

	//! Set the current color

	void current_color(ONLY IN_THREAD, const rgb &);
};

LIBCXXW_NAMESPACE_END

#endif
