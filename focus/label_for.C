/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/label_for.H"
#include "focus/focusable.H"
#include "x/w/impl/element.H"

LIBCXXW_NAMESPACE_START

label_forObj::label_forObj(const ref<elementObj::implObj> &label,
			   const focusable_impl &focusable)
	: label_thread_only(label), focusable_thread_only(focusable)
{
}

label_forObj::~label_forObj()=default;

void label_forObj::do_with_link(ONLY IN_THREAD,
				const function<with_link_t> &callback)
{
	auto l=label(IN_THREAD).getptr();
	auto f=focusable(IN_THREAD).getptr();

	if (l && f)
		callback(l, f);
}

LIBCXXW_NAMESPACE_END
