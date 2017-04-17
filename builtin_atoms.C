/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection.H"
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
		{"MULTIPLE", &builtin_atoms::multiple},
		{"TARGETS", &builtin_atoms::targets},
		{"INCR", &builtin_atoms::incr},

		{"_XIM_XCONNECT", &builtin_atoms::xim_xconnect},
		{"_XIM_PROTOCOL", &builtin_atoms::xim_protocol},
		{"_XIM_MOREDATA", &builtin_atoms::xim_moredata},
		{"_clientXXX", &builtin_atoms::xim_clientXXX},
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
			       << connectionObj::implObj::get_error(error);
			error_sep="; ";
		}
		else
		{
			(this->*atoms[i].atom_value)=value->atom;
		}
	}

	if (*error_sep)
		throw EXCEPTION(errors.str());
}

LIBCXXW_NAMESPACE_END
