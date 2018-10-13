/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_itembuttonfwd_h
#define x_w_itembuttonfwd_h

#include "x/w/namespace.H"
#include "x/w/itemlayoutmanagerfwd.H"
#include <x/ptrfwd.H>
#include <vector>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN itembuttonObj;

/*! A single button in the item layout manager

Internal display element a container with a button, and a label for an item.

*/

typedef ref<itembuttonObj> itembutton;

//! A constant \ref itembutton "item button".

//! \see itembutton

typedef const_ref<itembuttonObj> const_itembutton;

//! A nullable pointer reference to a \ref itembutton "item button".

//! \see itembutton

typedef ptr<itembuttonObj> itembuttonptr;

//! A nullable pointer reference to a const \ref itembutton "item button".

//! \see itembutton

typedef const_ptr<itembuttonObj> const_itembuttonptr;

struct LIBCXX_HIDDEN itemlayout_button_container;

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


LIBCXXW_NAMESPACE_END

#endif
