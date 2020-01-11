/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_thread_debug_h
#define connection_thread_debug_h

#include "x/w/namespace.H"
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

// #define CONNECTION_THREAD_DEBUG 1

#if CONNECTION_THREAD_DEBUG

void connection_thread_action(const char *) LIBCXX_HIDDEN;
void connection_thread_action_for(const char *, void *p) LIBCXX_HIDDEN;
void connection_thread_poll_start(xcb_connection_t *c) LIBCXX_HIDDEN;
void connection_thread_poll_end() LIBCXX_HIDDEN;

#define CONNECTION_THREAD_ACTION(what) connection_thread_action((what))
#define CONNECTION_THREAD_ACTION_FOR(what,ptr)\
	connection_thread_action_for((what),(ptr))
#define CONNECTION_THREAD_POLL_START(c) connection_thread_poll_start(c)
#define CONNECTION_THREAD_POLL_END() connection_thread_poll_end()

#else

#define CONNECTION_THREAD_ACTION(what) do{}while(0)
#define CONNECTION_THREAD_ACTION_FOR(what,ptr) do{}while(0)
#define CONNECTION_THREAD_POLL_START(c) do{}while(0)
#define CONNECTION_THREAD_POLL_END() do{}while(0)

#endif

LIBCXXW_NAMESPACE_END

#endif
