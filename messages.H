/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef messages_H
#define messages_H

#include "x/w/namespace.H"
#include <x/messages.H>
#include <courier-unicode.h>

#define cxxwlibmsg(x) dgettext("libcxxw", x)
#define _(x) cxxwlibmsg(x)
#define _txt(x) x

LIBCXXW_NAMESPACE_START

//! Strip prefix from localized strings.

//! Localized string may be of the form "TAG:MEAT". This returns the meat.
//!
//! This provides different tags with the same meat, and separate localization
//! for each tag.

const char *TAG(const char *) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
