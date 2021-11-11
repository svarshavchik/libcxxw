/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_h
#define x_w_input_field_h

#include <x/w/input_fieldfwd.H>
#include <x/w/input_fieldobj.H>
#include <x/w/input_field_config.H>
#include <x/w/input_field_lock.H>
#include <x/w/validated_input_field.H>
#include <x/w/busy.H>
#include <x/w/main_window.H>

#include <x/ref.H>
#include <x/weakptr.H>
#include <x/strtok.H>
#include <x/functionalrefptr.H>
#include <x/refptr_traits.H>
#include <sstream>
#include <optional>
#include <utility>
#include <string_view>

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

//! Determine the type of the first parameter to an \ref input_field "input_field_validator".

template<typename validator_functor, typename=void>
class input_field_detect_validator_functor_parameter;

//! Specialization: the input field validator takes a \c std::string parameter.

template<typename validator_functor>
class input_field_detect_validator_functor_parameter<
	validator_functor,
	std::void_t<decltype(std::declval<validator_functor &&>()
			     (std::declval<THREAD_CALLBACK>(),
			      std::declval<const std::string &>(),
			      std::declval<input_lock &>(),
			      std::declval<const callback_trigger_t &>()))>>
	: public input_field_get_for_validator<std::string> {};

//! Specialization: the input field validator takes a \c std::u32string parameter.
template<typename validator_functor>
class input_field_detect_validator_functor_parameter<
	validator_functor,
	std::void_t<decltype(std::declval<validator_functor &&>()
			     (std::declval<THREAD_CALLBACK>(),
			      std::declval<const std::u32string &>(),
			      std::declval<input_lock &>(),
			      std::declval<const callback_trigger_t &>()))>>
	: public input_field_get_for_validator<std::u32string> {};

//! Determine the value returned by the input field validator.

template<typename> class input_field_validator_ret_t;

//! The input field validator must return a \c std::optional

//! Determine what the optional type is.

template<typename type>
class input_field_validator_ret_t<std::optional<type>> {
public:

	typedef type type_t;
};


template<typename type, typename canonical_formatter>
class LIBCXX_PUBLIC validated_input_field_implObj;

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

		lock.set(IN_THREAD, s);
	}
};

template<typename validator_functor,
	 typename canonical_format_functor>
inline auto input_fieldObj::set_validator(validator_functor &&validator,
					  canonical_format_functor
					  &&canonical_format)
{
	return set_validator(std::forward<validator_functor>(validator),
			     std::forward<canonical_format_functor>
			     (canonical_format),
			     []
			     (ONLY IN_THREAD, const auto &ignore)
			     {
			     });
}

//! Install input field validation.

//! \see input_field

template<typename validator_functor,
	 typename canonical_format_functor,
	 typename new_value_functor>
auto input_fieldObj::set_validator(validator_functor &&validator,
				   canonical_format_functor
				   &&canonical_format,
				   new_value_functor &&new_value)
{
	// Figure out the validator's first parameter.

	typedef input_field_detect_validator_functor_parameter
		<validator_functor> validator_t;

	typedef decltype(validator_t
			 ::get(std::declval<const input_lock &>())
			 ) validated_string_t;

	// Figure out the std::optional that the validator returns.

	typedef decltype(validator(std::declval<THREAD_CALLBACK>(),
				   std::declval<const validated_string_t &>(),
				   std::declval<input_lock &>(),
				   std::declval<const callback_trigger_t &>()))
		validator_optional_ret_t;

	typedef typename input_field_validator_ret_t<
		validator_optional_ret_t>::type_t type;

	// Construct the validated object, and install an on_keyboard_focus()
	// callback.
	auto validated=ref<validated_input_field_implObj
			   <type,
			    std::remove_reference_t
			    <canonical_format_functor>>>
		::create(std::forward<canonical_format_functor>
			 (canonical_format));

	auto new_validator=
		validated_input_field<type>::create(validated, ref{this});

	on_validate
		([contents=new_validator->contents, validator, new_value]
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
		 });

	return new_validator;
}

template<typename validator_functor,
	 typename canonical_format_functor>
inline auto input_fieldObj::set_string_validator(validator_functor &&validator,
						 canonical_format_functor
						 &&canonical_format)
{
	return set_string_validator(std::forward<validator_functor>(validator),
				    std::forward<canonical_format_functor>
				    (canonical_format),
				    []
				    (ONLY IN_THREAD, const auto &ignore)
				    {
				    });
}

//! Create a set_validator() by using a formatted string extraction operator, \c >>.

template<typename validator_functor,
	 typename canonical_format_functor,
	 typename new_value_functor>
auto input_fieldObj::set_string_validator(validator_functor &&validator,
					  canonical_format_functor
					  &&canonical_format,
					  new_value_functor &&new_value)
{
	return set_validator
		([=]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  auto &lock,
		  const callback_trigger_t &trigger)
		 {
			 // The validator returns a std::optional, figure out
			 // the type being returned.

			 typedef typename input_field_validator_ret_t<
				 decltype(validator(std::declval
						    <THREAD_CALLBACK>(),
						    std::declval<const
						    std::string &>(),
						    nullptr,
						    std::declval<input_lock &>
						    (),
						    trigger))>::type_t
				 object_type;

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

#include <x/w/editable_comboboxlayoutmanager_input_field.H>

LIBCXXW_NAMESPACE_END

#endif
