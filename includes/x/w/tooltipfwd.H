/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltipfwd_h
#define x_w_tooltipfwd_h

#include <x/w/labelfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN tooltip_factory;

enum class attached_to;

struct LIBCXX_PUBLIC static_tooltip_config;

//! Create a default tooltip factory consisting of a label.

//! A \ref element "widget"'s create_tooltip() is equivalent to calling
//! create_label_tooltip() and pasing the resulting callback to
//! create_custom_tooltip().

functionref<void (THREAD_CALLBACK,
		  const tooltip_factory &)>
create_label_tooltip(const text_param &, const label_config &);

LIBCXXW_NAMESPACE_END

#endif
