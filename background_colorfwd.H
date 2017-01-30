/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_colorfwd_H
#define background_colorfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class background_colorObj;
class background_colorBase;

//! A background color in the current theme.

//! This object produces, on demand, a background color.

typedef ref<background_colorObj, background_colorBase> background_color;

#define DEFAULT_MAINWINDOW_BACKGROUND_COLOR rgb(0xCCCC, 0xCCCC, 0xCCCC)

LIBCXXW_NAMESPACE_END
#endif
