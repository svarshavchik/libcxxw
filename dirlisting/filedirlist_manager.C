/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "filedirlist_manager_impl.H"

LIBCXXW_NAMESPACE_START

filedirlist_managerObj::filedirlist_managerObj(const factory &f,
					       const std::string
					       &initial_directory,
					       const std::function
					       <filedirlist_selected_callback_t>
					       &callback)
	: impl(ref<implObj>::create(f, initial_directory, callback))
{
}

filedirlist_managerObj::~filedirlist_managerObj()=default;

filedirlist_entry filedirlist_managerObj::at(size_t n)
{
	return impl->at(n);
}

void filedirlist_managerObj::chdir(const std::string &directory)
{
	impl->chdir(directory);
}

LIBCXXW_NAMESPACE_END
