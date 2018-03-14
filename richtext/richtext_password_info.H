/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_password_info_H
#define richtext_password_info_H

#include "x/w/namespace.H"
#include <string>
#include "richtext/richtextstring.H"
#include "x/w/input_field_configfwd.H"

LIBCXXW_NAMESPACE_START

//! Additional information for password input fields.

//! Superclass of \ref editorObj::implobj "editorObj::implObj". When
//! password_char is non-zero, the real contents of the input field
//! are maintained in the \c real_string.

struct LIBCXX_HIDDEN richtext_password_info {

	//! Whether this input field is a password field, if this is non-zero.

	char32_t password_char;

	//! Real field contents.
	richtextstring real_string;

	richtext_password_info(const input_field_config &);
	~richtext_password_info();
};

LIBCXXW_NAMESPACE_END;
#endif
