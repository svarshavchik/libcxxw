/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/booklayoutmanager_impl.H"
#include "booklayoutmanager/pagetabgridlayoutmanager_impl.H"
#include "booklayoutmanager/pagetab.H"
#include "peephole/peephole.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "pagelayoutmanager_impl.H"
#include "image_button.H"
#include "grid_map_info.H"
#include "x/w/impl/container.H"
#include "x/w/impl/singletonlayoutmanager.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/container.H"

LIBCXXW_NAMESPACE_START

booklayoutmanagerObj::implObj
::implObj(const ref<bookgridlayoutmanagerObj> &impl)
	: implObj{impl, grid_map_t::lock{impl->grid_map}}
{
}

booklayoutmanagerObj::implObj
::implObj(const ref<bookgridlayoutmanagerObj> &impl,
	  grid_map_t::lock &&grid_lock)

	: impl{impl},

	  // This stuff lives in known grid positions.
	  // left_scrollbutton's position is also ass-umed in
	  // book_focusable_containerObj::get_impl()
	  left_scrollbutton{(*grid_lock)->get(0,0)},
	  right_scrollbutton{(*grid_lock)->get(0,2)},
	  book_pagecontainer{(*grid_lock)->get(1,0)},
	  book_pagelayoutmanager_impl{book_pagecontainer->get_layout_impl()},
	  book_pagetabgrid{container{peephole((*grid_lock)->get(0, 1))
				     ->peepholed_element}},
	  book_pagetabgridlayoutmanager_impl{book_pagetabgrid
					     ->get_layout_impl()}
{
}

booklayoutmanagerObj::implObj::~implObj()=default;

pagelayoutmanager booklayoutmanagerObj::implObj::book_pagelayoutmanager()
{
	return book_pagelayoutmanager_impl->create_pagelayoutmanager();
}

pagetabgridlayoutmanager
booklayoutmanagerObj::implObj::book_pagetabgridlayoutmanager()
{
	return book_pagetabgridlayoutmanager_impl
		->create_pagetabgridlayoutmanager();
}

pagetabptr booklayoutmanagerObj::implObj::get_pagetab(book_lock &lock,
						      size_t index)
{
	return get_pagetab(lock.grid_lock, index);
}

pagetabptr booklayoutmanagerObj::implObj::get_pagetab(grid_map_t::lock &lock,
						      size_t index)
{
	pagetabptr p;

	containerptr e=(*lock)->get(0, index);

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

std::vector<focusable> booklayoutmanagerObj::implObj::get_focusables() const
{
	grid_map_t::lock grid_lock{book_pagetabgridlayoutmanager_impl
				   ->grid_map};

	std::vector<focusable> v;

	size_t n=book_pagelayoutmanager_impl->pages();

	v.reserve(n+2);

	v.push_back(left_scrollbutton);
	for (size_t i=0; i<n; ++i)
		v.push_back(get_pagetab(grid_lock, i));
	v.push_back(right_scrollbutton);

	return v;
}

focusable booklayoutmanagerObj::implObj::get_focusable() const
{
	grid_map_t::lock grid_lock{book_pagetabgridlayoutmanager_impl
				   ->grid_map};

	if (book_pagelayoutmanager_impl->pages() > 0)
		return get_pagetab(grid_lock, 0);

	return left_scrollbutton;
}

void booklayoutmanagerObj::implObj::rebuild_focusables()
	const
{
	auto v=get_focusables();

	// The first focusable is the left scroll button, which is our
	// "anchor". get_focus_after_me will simply ignore itself in the
	// vector.
	v.front()->get_focus_after_me(v);
}

LIBCXXW_NAMESPACE_END
