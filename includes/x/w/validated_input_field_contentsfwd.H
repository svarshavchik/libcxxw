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

value->set(IN_THREAD, 5);

value->set(IN_THREAD, std::nullopt);
\endcode

value() returns the most recently entered value.
Generally, no value gets returned before the first value gets entered into
the input field, or if the entered contents could not be validated.

value_or() returns the most recently entered value, or its parameter if
no value was entered. access() locks the value and passes it to the
callable object given as access()'s parameter, releasing the lock after
it returns.

The new value is also a \c std::optional, with a std::nullopt indicated
an unvalidated (non-entered) initial value for the field.

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
