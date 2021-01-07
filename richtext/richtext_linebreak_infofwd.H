/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_linebreak_infofwd_H
#define richtext_linebreak_infofwd_H

#include "x/w/namespace.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN richtext_linebreak_calculate;

//! Type-safe unicode linebreaking values

enum class LIBCXX_HIDDEN unicode_lb : signed char {
     mandatory=UNICODE_LB_MANDATORY,
     none=UNICODE_LB_NONE,
     allowed=UNICODE_LB_ALLOWED
};

LIBCXXW_NAMESPACE_END;
#endif
