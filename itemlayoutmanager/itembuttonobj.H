/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_itembuttonobj_h
#define x_w_itembuttonobj_h

#include "itemlayoutmanager/itembuttonfwd.H"
#include "x/w/containerobj.H"
#include "x/w/image_buttonfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see itembutton

class itembuttonObj: public containerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Our image button
	const image_button deletebutton;

	//! Constructor
	itembuttonObj(const ref<implObj> &impl,
		      const layout_impl &container_layout_impl,
		      const image_button &deletebutton) LIBCXX_HIDDEN;

	//! Destructor
	~itembuttonObj() LIBCXX_HIDDEN;

private:
	//! My index

	//! Only accessible while holding a lock on the item button container.

	size_t my_index_under_lock;

public:
	//! Access/update my index in the button array.
	inline constexpr size_t &index(item_info_t::lock &)
	{
		return my_index_under_lock;
	}
};

LIBCXXW_NAMESPACE_END

#endif
