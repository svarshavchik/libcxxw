/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_positions_confighandlefwd_h
#define x_w_screen_positions_confighandlefwd_h

#include <x/w/screen_positionsobj.H>
#include <x/w/screen_positionsfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

/*! A handle for a saved widget configuration.

A screen_positions_handle gets constructed when creating a widget that
preserves its configuration as XML. Upon construction, the widget's
configuration, identified by its type, name, and the main window it's created
in, gets used to compute its XPath in the configuration file.

newconfig() returns a writelock anchored at an XML element without any
contents. Any existing, previously-saved configuration, gets removed. This
is called from save(), to save the widget's configuration.

config() returns a readlock for reading the existing configuration. If there
is none the returned readlock points to an XML element without any content.

*/

typedef ref<screen_positionsObj::config_handleObj> screen_positions_handle;

//! Constant \ref screen_positions_handle "screen_positions_handle".

//! \see screen_positions_handle

typedef const_ref<screen_positionsObj::config_handleObj
		  > const_screen_positions_handle;

//! A nullable pointer reference to \ref screen_positions_handle "screen_positions_handle".

//! \see screen_positions_handle

typedef ptr<screen_positionsObj::config_handleObj
	    > screen_positions_handleptr;

//! A nullable pointer reference to const \ref screen_positions_handle "screen_positions_handle".

//! \see screen_positions_handle

typedef const_ptr<screen_positionsObj::config_handleObj
		  > const_screen_positions_handleptr;

LIBCXXW_NAMESPACE_END

#endif
