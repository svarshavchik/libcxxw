/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_itembuttonobj_h
#define x_w_itembuttonobj_h

#include "itemlayoutmanager/itembuttonfwd.H"
#include "x/w/containerobj.H"
#include "x/w/image_buttonfwd.H"
#include <x/functionalrefptrfwd.H>
#include <x/functionalrefptrobj.H>

LIBCXXW_NAMESPACE_START

//! The info we have on the items we are managing.

struct itemlayout_button_container {

	inline itemlayout_button_container(const itemlayout_callback_t &);

	//! Callback for each item's close buttons
	itemlayout_callback_t callback;

	//! A button with each item in the container.
	std::vector<itembutton> all_buttons;

	//! Any new button that has been added, but not processed yet.

	//! A new button gets added both to all_buttons and new_buttons,
	//! recalculate() initializes each new button, then clears this
	//! vector.
	std::vector<itembutton> new_buttons;

	//! Buttons that have been removed
	std::vector<itembutton> removed_buttons;
};

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
