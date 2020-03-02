/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef gc_thread_H
#define gc_thread_H

#include "gc.H"
#include "drawable.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

inline connection_thread gcObj::handlerObj::thread() const
{
	return this->thread_;
}

inline xcb_connection_t *gcObj::handlerObj::conn() const
{
	return thread()->info->conn;
}

LIBCXXW_NAMESPACE_END
#endif
