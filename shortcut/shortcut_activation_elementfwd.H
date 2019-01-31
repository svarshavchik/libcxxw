/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef shortcut_activation_elementfwd_h
#define shortcut_activation_elementfwd_h

#include "shortcut/shortcut_activation_element_impl.H"

LIBCXXW_NAMESPACE_START

//! Combine shortcut_activation_element_implObj mixin with an element.

//! The template parameter is a subclass of elementObj::implObj. The
//! template combines the mixin, and implements shortcut_window_handler().

template<typename element_impl>
class LIBCXX_HIDDEN shortcut_activation_elementObj
	: public element_impl,
	  public shortcut_activation_element_implObj {

public:

	using element_impl::element_impl;

	//! Destructor
	~shortcut_activation_elementObj();

	//! Implement shortcut_window_handler().
	generic_windowObj::handlerObj &shortcut_window_handler() override;
};

LIBCXXW_NAMESPACE_END

#endif
