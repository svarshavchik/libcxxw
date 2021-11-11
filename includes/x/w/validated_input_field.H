/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_validated_input_field_h
#define x_w_validated_input_field_h

#include <x/w/validated_input_fieldfwd.H>
#include <x/w/validated_input_fieldobj.H>
#include <x/w/validated_input_field_contents.H>
#include <x/w/input_field_lock.H>
#include <x/ref.H>
#include <x/strtok.H>
#include <x/functionalrefptr.H>
#include <utility>

LIBCXXW_NAMESPACE_START
//! Determine the first parameter to an \ref input_field "input field validator".

template<typename ret_type>
class input_field_get_for_validator;

//! Specialization: the input field validator takes a \c std::string parameter.
template<>
class input_field_get_for_validator<std::string> {

 public:

	//! Get the value for the input field validator.
	static inline auto get(const input_lock &l)
	{
		return l.get();
	}
};

//! Specialization: the input field validator takes a \c std::u32string parameter.

template<>
class input_field_get_for_validator<std::u32string> {

 public:

	//! Get the value for the input field validator.
	static inline auto get(const input_lock &l)
	{
		return l.get_unicode();
	}
};

//! Determine the value returned by the input field validator.

template<typename> class input_field_validator_ret_t;

//! The input field validator must return a \c std::optional

//! Determine what the optional type is.

template<typename type>
class input_field_validator_ret_t<std::optional<type>> {
public:

	//! The optional type returned by an input field validation function.

	typedef type optional_return_type_t;
};


//! Specialization: the input field validator takes a \c std::string parameter.

template<typename validator_functor>
class input_field_detect_validator_functor_parameter<
	validator_functor,
	std::void_t<decltype(std::declval<validator_functor &&>()
			     (std::declval<THREAD_CALLBACK>(),
			      std::declval<const std::string &>(),
			      std::declval<input_lock &>(),
			      std::declval<const callback_trigger_t &>()))>>
	: public input_field_get_for_validator<std::string> {

public:
	//! The optional type returned by an input field validation function.

	typedef typename input_field_validator_ret_t<
		decltype(std::declval<validator_functor &&>()
			 (std::declval<THREAD_CALLBACK>(),
			  std::declval<const std::string &>(),
			  std::declval<input_lock &>(),
			  std::declval<const callback_trigger_t &>()))
		>::optional_return_type_t optional_return_type_t;
};

//! Specialization: the input field validator takes a \c std::u32string parameter.
template<typename validator_functor>
class input_field_detect_validator_functor_parameter<
	validator_functor,
	std::void_t<decltype(std::declval<validator_functor &&>()
			     (std::declval<THREAD_CALLBACK>(),
			      std::declval<const std::u32string &>(),
			      std::declval<input_lock &>(),
			      std::declval<const callback_trigger_t &>()))>>
	: public input_field_get_for_validator<std::u32string> {

public:
	//! The optional type returned by an input field validation function.

	typedef typename input_field_validator_ret_t<
		decltype(std::declval<validator_functor &&>()
			 (std::declval<THREAD_CALLBACK>(),
			  std::declval<const std::u32string &>(),
			  std::declval<input_lock &>(),
			  std::declval<const callback_trigger_t &>()))
		>::optional_return_type_t optional_return_type_t;
};

//! Implement the \ref validated_input_field "input field validation" object.

//! \internal

template<typename type, typename canonical_formatter>
class validated_input_field_implObj
	: public validated_input_field_contentsObj<type> {

public:

	//! Captured formatting callable object.
	const canonical_formatter formatter;

	//! What the formatter returns: std::string or std::u32string

	typedef decltype(std::declval<canonical_formatter &>()
			 (std::declval<const type &>())
			 ) formatter_ret_t;

	//! Constructor
	template<typename F>
	validated_input_field_implObj(F &&f)
		: formatter{std::forward<F>(f)}
	{
	}

	//! Destructor
	~validated_input_field_implObj()=default;

	//! Implement set().

	void set(ONLY IN_THREAD,
		 input_lock &lock,
		 const std::optional<type> &v) override
	{
		this->validated_value=v;

		if (!v)
			// Do not set() the input field. No value
			// means it's not valid, and set()ing it
			// validates the contents of the input field.
		{
			lock.set("");
			return;
		}
		formatter_ret_t s=formatter(*v);

		// Do not bother with set() if the value is the same.

		if (s == input_field_get_for_validator<decltype(s)>::get(lock))
			return;

		// Set it, and mark it as validated.

		lock.set(IN_THREAD, s, true);
	}
};

/*! Create a validator for an input field.

create_validated_input_field_contents() returns two values:

1) An \ref input_field_validation_callback "on_validate()" callback, this
callback should get passed to an \ref input_field "input_field"'s
on_validate() callback to install the validator.

2) An \ref validated_input_field_contents "validated_input_field_contents<T>"
validator object. The type gets deduced from the validation_function parameter.
The on_validate() callback also owns a reference to this object.

\code
const auto &[callback, contents] = create_validated_input_field_contents(
         []
         (ONLY IN_THREAD,

          const std::string &value,
          INSERT_LIBX_NAMESPACE::w::input_lock &lock,
          const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger)-> std::optional<int>
         {
             std::istringstream i{value};

             int n;

             if (i >> n)
                  return n;

             lock.stop_message("Numeric input required");

             return std::nullopt;
         },
         []
         (int v) -> std::string
         {
             return std::to_string(v);
         },
         []
         (ONLY IN_THREAD, int v)
         {
             std::cout << "New value: " << *v << std::endl;
         });

\endcode

This adapts an \ref input_field "input_field" on_validate() callback to
implement a consistent input validation
framework for parsing and edit-checking the text input and converting it
to a specific data type.

The first parameter to create_validated_input_field_contents()
is a closure that returns a \c std::optional<T>, where \c T is some
data type that the entered text gets converted to. The \c T type deduces
the type of the returned validated_input_field_contents.

The first closure itself receives the following parameters:

- the closure gets invoked IN_THREAD.

- either a \c std::string or \c std::u32string, the closure receives
the current contents of the input field as either a string in the current
encoding, or a unicode string. Note: leading and trailing whitespace gets
trimmed off.

- a reference to an \ref input_lock "INSERT_LIBX_NAMESPACE::w::input_lock"
on its own field, for convenience.

- the triggering event.

The closure returns a \c T value to indicate the value of the converted
string, or a \c std::nullopt if the string is not valid.

The closure is responsible for reporting invalid input, and the usual
way to do that is to use the \c lock parameter and call the lock's
stop_message(), that displays an error dialog in the input field's
window.

create_validated_input_field_contents()'s second closure takes a
T parameter and returns either a \c std::string in the current locale's
encoding or a unicode string.

The returned string represents the
entered \c T value in canonical form. It may or may not be identical
to what was entered into the text input field, and passed to the first
closure. It's expected that if the
returned value gets passed to the first closure, the first closure returns
the same value of \c T.

The \ref date_input_field "date input field"'s parser provides a default
year, if none was typed in, and its formatter converts the full date to
canonical format. In this manner, typing in just the month and the day
then tabbing out of the date input field updates it to show the full
date.

create_validated_input_field_contents() optional third closure gets called after
the input field's validated value changes, and it receives the new validated
value.

*/

template<input_field_validation_function validation_function,
	 typename canonical_format_functor,
	 typename new_value_functor=default_new_value_function<
		 input_field_validation_function_type<validation_function>
		 >
	 >
auto create_validated_input_field_contents(
	validation_function &&validator,
	canonical_format_functor &&formatter,
	new_value_functor &&new_value=new_value_functor{}
) -> create_validator_ret_t<validation_function>
{
	// Figure out the validator's first parameter.

	typedef input_field_detect_validator_functor_parameter
		<validation_function> validator_t;

	// Figure out the std::optional that the validator returns.

	typedef typename validator_t::optional_return_type_t type;

	validated_input_field_contents<type> contents=
		ref<validated_input_field_implObj
		    <type, canonical_format_functor>>
		::create(std::forward<canonical_format_functor>(formatter));

	return {
		[contents, validator, new_value]
		(ONLY IN_THREAD,
		 auto &lock,
		 const callback_trigger_t &trigger)->bool
		{
			auto v=validator(IN_THREAD,
					 trim(validator_t::get(lock)),
					 lock,
					 trigger);

			contents->set(IN_THREAD, lock, v);

			if (!v)
				return false;

			// Declare the new value.
			new_value(IN_THREAD, *v);

			return true;
		},
		contents
	};
}

/*! Create a validator for an input field using the >> operator.

Uses create_validated_input_field_contents() to create an
\ref input_field "input field"
validator that uses the >> operator to convert the input field's contents
to an object. This is convenient when the object is an integral type, but
any default-constructible type can be used.

create_validated_input_field_contents() returns the same values: the
callback that gets passed to the input field's "on_validate()" and
a \ref validated_input_field_contents "validated_input_field_contents<T>"
validator object.

\code
const auto &[callback, contents] = create_string_validated_input_field_contents(
         []
         (ONLY IN_THREAD,
          const std::string &value,
          int *parsed_value,
          INSERT_LIBX_NAMESPACE::w::input_lock &lock,
          const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger)-> std::optional<int>
         {
             if (parsed_value)
             {
                 if (*parsed_value > 99)
                 {
                      lock.stop_message("Invalid number");
                      return std::nullopt;
                 }
             }

             return *parsed_value;
         },
         []
         (int v) -> std::string
         {
             return std::to_string(v);
         },
         []
         (ONLY IN_THREAD, int v)
         {
             std::cout << "New value: " << *v << std::endl;
         });
\endcode

The first parameter is different from
\ref create_validated_input_field_contents "create_validated_input_field_contents()"'s,
the remaining parameters are the same. The first parameter is a callable object
that receives the following parameters

- the closure gets invoked IN_THREAD.

- a \c std::string with the input field's contents

- a possibly NULL pointer to an object that the input field's contents were
converted to using the >> operator. A NULL value indicates a conversion failure
which could be because the field is empty (the closure can look at the
string parameter to verify that).

- an input lock and a trigger, they serve the same purpose as the corresponding
parameters to the analogous create_validated_input_field_contents() callback.

The closure returns a \c std::optional of the same object that the pointer
parameter points to. The closure can return a different value, which updates
the input field.

 */

template<input_field_string_validation_function validation_function,
	 typename canonical_format_functor,
	 typename new_value_functor=default_new_value_function<
		 input_field_string_validation_function_type<
			 validation_function>
		 >
	 >
auto create_string_validated_input_field_contents(
	validation_function &&validator,
	canonical_format_functor &&canonical_format,
	new_value_functor &&new_value=new_value_functor{}
) -> create_string_validator_ret_t<validation_function>
{
	// The validator returns a std::optional, figure out
	// the type being returned.

	typedef input_field_string_validation_function_type<
		validation_function> object_type;

	return create_validated_input_field_contents(
		[validator]
		(ONLY IN_THREAD,
		 const std::string &value,
		 auto &lock,
		 const callback_trigger_t &trigger)
		 {
			 // Non empty value entered into the input field.

			 if (!value.empty())
			 {
				 // Use the formatted extraction operator.

				 object_type v{};

				 std::istringstream i{value};

				 i >> v;

				 if (!i.fail())
				 {
					 // Even if it succeeds, we want to
					 // make sure it consumed all input.

					 i.get();
					 if (i.eof())
					 {
						 return validator(IN_THREAD,
								  value, &v,
								  lock,
								  trigger);
					 }
				 }
			 }

			 return validator(IN_THREAD,
					  value, nullptr,
					  lock, trigger);
		 },

		 std::forward<canonical_format_functor>(canonical_format),
		 std::forward<new_value_functor>(new_value));
}

LIBCXXW_NAMESPACE_END

#endif
