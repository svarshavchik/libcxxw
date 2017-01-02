/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "connection_thread.H"
#include "window_handler.H"
#include "element.H"
#include "xid_t.H"
#include "catch_exceptions.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::connection_threadObj);

LIBCXXW_NAMESPACE_START

connection_threadObj
::connection_threadObj(const connection_info &info): info(info)
{
}

connection_threadObj::~connection_threadObj()=default;

void connection_threadObj::run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin)
{
	msgqueue_auto msgqueue(this);

	threadmsgdispatcher_mcguffin=nullptr;

	LOG_DEBUG("Connection thread started");

	// Initialize thread-only variables

	std::map<xcb_window_t, ref<window_handlerObj>> window_handlers;
	std::map<uint32_t, ref<xidObj>> destroyed_xids;
	std::set<rectangle> exposed_rectangles;

	element_set_t visibility_updated;

	window_handlers_thread_only= &window_handlers;
	destroyed_xids_thread_only= &destroyed_xids;
	exposed_rectangles_thread_only= &exposed_rectangles;

	visibility_updated_thread_only= &visibility_updated;
	disconnect_callback_thread_only=[] {};

	// Set two file descriptors to poll.
	//
	// 1. The message queue event file descriptor, set it to
	//    non-blocking mode.
	//
	// 2. xcb_connection_t file descriptor.
	//
	// n_poll also gets initialized to 2, indicating 2 file descriptors
	// will be polled. But before we poll, if xcb_connection_has_error(),
	// n_poll gets decremented to 1, and we'll only handle event messages,
	// until doom arrives.

	auto eventfd=msgqueue->getEventfd();
	eventfd->nonblock(true);

	struct pollfd pfd[2];

	pfd[0].fd=eventfd->getFd();
	pfd[0].events=POLLIN | POLLHUP;

	pfd[1].fd=xcb_get_file_descriptor(info->conn);
	pfd[1].events=POLLIN | POLLHUP;

	size_t n_poll=2;

	stop_received=false;
	stopping_politely=false;

	do
	{
		try {
			run_something(msgqueue, pfd, n_poll);
		} CATCH_EXCEPTIONS;
	} while (!stopping_politely);
}

void connection_threadObj::report_error(const xcb_generic_error_t *e)
{
	LOG_ERROR(connectionObj::implObj::get_error(e));
}

LIBCXXW_NAMESPACE_END
