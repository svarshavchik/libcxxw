/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_validated_input_field_contentsfwd_h
#define x_w_validated_input_field_contentsfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

template<typename type> class validated_input_field_contentsObj;
template<typename type>	class validated_input_fieldObj;

/*! Current contents of a validated input field

\code
INSERT_LIBX_NAMESPACE::validated_input_field_contents<int> value;

std::optional<int> last_value=value->value();

int last_value=value->value_or(0);

value->access([]
              (const std::optional<int> &value)
              {
              });

\endcode

value() returns the most recently entered value.
Generally, no value gets returned before the first value gets entered into
the input field, or if the entered contents could not be validated.

value_or() returns the most recently entered value, or its parameter if
no value was entered. access() locks the value and passes it to the
callable object given as access()'s parameter, releasing the lock after
it returns.

\code
value->set(IN_THREAD,
           lock,
           3);
\endcode

set() is a shortcut for updating the value of the validated input field.
It is used by the installed input field validation callback, and is also
available for on_spin() callbacks as means of updating the spinned value.
These callbacks receive an \ref input_lock "input field lock" object that
gets passed in as set()'s 2nd parameter. The third parameter is the new
\c std::optional value of this input field, with the \c std::nullopt value
indicating an unset/unvalidated value.

An option fourth parameter is a \c bool value, defaulting to \c true which
specifies that the passed in \c std::nullopt value should result in the
input field getting cleared. In all cases the passed in value, \c std::nullopt
or otherwise, sets the new value of the validated input field. Passing in
a \c false flag results in whatever actual text is in an input field not
getting cleared by a \c std::nullopt.

\code
INSERT_LIBX_NAMESPACE::w::validated_input_field_contents<int> contents=value->contents;
\endcode

A \c validated_input_field objects owns a reference to the underlying field.
The input field also owns a reference to the \c contents member, a
\c validated_input_field_contents object that holds the actual validated
value and implements the value(), value_or(), access() and the
IN_THREAD set() overload. The methods in the
\c validated_input_field object are just proxies for the real one, and
only the \c validated_input_field implements the non-IN_THREAD set()
overload.


*/
template<typename type>
using validated_input_field_contents=
	ref<validated_input_field_contentsObj<type>>;

//! A constant \ref validated_input_field_contents "validated input field contents".

//! \see validated_input_field_contents

template<typename type>
using const_validated_input_field_contents=
	const_ref<validated_input_field_contentsObj<type>>;

//! A nullable pointer reference to a \ref validated_input_field_contents "validated input field contents".

//! \see validated_input_field_contents

template<typename type>
using validated_input_field_contentsptr=
	ptr<validated_input_field_contentsObj<type>>;

//! A nullable pointer reference to a const \ref validated_input_field_contents "validated input field contents".

//! \see validated_input_field_contents

template<typename type>
using const_validated_input_field_contentsptr=
	const_ptr<validated_input_field_contentsObj<type>>;

LIBCXXW_NAMESPACE_END

#endif
