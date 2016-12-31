/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "connection_thread.H"
#include "window_handler.H"
#include "xid_t.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::connection_threadObj);

LIBCXXW_NAMESPACE_START

connection_threadObj
::connection_threadObj(const connection_info &info): info(info)
{
}

connection_threadObj::~connection_threadObj() noexcept=default;

void connection_threadObj::run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin)
{
	msgqueue_auto msgqueue(this);

	threadmsgdispatcher_mcguffin=nullptr;

	LOG_DEBUG("Connection thread started");

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

	bool keepgoing=true;

	do
	{
		try {
			run_something(msgqueue, pfd, n_poll);
		} catch (const stopexception &e)
		{
			LOG_DEBUG("Connection thread shutdown");
			keepgoing=false;
		} catch (const exception &e)
		{
			e->caught();
		}
	} while (keepgoing);
}

void connection_threadObj::report_error(const xcb_generic_error_t *e)
{
	LOG_ERROR(connectionObj::implObj::get_error(e));
}

LIBCXXW_NAMESPACE_END
