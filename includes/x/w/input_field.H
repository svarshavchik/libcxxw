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
#include <x/w/text_param.H>

#include <x/ref.H>
#include <x/refptr_traits.H>
#include <sstream>
#include <utility>

LIBCXXW_NAMESPACE_START

template<input_field_validation_function validation_function,
	 typename canonical_format_functor,
	 typename ...Args>
auto input_field_setObj::set_validator(
	validation_function &&validator,
	canonical_format_functor &&canonical_format,
	Args && ...args
) -> validated_input_field<
	typename input_field_detect_validator_functor_parameter<
		validation_function
		>::optional_return_type_t>
{
	typedef typename input_field_detect_validator_functor_parameter<
		validation_function
		>::optional_return_type_t type;

	const auto &[validator_callback, contents] =
		create_validated_input_field_contents(
			std::forward<validation_function>(validator),
			std::forward<canonical_format_functor>(
				canonical_format
			),
			std::forward<Args>(args)...);

	auto me=my_input_field();

	me->in_thread([me=ref{me}, validator_callback, contents]
		      (ONLY IN_THREAD)
	{
		input_lock lock{me};

		me->on_validate(IN_THREAD, validator_callback);
		contents->reset(IN_THREAD, lock);
	});

	return validated_input_field<type>::create(contents, ref{me});
}

template<input_field_validation_function validation_function,
	 typename canonical_format_functor,
	 typename ...Args>
auto input_field_setObj::set_validator(
	ONLY IN_THREAD,
	validation_function &&validator,
	canonical_format_functor &&canonical_format,
	Args && ...args
) -> validated_input_field<
	typename input_field_detect_validator_functor_parameter<
		validation_function
		>::optional_return_type_t>
{
	typedef typename input_field_detect_validator_functor_parameter<
		validation_function
		>::optional_return_type_t type;

	const auto &[validator_callback, contents] =
		create_validated_input_field_contents(
			std::forward<validation_function>(validator),
			std::forward<canonical_format_functor>(
				canonical_format
			),
			std::forward<Args>(args)...);

	auto me=my_input_field();
	{
		input_lock lock{ref{me}};

		me->on_validate(IN_THREAD, validator_callback);
		contents->reset(IN_THREAD, lock);
	}
	return validated_input_field<type>::create(contents, ref{me});
}

//! Create a set_validator() by using a formatted string extraction operator, \c >>.

template<typename T,
	 input_field_string_validation_function<T> validation_function,
	 typename canonical_format_functor,
	 typename ...Args>
auto input_field_setObj::set_string_validator(
	validation_function &&validator,
	canonical_format_functor  &&canonical_format,
	Args && ...args
) ->validated_input_field<T>
{
	typedef T type;

	const auto &[validator_callback, contents] =
		create_string_validated_input_field_contents<T>(
			std::forward<validation_function>(validator),
			std::forward<canonical_format_functor>(
				canonical_format
			),
			std::forward<Args>(args)...);

	auto me=my_input_field();

	me->in_thread([me=ref{me}, validator_callback, contents]
		      (ONLY IN_THREAD)
	{
		input_lock lock{me};

		me->on_validate(IN_THREAD, validator_callback);
		contents->reset(IN_THREAD, lock);
	});

	return validated_input_field<type>::create(contents, ref{me});
}

//! Create a set_validator() by using a formatted string extraction operator, \c >>.

template<typename T,
	 input_field_string_validation_function<T> validation_function,
	 typename canonical_format_functor,
	 typename ...Args>
auto input_field_setObj::set_string_validator(
	ONLY IN_THREAD,
	validation_function &&validator,
	canonical_format_functor  &&canonical_format,
	Args && ...args)
	->validated_input_field<T>
{
	typedef T type;

	const auto &[validator_callback, contents] =
		create_string_validated_input_field_contents<T>(
			std::forward<validation_function>(validator),
			std::forward<canonical_format_functor>(
				canonical_format
			),
			std::forward<Args>(args)...);

	auto me=my_input_field();

	{
		input_lock lock{ref{me}};

		me->on_validate(IN_THREAD, validator_callback);
		contents->reset(IN_THREAD, lock);
	}

	return validated_input_field<type>::create(contents, ref{me});
}

LIBCXXW_NAMESPACE_END

#endif
