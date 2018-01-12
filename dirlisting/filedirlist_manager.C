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
					       file_dialog_type type)
	: impl(ref<implObj>::create(f, initial_directory, type))
{
}

filedirlist_managerObj::~filedirlist_managerObj()=default;

void filedirlist_managerObj
::set_selected_callback(const std::function<filedirlist_selected_callback_t> &c)
{
	impl->set_selected_callback(c);
}

filedirlist_entry filedirlist_managerObj::at(const filedirlist_entry_id &id)
{
	return impl->at(id);
}

void filedirlist_managerObj::chdir(const std::string &directory)
{
	impl->chdir(directory);
}

void filedirlist_managerObj::chfilter(const pcre &filter)
{
	impl->chfilter(filter);
}

std::string filedirlist_managerObj::pwd() const
{
	return impl->pwd();
}

LIBCXXW_NAMESPACE_END
