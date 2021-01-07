/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/booklayoutmanager.H"
#include "x/w/book_appearance.H"
#include "x/w/impl/container.H"
#include "busy.H"
#include "catch_exceptions.H"
#include "booklayoutmanager/bookgridlayoutmanager.H"
#include "booklayoutmanager/booklayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

bookgridlayoutmanagerObj
::bookgridlayoutmanagerObj(const container_impl &container,
			   const const_book_appearance &appearance)
	: gridlayoutmanagerObj::implObj{container, {}},
	  appearance{appearance}
{
}

bookgridlayoutmanagerObj::~bookgridlayoutmanagerObj()=default;

layoutmanager bookgridlayoutmanagerObj::create_public_object()
{
	// The public object's impl object is the intermediate
	// booklayoutmanagerObj::implObj object, so we create it first.

	auto impl=ref<booklayoutmanagerObj::implObj>::create(ref(this));

	return booklayoutmanager::create(impl);
}

void bookgridlayoutmanagerObj
::invoke_callback(ONLY IN_THREAD, book_lock &lock,
		  size_t n,
		  const callback_trigger_t &trigger)
{
	auto &cb=callback(lock);

	if (!cb)
		return;

	auto &e=layout_container_impl->container_element_impl();

	try {
		cb(IN_THREAD, book_status_info_t{lock, n, trigger,
							 busy_impl{e}});
	} REPORT_EXCEPTIONS(&e);
}

LIBCXXW_NAMESPACE_END
