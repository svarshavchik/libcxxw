/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connectionfwd.H"
#include "builtin_atoms.H"
#include "returned_pointer.H"

#include <vector>
#include <sstream>
#include <cstring>
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

builtin_atoms::builtin_atoms(xcb_connection_t *conn)
{
	// Load intern atoms we will use

	static const struct {
		const char *atom_name;
		xcb_atom_t builtin_atoms::*atom_value;
	} atoms[] = {
		{"WM_PROTOCOLS", &builtin_atoms::wm_protocols},
		{"WM_DELETE_WINDOW", &builtin_atoms::wm_delete_window},
		{"WM_NORMAL_HINTS", &builtin_atoms::wm_normal_hints},
		{"STRING", &builtin_atoms::string},
		{"UTF8_STRING", &builtin_atoms::utf8_string},
		{"CXXWTHEME", &builtin_atoms::cxxwtheme},
		{"CXXWPASTE", &builtin_atoms::cxxwpaste},
		{"MULTIPLE", &builtin_atoms::multiple},
		{"TARGETS", &builtin_atoms::targets},
		{"INCR", &builtin_atoms::incr},

		{"_NET_FRAME_EXTENTS", &builtin_atoms::net_frame_extents},
		{"_XIM_XCONNECT", &builtin_atoms::xim_xconnect},
		{"_XIM_PROTOCOL", &builtin_atoms::xim_protocol},
		{"_XIM_MOREDATA", &builtin_atoms::xim_moredata},
		{"_clientXXX", &builtin_atoms::xim_clientXXX},

		{"XdndAware", &builtin_atoms::XdndAware},
		{"XdndProxy", &builtin_atoms::XdndProxy},
		{"XdndEnter", &builtin_atoms::XdndEnter},
		{"XdndLeave", &builtin_atoms::XdndLeave},
		{"XdndPosition", &builtin_atoms::XdndPosition},
		{"XdndStatus", &builtin_atoms::XdndStatus},
		{"XdndFinished", &builtin_atoms::XdndFinished},
		{"XdndDrop", &builtin_atoms::XdndDrop},
		{"XdndActionAsk", &builtin_atoms::XdndActionAsk},
		{"XdndActionCopy", &builtin_atoms::XdndActionCopy},
		{"XdndActionLink", &builtin_atoms::XdndActionLink},
		{"XdndActionList", &builtin_atoms::XdndActionList},
		{"XdndActionMove", &builtin_atoms::XdndActionMove},
		{"XdndActionPrivate", &builtin_atoms::XdndActionPrivate},
		{"XdndSelection", &builtin_atoms::XdndSelection},
		{"XdndTypeList", &builtin_atoms::XdndTypeList},
		{"text/plain", &builtin_atoms::text_plain_mime},
		{"text/plain;charset=utf-8", &builtin_atoms::text_plain_utf8_mime},
		{"text/plain;charset=iso-8859-1", &builtin_atoms::text_plain_iso8859_mime},
		{"text/uri-list", &builtin_atoms::text_uri_list_mime},
	};

	std::vector<xcb_intern_atom_cookie_t> cookies;

	cookies.reserve(sizeof(atoms)/sizeof(atoms[0]));

	for (const auto &atom:atoms)
		cookies.push_back(xcb_intern_atom(conn, 0,
						  strlen(atom.atom_name),
						  atom.atom_name));

	std::ostringstream errors;
	const char *error_sep="";

	for (size_t i=0; i<sizeof(atoms)/sizeof(atoms[0]); ++i)
	{
		returned_pointer<xcb_generic_error_t *> error;

		auto value=return_pointer(xcb_intern_atom_reply
					  (conn,
					   cookies[i],
					   error.addressof()));

		if (error)
		{
			errors << atoms[i].atom_name << ": " << error_sep
			       << connection_error(error);
			error_sep="; ";
		}
		else
		{
			(this->*atoms[i].atom_value)=value ? value->atom:0;
		}
	}

	if (*error_sep)
		throw EXCEPTION(errors.str());
}

LIBCXXW_NAMESPACE_END
