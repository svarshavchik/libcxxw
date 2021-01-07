/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef builtin_atoms_h
#define builtin_atoms_h

#include <xcb/xcb.h>
#include <x/namespace.h>

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

	xcb_atom_t XdndAware;
	xcb_atom_t XdndProxy;
	xcb_atom_t XdndEnter;
	xcb_atom_t XdndLeave;
	xcb_atom_t XdndPosition;
	xcb_atom_t XdndStatus;
	xcb_atom_t XdndFinished;
	xcb_atom_t XdndDrop;
	xcb_atom_t XdndActionAsk;
	xcb_atom_t XdndActionCopy;
	xcb_atom_t XdndActionLink;
	xcb_atom_t XdndActionList;
	xcb_atom_t XdndActionMove;
	xcb_atom_t XdndActionPrivate;
	xcb_atom_t XdndSelection;
	xcb_atom_t XdndTypeList;
	xcb_atom_t text_plain_mime;
	xcb_atom_t text_plain_utf8_mime;
	xcb_atom_t text_plain_iso8859_mime;
	xcb_atom_t text_uri_list_mime;

	builtin_atoms(xcb_connection_t *conn);
};

LIBCXXW_NAMESPACE_END

#endif
