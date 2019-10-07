/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pagelayoutcontainer_impl_h
#define pagelayoutcontainer_impl_h

#include "pagelayoutcontainer.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/always_visiblefwd.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class pagelayoutcontainerObj::implObj : public always_visibleObj<
	container_elementObj<child_elementObj>, false> {

	//! Alias
	typedef always_visibleObj<
		container_elementObj<child_elementObj>, false> superclass_t;
 public:

	//! Constructor
	implObj(const container_impl &parent);

	//! Destructor
	~implObj();

	//! Override draw-after_visibility_updated

	//! Sets preclear_flag if we were made visible.
	void draw_after_visibility_updated(ONLY IN_THREAD,
					   bool flag) override;

	//! Override do_draw

	//! If preclear_flag is set, clear the container to the background
	//! color ourselves, then manually schedule our child elements
	//! for recursive redrawing. It should already be scheduled for
	//! redrawing, but this makes sure of that. It is more visually
	//! pleasing to immediately clear the page, then redraw the child
	//! containers, when switching pages.
	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas) override;
private:

	bool preclear_flag=false;
};

LIBCXXW_NAMESPACE_END

#endif
