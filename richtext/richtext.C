/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_impl.H"

LIBCXXW_NAMESPACE_START

richtext richtextBase::create(halign alignment, dim_t initial_width)
{
	return ptrrefBase::objfactory<richtext>
		::create(ref<richtextObj::implObj>
			 ::create(alignment), initial_width);
}

richtextObj::richtextObj(const ref<implObj> &impl,
			 dim_t word_wrap_width)
	: impl(impl),
	  word_wrap_width_thread_only(word_wrap_width)
{
}

richtextObj::~richtextObj()=default;

void richtextObj::set(IN_THREAD_ONLY, richtextstring &string)
{
	impl_t::lock lock{impl};

	(*lock)->set(IN_THREAD, string);
}

ref<richtextObj::implObj> richtextObj::debug_get_impl()
{
	impl_t::lock lock{impl};

	return *lock;
}

LIBCXXW_NAMESPACE_END
