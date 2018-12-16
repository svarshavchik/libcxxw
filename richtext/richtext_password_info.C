/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_password_info.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/input_field_config.H"

LIBCXXW_NAMESPACE_START

richtext_password_info::richtext_password_info(const input_field_config &conf)
	: password_char{conf.password_char}
{
}

richtext_password_info::~richtext_password_info()=default;

LIBCXXW_NAMESPACE_END
