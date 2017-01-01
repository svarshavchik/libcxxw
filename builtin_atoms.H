/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef builtin_atoms_h
#define builtin_atoms_h

#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

//! Built-in atoms.

class LIBCXX_HIDDEN builtin_atoms {

 public:

	xcb_atom_t wm_protocols;
	xcb_atom_t wm_delete_window;

	builtin_atoms(xcb_connection_t *conn);
};

LIBCXXW_NAMESPACE_END

#endif
