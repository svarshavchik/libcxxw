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
//! \ref input_lock "input_lock"'s get() and get_unicode() methods, by default.

enum class bidi_format {

	//! Do not include embedded directional markers.

	//! This mostly means that text that was forcibly entered in the
	//! opposite direction will not have embedded directional markers.
	//! set()ing the resulting text may not necessarily result in the
	//! text being rendered in the same order as it was entered.
	//!
	//! \note
	//! Input fields do not preserve any directional override or isolate
	//! markers. They get removed from the initial contents of the input
	//! field, or from any explicitly set() contents, and the standard
	//! format retrieves the entered text without any directional override
	//! and isolate markers.

	none,

	//! Include embedded directional override and isolation markers

	//! Embedded directional override and isolation markers get included
	//! in the
	//! returned text, so that it's accurately restored if it gets set()
	//! in the same input field again. The logic for calculating directional
	//! markers is not exact. Some extra and unneeded directional markers
	//! may be included as a result, depending on the input field's
	//! specified \ref bidi "bi-directional text direction", and won't
	//! necessarily match the override and isolation markers from the
	//! original contents of the input field.
	//!
	//! Applications must be fully aware
	//! and be able to process all Unicode bi-directional markers.

	embedded,

	//! Include embeded directional markers heuristically

	//! Determine heuristically whether embedded directional
	//! or isolation markers are required.

	automatic
};

//! For internal use.

extern const char bidi_format_names[3][16];

//! Callback used by an \ref input_field "input field" to search for values.

typedef void input_field_search_callback_t(const input_field_search_info &);

LIBCXXW_NAMESPACE_END

#endif
