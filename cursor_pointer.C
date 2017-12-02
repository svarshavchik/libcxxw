/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "cursor_pointer.H"
#include "icon.H"
#include "pixmap_with_picture.H"
#include "defaulttheme.H"
#include "screen.H"
#include "cursor_pointer_cache.H"
#include "pixmap.H"
#include "picture.H"
#include "xid_t.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

cursor_pointerObj::cursor_pointerObj(const icon &cursor_pointer_icon)
	: xidObj(cursor_pointer_icon->image->impl->screenref->impl->thread),
	  cursor_pointer_icon(cursor_pointer_icon)
{
	const auto &points=cursor_pointer_icon->image
		->points_of_interest;

	coord_t x{}, y{};

	auto iter=points.find("hotspot");

	if (iter != points.end())
		std::tie(x, y)=iter->second;

	xcb_render_create_cursor(conn()->conn, cursor_id(),
				 cursor_pointer_icon->image->icon_picture
				 ->impl->picture_id(),
				 coord_t::truncate(x),
				 coord_t::truncate(y));
}

cursor_pointerObj::~cursor_pointerObj()
{
	xcb_free_cursor(conn()->conn, cursor_id());
}

cursor_pointer cursor_pointerObj::initialize(IN_THREAD_ONLY)
{
	return cursor_pointer_icon->image->impl->screenref->impl
		->cursor_pointercaches
		->create_cursor_pointer(cursor_pointer_icon
					->initialize(IN_THREAD));
}

icon cursor_pointerObj::theme_updated(IN_THREAD_ONLY,
				      const defaulttheme &new_theme)
{
	return cursor_pointer_icon->image->impl->screenref->impl
		->cursor_pointercaches
		->create_cursor_pointer(theme_updated(IN_THREAD, new_theme));
}

cursor_pointer iconObj::create_cursor()
{
	return image->impl->screenref->impl->cursor_pointercaches
		->create_cursor_pointer(icon(this));
}

LIBCXXW_NAMESPACE_END
