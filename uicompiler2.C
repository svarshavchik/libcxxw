/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "x/w/uigenerators.H"
#include "x/w/all_appearances.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

static void wrong_appearance_type2(const std::string &name)
	__attribute__((noreturn));

static void wrong_appearance_type2(const std::string &name)
{
	throw EXCEPTION(gettextmsg(_("<appearance id=\"%1%\"> expected "
				     "to be a different appearance type"),
				   name));
}

template<typename appearance_type>
appearance_type uicompiler::get_compiled_appearance(const std::string &name)
{
	compile_uncompiled_appearance(name); // If necessary

	auto appearance=generators->lookup_appearance(name);

	if (!appearance->isa<appearance_type>())
		wrong_appearance_type2(name);

	return appearance;
}

// Helper class used to pass parameters into generate() without having to
// pull in the entire xml/doc file for everyone who includes uicompiler.H.

struct uicompiler::generate_info {
	const theme_parser_lock &parent;
	const theme_parser_lock &lock;
	xml::doc::base::xpath &xpath;
};

template<typename T> appearance_wrapper<T>::~appearance_wrapper()=default;

template<typename T> appearance_wrapper<T>::appearance_wrapper(const T &arg)
	: T{arg}
{
}

static void duplicate_reset(const std::string &name)
	       __attribute__((noreturn));

static void duplicate_reset(const std::string &name)
{
	throw EXCEPTION(gettextmsg(_("multiple <reset> in <%1%>"), name));
}

static void unknown_appearance_type(const std::string &id,
				    const std::string &type)
	       __attribute__((noreturn));

static void unknown_appearance_type(const std::string &id,
				    const std::string &type)
{
	if (type.empty())
		throw EXCEPTION(gettextmsg(_("<appearance id=\"%1\"> is missing"
					     " a type"), id));
	throw EXCEPTION(gettextmsg(_("unknown <appearance id=\"%1\"> type:"
				     " %2%"), id, type));
}

static void unknown_appearance_node(const char *where,
				    const std::string &name)
	       __attribute__((noreturn));

static void unknown_appearance_node(const char *where,
				    const std::string &name)
{
	throw EXCEPTION(gettextmsg(_("circular or non-existent dependency of "
				     "%1% appearance object \"%2\""),
				   where,
				   name));
}

LIBCXXW_NAMESPACE_END

#include "appearance/appearance_parser.inc.C"
