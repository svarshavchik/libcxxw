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

#include "x/w/pagelayoutmanager.H"
#include "x/w/container.H"

LIBCXXW_NAMESPACE_START

booklayoutmanagerObj::implObj
::implObj(const ref<bookgridlayoutmanagerObj> &impl)
	: impl{impl},

	  // This stuff lives in known grid positions.
	  book_switchcontainer{impl->get(1,0)},
	  book_pagelayoutmanager{book_switchcontainer->get_layoutmanager()},
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
		gridlayoutmanager g=e->get_layoutmanager();

		p=g->get(0, 0);
	}

	return p;
}



LIBCXXW_NAMESPACE_END
