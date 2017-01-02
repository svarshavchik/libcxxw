/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element.H"
#include "screen.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

#define THREAD screenref->impl->thread

elementObj::implObj::implObj(const screen &screenref,
			     size_t nesting_level,
			     const rectangle &initial_position)
	: screenref(screenref),
	  data_thread_only
	  ({
		  nesting_level,
		  initial_position,
	  })
{
}

elementObj::implObj::~implObj()=default;

void elementObj::implObj::request_visibility(bool flag)
{
	// Set requested_visibility, make sure this is done in the connection
	// thread.

	// Sets requested_visibility, then adds the element to the
	// visibility_updated list.

	// The connection thread invokes update_visibility after processing
	// all messages.

	THREAD->run_as(RUN_AS,
		       [flag, me=elementimpl(this)]
		       (IN_THREAD_ONLY)
		       {
			       me->data(IN_THREAD).requested_visibility=flag;
			       IN_THREAD->visibility_updated(IN_THREAD)->insert(me);
		       });
}

void elementObj::implObj::update_visibility(IN_THREAD_ONLY)
{
	if (data(IN_THREAD).actual_visibility ==
	    data(IN_THREAD).requested_visibility)
		return;

	visibility_updated(IN_THREAD,
			   (
			    data(IN_THREAD).actual_visibility=
			    data(IN_THREAD).requested_visibility));
}

void elementObj::implObj::visibility_updated(IN_THREAD_ONLY, bool flag)
{
}

LIBCXXW_NAMESPACE_END
