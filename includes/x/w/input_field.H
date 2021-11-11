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
#include <x/functionalrefptr.H>
#include <sstream>
#include <utility>
#include <string_view>

LIBCXXW_NAMESPACE_START

template<input_field_validation_function validation_function,
	 typename canonical_format_functor>
inline auto input_fieldObj::set_validator(
	validation_function &&validator,
	canonical_format_functor &&canonical_format
)
{
	return set_validator(std::forward<validation_function>(validator),
			     std::forward<canonical_format_functor>
			     (canonical_format),
			     []
			     (ONLY IN_THREAD, const auto &ignore)
			     {
			     });
}

//! Install input field validation.

//! \see input_field

template<input_field_validation_function validation_function,
	 typename canonical_format_functor,
	 typename new_value_functor>
auto input_fieldObj::set_validator(
	validation_function &&validator,
	canonical_format_functor &&canonical_format,
	new_value_functor &&new_value
)
{
	// Figure out the validator's first parameter.

	typedef input_field_detect_validator_functor_parameter
		<validation_function> validator_t;

	// Figure out the std::optional that the validator returns.

	typedef typename validator_t::optional_return_type_t type;

	// Construct the validated object, and install an on_keyboard_focus()
	// callback.
	validated_input_field_contents<type> validated{
		ref<validated_input_field_implObj<type,
		std::remove_reference_t<canonical_format_functor>>>
		::create(std::forward<canonical_format_functor>
			 (canonical_format))
	};

	auto new_validator=
		validated_input_field<type>::create(validated, ref{this});

	on_validate
		([contents=validated, validator, new_value]
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

template<input_field_string_validation_function validator_functor,
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

template<input_field_string_validation_function validator_functor,
	 typename canonical_format_functor,
	 typename new_value_functor>
auto input_fieldObj::set_string_validator(validator_functor &&validator,
					  canonical_format_functor
					  &&canonical_format,
					  new_value_functor &&new_value)
{
	// The validator returns a std::optional, figure out
	// the type being returned.

	typedef typename detect_input_field_string_validation_function<
		validator_functor>::type object_type;

	return set_validator
		([=]
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

#include <x/w/editable_comboboxlayoutmanager_input_field.H>

LIBCXXW_NAMESPACE_END

#endif
