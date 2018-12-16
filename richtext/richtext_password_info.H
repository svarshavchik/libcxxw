/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_password_info_H
#define richtext_password_info_H

#include "x/w/namespace.H"
#include <string>
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/input_field_configfwd.H"

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN richtext_password_info;

//! Additional information for password input fields.

//! Superclass of \ref editorObj::implobj "editorObj::implObj". When
//! password_char is non-zero, the real contents of the input field
//! are maintained in the \c real_string.

struct richtext_password_info {

	//! Whether this input field is a password field, if this is non-zero.

	const char32_t password_char;

	//! Real field contents.
	mpobj<richtextstring> real_string;

	richtext_password_info(const input_field_config &);
	~richtext_password_info();
};

LIBCXXW_NAMESPACE_END;
#endif
