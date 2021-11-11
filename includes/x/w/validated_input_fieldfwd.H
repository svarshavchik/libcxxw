/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_validated_input_fieldfwd_h
#define x_w_validated_input_fieldfwd_h

#include <x/w/namespace.H>
#include <x/w/validated_input_field_contentsfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

template<typename type>
class validated_input_fieldObj;

/*! Most recently validated value in the \ref input_field.

\code
INSERT_LIBX_NAMESPACE::w::validated_input_field<int> validated_int_value=
        input_field->set_string_validator(
             []
		 (ONLY IN_THREAD,
              const std::string &value,
		  int *parsed_value,
		  INSERT_LIBX_NAMESPACE::w::input_lock &lock,
		  const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger) -> std::optional<int>
             {
                 // ...
             },
             []
             (int n) { return std::to_string(n) });
\endcode

\ref validated_input_field "INSERT_LIBX_NAMESPACE::w::validated_input_field<T>"
gets returned by \ref input_field "input field"'s
set_validator() and set_string_validator().

\code
#include &lt;INSERT_LIBX_NAMESPACE/w/validated_input_field_contents&gt;

INSERT_LIBX_NAMESPACE::w::validated_input_field_contents<int> validated_int_contents=validated_int_value->contents;
\endcode

\ref validated_input_field "INSERT_LIBX_NAMESPACE::w::validated_input_field"'s
\c contents member is a
\ref validated_input_field_contents "INSERT_LIBX_NAMESPACE::w::validated_input_field_contents"
object. The installed \c INSERT_LIBX_NAMESPACE::w::input_field callback
owns a reference to the \c validated_input_field_contents, so does the
returned
\ref validated_input_field "INSERT_LIBX_NAMESPACE::w::validated_input_field",
which also owns a reference to the \c INSERT_LIBX_NAMESPACE::w::input_field.

The following methods get implemented in a
\ref validated_input_field "INSERT_LIBX_NAMESPACE::w::validated_input_field<T>"
by forwarding them to the
\ref validated_input_field_contents "INSERT_LIBX_NAMESPACE::w::validated_input_field_contents":

- \c value()

- \c value_or()

- \c access()

\code
validated_int_value->set(5);

validated_int_value->set(std::nullopt);

validated_int_value->set(IN_THREAD, 5);
\endcode

set() reinitializes the input field with a new value. \c std::nullopt specifies
to value set, the input field gets cleared.

\note
The non-IN_THREAD overload does not immediately update the value(), but
forwards a message to the connection thread, which updates the value and
the input field widget.

*/

template<typename type>
using validated_input_field=ref<validated_input_fieldObj<type>>;

//! A constant \ref validated_input_field "validated input field value".

//! \see validated_input_field

template<typename type>
using const_validated_input_field=const_ref<validated_input_fieldObj<type>>;

//! A nullable pointer reference to a \ref validated_input_field "validated input field value".

//! \see validated_input_field

template<typename type>
using validated_input_fieldptr=ptr<validated_input_fieldObj<type>>;

//! A nullable pointer reference to a const \ref validated_input_field "validated input field value".

//! \see validated_input_field

template<typename type>
using const_validated_input_fieldptr=const_ptr<validated_input_fieldObj<type>>;

LIBCXXW_NAMESPACE_END

#endif
