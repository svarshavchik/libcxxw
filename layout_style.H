/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef layout_style_h
#define layout_style_h

#include "peepholed_listcontainerfwd.H"
#include "peephole/peepholeobj.H"
#include "includes/x/w/listlayoutmanager.H"
#include "peephole/peepholedfwd.H"
#include "x/w/focusablefwd.H"
#include "focus/focusablefwd.H"

LIBCXXW_NAMESPACE_START

//! Specifies the style of the list created by a \ref new_listlayoutmanager.

//! An abstract class that implements create(), that takes a
//! opeepholeObj::implObj that new_listlayoutmanager creates, for the
//! parent element for the peepholed_listcontainer that create() creates.
//!
//! new_listlayoutmanager passes itself as the second parameter.
class layout_style_t {

public:
	//! Create the peepholed list container, with this style.

	virtual std::tuple<container, peepholed, focusable, focusable_impl>
	create(const ref<peepholeObj::implObj> &,
	       const new_listlayoutmanager &) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
