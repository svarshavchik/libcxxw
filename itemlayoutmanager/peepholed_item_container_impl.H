/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_item_container_impl_h
#define peepholed_item_container_impl_h

#include "itemlayoutmanager/peepholed_item_container.H"
#include "x/w/itemlayoutmanagerfwd.H"
#include "x/w/focusablefwd.H"
#include "x/w/impl/theme_font_elementfwd.H"
#include "x/w/impl/themedim_elementfwd.H"
#include "x/w/impl/always_visible_elementfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

struct itemlayout_h_padding;
struct itemlayout_v_padding;

//! Internal implementation object.

class peepholed_item_containerObj::implObj
	: public theme_font_elementObj<themedim_elementObj
				       <always_visible_elementObj<
						container_elementObj
						<child_elementObj>>,
					itemlayout_h_padding,
					itemlayout_v_padding>> {

	typedef theme_font_elementObj<themedim_elementObj
				      <always_visible_elementObj<
					       container_elementObj
					       <child_elementObj>>,
				       itemlayout_h_padding,
				       itemlayout_v_padding
				       >> superclass_t;

public:

	//! Our peephole's horizontal scrollbar
	const focusable horizontal_scrollbar;

	//! Our peephole's vertical scrollbar
	const focusable vertical_scrollbar;

	implObj(const container_impl &parent,
		const focusable &horizontal_scrollbar,
		const focusable &vertical_scrollbar,
		const new_itemlayoutmanager &config);

	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
