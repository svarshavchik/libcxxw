/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_hotspot_bgcolorfwd_h
#define x_w_hotspot_bgcolorfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC hotspot_bgcolorObj;

//! A hotspot that changes colors.

//! A subclass of a \ref hotspot "hotspot" which changes background color
//! when activated or used.

typedef ref<hotspot_bgcolorObj> hotspot_bgcolor;

//! A nullable pointer reference to a hotspot with colors.

//! \see hotspot_bgcolor
typedef ptr<hotspot_bgcolorObj> hotspot_bgcolorptr;

//! A reference to a constant hotspot with colors.

//! \see hotspot_bgcolor

typedef const_ref<hotspot_bgcolorObj> const_hotspot_bgcolor;

//! A nullable pointer reference to a constant hotspot with colors.

//! \see hotspot_bgcolor

typedef const_ptr<hotspot_bgcolorObj> const_hotspot_bgcolorptr;

LIBCXXW_NAMESPACE_END

#endif
