/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef shortcut_activation_element_h
#define shortcut_activation_element_h

#include "shortcut/shortcut_activation_elementfwd.H"
#include "element.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
shortcut_activation_elementObj<element_impl>::~shortcut_activation_elementObj()
{
	uninstall_shortcut();
}

template<typename element_impl>
generic_windowObj::handlerObj &shortcut_activation_elementObj<element_impl>
::shortcut_window_handler()
{
	elementObj::implObj *e=this;
	return e->get_window_handler();
}

LIBCXXW_NAMESPACE_END

#endif
