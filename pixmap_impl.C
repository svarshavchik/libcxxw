/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pixmap.H"
#include "connection_thread.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

pixmapObj::implObj::implObj(const const_pictformat &pixmap_pictformat,
			    const screen &screenref,
			    dim_t width,
			    dim_t height)
	: xidObj(screenref->impl->thread),
	  drawableObj::implObj(screenref->impl->thread, this->id_,
			       pixmap_pictformat),
	screenref(screenref),
	width(width),
	height(height)
{
	// We support creating pixmaps with logical width or height of 0.

	// Make sure the server is happy too. The lone pixel won't be used.

	if (width == 0 || height == 0)
		width=height=1;

	xcb_create_pixmap(conn()->conn,
			  (depth_t::value_type)pixmap_pictformat->depth,
			  pixmap_id(),
			  screenref->impl->xcb_screen->root,
			  (dim_t::value_type)width,
			  (dim_t::value_type)height);
}

pixmapObj::implObj::~implObj()
{
	xcb_free_pixmap(conn()->conn, pixmap_id());
}

////////////////////////////////////////////////////////////////////
//
// Inherited from drawableObj::implObj

screen pixmapObj::implObj::get_screen()
{
	return screenref;
}

const_screen pixmapObj::implObj::get_screen() const
{
	return screenref;
}

dim_t pixmapObj::implObj::get_width() const
{
	return width;
}

dim_t pixmapObj::implObj::get_height() const
{
	return height;
}

LIBCXXW_NAMESPACE_END
