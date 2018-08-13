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
	xcb_atom_t wm_normal_hints;

	xcb_atom_t string;
	xcb_atom_t utf8_string;
	xcb_atom_t cxxwtheme;
	xcb_atom_t cxxwpaste;
	xcb_atom_t multiple;
	xcb_atom_t targets;
	xcb_atom_t incr;

	xcb_atom_t net_frame_extents;
	xcb_atom_t xim_xconnect;
	xcb_atom_t xim_protocol;
	xcb_atom_t xim_moredata;
	xcb_atom_t xim_clientXXX;

	builtin_atoms(xcb_connection_t *conn);
};

LIBCXXW_NAMESPACE_END

#endif
