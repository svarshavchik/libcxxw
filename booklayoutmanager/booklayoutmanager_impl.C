/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/booklayoutmanager_impl.H"
#include "booklayoutmanager/pagetabgridlayoutmanager.H"
#include "booklayoutmanager/pagetab.H"
#include "peephole/peephole.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "image_button.H"
#include "x/w/impl/container.H"
#include "x/w/impl/singletonlayoutmanager.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/container.H"

LIBCXXW_NAMESPACE_START

booklayoutmanagerObj::implObj
::implObj(const ref<bookgridlayoutmanagerObj> &impl)
	: impl{impl},

	  // This stuff lives in known grid positions.
	  // left_scrollbutton's position is also ass-umed in
	  // book_focusable_containerObj::get_impl()
	  left_scrollbutton{impl->get(0,0)},
	  right_scrollbutton{impl->get(0,2)},
	  book_pagecontainer{impl->get(1,0)},
	  book_pagelayoutmanager{book_pagecontainer->get_layoutmanager()},
	  book_pagetabgrid{container(peephole(impl->get(0, 1))
				     ->get_peepholed())},
	  book_pagetabgridlayoutmanager{book_pagetabgrid->get_layoutmanager()}
{
}

booklayoutmanagerObj::implObj::~implObj()=default;

pagetabptr booklayoutmanagerObj::implObj::get_pagetab(size_t index) const
{
	pagetabptr p;

	containerptr e=book_pagetabgridlayoutmanager->get(0, index);

	if (e)
	{
		e->impl->invoke_layoutmanager
			([&]
			 (const ref<singletonlayoutmanagerObj::implObj> &slm)
			 {
				 p=slm->get();
			 });
	}

	return p;
}

std::vector<focusable> booklayoutmanagerObj::implObj
::get_focusables(grid_map_t::lock &) const
{
	std::vector<focusable> v;

	auto n=book_pagelayoutmanager->pages();

	v.reserve(n+2);

	v.push_back(left_scrollbutton);
	for (size_t i=0; i<n; ++i)
		v.push_back(get_pagetab(i));
	v.push_back(right_scrollbutton);

	return v;
}

void booklayoutmanagerObj::implObj::rebuild_focusables(grid_map_t::lock &l)
	const
{
	auto v=get_focusables(l);

	// The first focusable is the left scroll button, which is our
	// "anchor". get_focus_after_me will simply ignore itself in the
	// vector.
	v.front()->get_focus_after_me(v);
}

LIBCXXW_NAMESPACE_END
