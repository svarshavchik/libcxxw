/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbarobj_h
#define x_w_scrollbarobj_h

#include <x/w/elementobj.H>
#include <x/w/focusableownerobj.H>
#include <x/w/scrollbarfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementaiton object

//! \see scrollbar

class scrollbarObj : public containerObj,
		     public focusableObj::ownerObj {

public:

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	scrollbarObj(const ref<implObj> &impl,
		     const ref<containerObj::implObj> &container_impl,
		     const ref<layoutmanagerObj::implObj> &layout_impl)
		LIBCXX_HIDDEN;

	//! Set the value of the scrollbar

	void set(scroll_v_t value);

	//! Return the scrollbar's current value
	scroll_v_t::value_type get_value() const;

	//! Return the scrollbar's current value
	scroll_v_t::value_type get_dragged_value() const;

	//! Reconfigure the scrollbar
	void reconfigure(const scrollbar_config &);

	//! Install a new callback.
	void on_update(const scrollbar_cb_t &) LIBCXX_PUBLIC;

	//! Destructor
	~scrollbarObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
