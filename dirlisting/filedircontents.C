/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dirlisting/filedircontents.H"
#include "dirlisting/filedircontents_impl.H"
#include <x/threadmsgdispatcher.H>

LIBCXXW_NAMESPACE_START

filedircontentsObj::filedircontentsObj(const std::string &directory,
				       const filedir_callback_t &callback)
	: impl(ref<implObj>::create(directory, callback))
{
	start_threadmsgdispatcher(impl);
}

filedircontentsObj::~filedircontentsObj()
{
	impl->stop();
}

std::string filedircontentsObj::directory() const
{
	return impl->directory;
}

LIBCXXW_NAMESPACE_END
