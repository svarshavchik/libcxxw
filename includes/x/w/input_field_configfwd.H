/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_configfwd_h
#define x_w_input_field_configfwd_h

#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_PUBLIC input_field_config_appearance;

struct LIBCXX_PUBLIC input_field_config_settings;

struct LIBCXX_PUBLIC input_field_config;

struct LIBCXX_HIDDEN input_field_search_info;

//! Callback used by an \ref input_field "input field" to search for values.

typedef void input_field_search_callback_t(const input_field_search_info &);

LIBCXXW_NAMESPACE_END

#endif
