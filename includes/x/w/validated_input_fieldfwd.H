/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_validated_input_fieldfwd_h
#define x_w_validated_input_fieldfwd_h

#include <x/w/namespace.H>
#include <x/w/validated_input_field_contentsfwd.H>
#include <x/w/input_fieldfwd.H>
#include <x/ptrfwd.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

template<typename type>
class validated_input_fieldObj;

//! Determine the type of the first parameter to an \ref input_field "input_field validator".

template<typename validator_functor, typename=void>
class input_field_detect_validator_functor_parameter;

//! An input field validation function

//! The validation function receives the following parameters:
//!
//! - It is invoked \ref ONLY "IN_THREAD".
//!
//! - Either a std::string or a std::u32string, the validation function can
//! declare either one as a parameter
//!
//! - A reference to an \ref input_lock "input field lock", the input field
//! remains locked for the duration of the validation call.
//!
//! - A reference to a \ref callback_trigger_t "trigger" providing the UI
//! context for the validation function's invocation.
//!
//! The validation function returns true or false, indicating whether the
//! input field's contents are valid, or not.

template<typename functor>
concept input_field_validation_function =
	sizeof(input_field_detect_validator_functor_parameter<functor>) > 0;

//! Helper for the input_field_string_validation_function concept
template<typename T,
	 typename functor,
	 typename=std::enable_if_t<
		 std::is_same_v<
			 decltype(std::declval<functor &&>()
				  (std::declval<THREAD_CALLBACK>(),
				   std::declval<const std::string &>(),
				   std::declval<std::optional<T> &>(),
				   std::declval<input_lock &>(),
				   std::declval<const callback_trigger_t &>()
				  )),
			 void>
		 >
	 > struct detect_input_field_string_validation_function {
	int n;
};

//! An input field validation function based on a parsed string.

//! A validation function that gets called if the input field's contents
//! are succesfully extracted via the >> formatted extraction operator into
//! a type.
//!
//! The validation function receives the following parameters:
//!
//! - It is invoked \ref ONLY "IN_THREAD".
//!
//! - The contents of the field before valiation
//!
//! - A possibly-NULL modifiable plain pointer to the validated type. A NULL
//! value indicates either an empty field, or the >> formatted extraction
//! operator failed, or it did not consumed all non-whitespace input. The
//! specific situation can be determined by checking whether the string is
//! empty or not.
//!
//! - A reference to an \ref input_lock "input field lock", the input field
//! remains locked for the duration of the validation call.
//!
//! - A reference to a \ref callback_trigger_t "trigger" providing the UI
//! context for the validation function's invocation.
//!
//! The validation function must return a std::optional containing the
//! validated type.
//!
//! - a nullopt indicates that the validation has failed.
//!
//! - a value indicates that the validation succeeded.

template<typename functor, typename T>
concept input_field_string_validation_function = requires {
	{ sizeof(detect_input_field_string_validation_function<T,
		 functor>) > 0 };
};

/*! Most recently validated value in the \ref input_field.

\code
#include <INSERT_LIBX_NAMESPACE/w/validated_input_field.H>

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

An
\ref validated_input_field "INSERT_LIBX_NAMESPACE::w::validated_input_field<T>"
is one of the values that gets returned from a
 \ref factory "widget factory's" create_input_field() overload that takes
the return value from create_validated_input_field_contents(), or
create_string_validated_input_field_contents() as the first parameter to
create_input_field(). It also gets returned by an existing
 \ref input_field "input field"'s
set_validator() and set_string_validator().

\code
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

\note
The \c validated_input_field object owns a reference on the underlying
\ref input_field "INSERT_LIBX_NAMESPACE::w::input_field" widget.

\code
INSERT_LIBX_NAMESPACE::w::validated_input_field_contents<int> contents=validated_int_value->contents;
\endcode

<tt>validated_input_field</tt>'s \c contents member is a reference to a
\ref validated_input_field_contents "INSERT_LIBX_NAMESPACE::w::validated_input_field_contents<T>"
object that implements the same value(), value_or(), and access() method,
but only an IN_THREAD version of set(). The contents object does not own
a reference to the underlying input field, but the input field owns a reference
on it.

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

template<typename type, typename canonical_formatter>
class LIBCXX_PUBLIC validated_input_field_implObj;


/*! Determine the validation function's type */

template<input_field_validation_function validation_function>
using input_field_validation_function_type=
	typename input_field_detect_validator_functor_parameter<
	validation_function
	>::optional_return_type_t;

template<typename T> struct default_new_value_function {
	inline void operator()(ONLY IN_THREAD, const T &) const
	{
	}
};

/*! Defines create_validated_input_field_contents() return type

Helper template that defines what
\ref create_validated_input_field_contents "create_validated_input_field_contents()"
returns.

*/

template<input_field_validation_function validation_function>
using create_validator_ret_t=std::tuple<
	input_field_validation_callback,
	validated_input_field_contents<
		input_field_validation_function_type<validation_function>
		>
	>;


/*! Defines create_string_validated_input_field_contents() return type

Helper template that defines what
\ref create_string_validated_input_field_contents "create_validated_stromg+input_field_contents()"
returns.

*/

template<typename T>
using create_string_validator_ret_t=std::tuple<
	input_field_validation_callback,
	validated_input_field_contents<T>
	>;


template<input_field_validation_function validation_function,
	 typename canonical_format_functor,
	 typename new_value_functor=default_new_value_function<
		 input_field_validation_function_type<validation_function>
		 >
	 >
auto create_validated_input_field_contents(
	validation_function &&validator,
	canonical_format_functor &&formatter,
	const std::optional<input_field_validation_function_type<
	validation_function>> &initial_value=std::nullopt,
	new_value_functor &&new_value=new_value_functor{}
) -> create_validator_ret_t<validation_function>;

template<typename T,
	 input_field_string_validation_function<T> validation_function,
	 typename canonical_format_functor,
	 typename new_value_functor=default_new_value_function<T>
	 >
auto create_string_validated_input_field_contents(
	validation_function &&validator,
	canonical_format_functor &&canonical_format,
	const std::optional<T> &initial_value=std::nullopt,
	new_value_functor &&new_value=new_value_functor{}
) -> create_string_validator_ret_t<T>;

LIBCXXW_NAMESPACE_END

#endif
