/*
** Copyright 2017-2021 Double Precision, Inc.
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

//! Unicode bi-directional formatting to apply to retrieved text

//! Whether to include directional embedding markers in text returned by
//! \ref input_lock "input_lock"'s get() and get_unicode() methods.

enum class bidi_format {

	//! Do not include embedded directional markers.

	//! This mostly means that text that was forcibly entered in the
	//! opposite direction will not have embedded directional markers.
	//! set()ing the resulting text may not necessarily result in the
	//! text being rendered in the same order as it was entered
	standard,

	//! Include embedded directional and isolation markers

	//! Embedded directional and isolation markers get included in the
	//! returned text, so that it's accurately restored if it gets set()
	//! in the same input field again. The logic for calculating directional
	//! markers is not exact. Some extra and unneeded directional markers
	//! may be included as a result, depending on the input field's
	//! specified \ref bidi "bi-directional text direction".
	//!
	//! Applications must be fully aware
	//! and be able to process all Unicode bi-directional markers.

	embedded
};

//! Callback used by an \ref input_field "input field" to search for values.

typedef void input_field_search_callback_t(const input_field_search_info &);

LIBCXXW_NAMESPACE_END

#endif
