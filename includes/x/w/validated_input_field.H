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

		lock.set(IN_THREAD, s);
	}
};

LIBCXXW_NAMESPACE_END

#endif
