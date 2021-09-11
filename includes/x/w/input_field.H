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
			      std::declval<const input_field &>(),
			      std::declval<const callback_trigger_t &>()))>>
	: public input_field_get_for_validator<std::string> {};

//! Specialization: the input field validator takes a \c std::u32string parameter.
template<typename validator_functor>
class input_field_detect_validator_functor_parameter<
	validator_functor,
	std::void_t<decltype(std::declval<validator_functor &&>()
			     (std::declval<THREAD_CALLBACK>(),
			      std::declval<const std::u32string &>(),
			      std::declval<const input_field &>(),
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

//! Implementation object.

//! \see validated_input_field

template<typename type>
class validated_input_fieldObj : virtual public obj {

public:

	validated_input_fieldObj()=default;
	~validated_input_fieldObj()=default;

	//! Most recently validated value.

	//! This should not be modified, call set() to do this and update
	//! the input field.
	mutable mpobj<std::optional<type>> validated_value;

	//! Set a new value in the input field.

	virtual void set(const std::optional<type> &v)=0;

	//! Set a new value in the input field.

	virtual void set(ONLY IN_THREAD,
			 const std::optional<type> &v)=0;
};

template<typename type, typename canonical_formatter>
class LIBCXX_PUBLIC validated_input_field_implObj;

//! Implement the \ref validated_input_field "input field validation" object.

//! \internal

template<typename type, typename canonical_formatter>
class validated_input_field_implObj : public validated_input_fieldObj<type> {

public:

	//! The input field.

	const weakptr<input_fieldptr> my_inputfield;

	//! Captured formatting callable object.
	const canonical_formatter formatter;

	//! What the formatter returns: std::string or std::u32string

	typedef decltype(std::declval<canonical_formatter &>()
			 (std::declval<const type &>())
			 ) formatter_ret_t;

	//! Alias
	typedef validated_input_field_implObj<type, canonical_formatter> me_t;

	struct LIBCXX_HIDDEN set_logic;
	struct LIBCXX_HIDDEN set_logic_in_thread;
	struct LIBCXX_HIDDEN set_logic_not_in_thread;
	template<typename base_type> struct LIBCXX_HIDDEN set_impl;

	//! Common logic shared by set() overloads.

	struct set_logic {

		//! My validated input field object.
		me_t &me;

		//! A lock on the validated value.
		typedef typename mpobj<std::optional<type>>::lock value_lock_t;

		//! Constructor
		set_logic(me_t &me) : me{me}
		{
		}

		//! Set the new validated value

		//! Lock my input field and validated value, then invoke
		//! do_locked_set(). The implementation of do_locked_set()
		//! receives the locked objects, must updated the locked
		//! validated value, and call set_cleanup().
		//!
		//! The validated input field's set() overloads already have
		//! the new value, and they update the validated value
		//! directly, then call set_cleanup().
		//!
		//! The on_validate() callback retrieves the contents of the
		//! input field, uses the app-provided callback to construct
		//! a new value, place it in the validated value object,
		//! and then invoke set_cleanup().

		void do_set()
		{
			//! Find my input field
			auto my_field_ptr=me.my_inputfield.getptr();

			if (!my_field_ptr)
				return;

			input_field my_field{my_field_ptr};

			// An input_lock is obtained first...

			input_lock lock{my_field};

			// ... then the validated_value lock.

			value_lock_t value_lock{me.validated_value};

			do_locked_set(my_field, lock, value_lock);
		}

		//! Ready to set a new value

		//! do_set() calls it, after locking the input field and
		//! the validated value.
		//!
		//! The implementing subclass must update the validated
		//! value, and forward these parameters to set_cleanup().

		virtual void do_locked_set(const input_field &,
					   const input_lock &,
					   value_lock_t &)=0;

		//! Finished setting the new value.

		//! Format the validated value and invoke set_as_string(),
		//! if needed.
		//!
		//! set_as_string() gets implemented in either
		//! set_logic_not_in-thread or set_logic_in_thread.

		void set_cleanup(const input_field &my_field,
				 const input_lock &lock,
				 value_lock_t &value_lock)
		{
			auto &v=*value_lock;

			if (!v)
				// Do not set() the input field. No value
				// means it's not valid, and set()ing it
				// validates the contents of the input field.
				return;

			formatter_ret_t s=me.formatter(*v);

			// Do not bother with set() if the value is the same.

			if (s == input_field_get_for_validator<decltype(s)>
			    ::get(lock))
				return;
			set_as_string(my_field, s);
		}

		//! A new validated value has been set.

		//! Implemented in the subclass by calling the appropriate
		//! set() overload.
		virtual void set_as_string(const input_field &my_filter,
					   const formatter_ret_t &s)=0;
	};

	//! Setting the new validated value not in the connection thread.

	//! Invokes the non-IN_THREAD overload of set().
	struct set_logic_not_in_thread : set_logic {

		using set_logic::set_logic;

		//! Implement set_as_string()

		void set_as_string(const input_field &my_field,
				   const formatter_ret_t &s) override
		{
			my_field->set(s);
		}
	};

	//! Setting the new validated value not in the connection thread.

	//! Invokes the IN_THREAD overload of set().
	struct set_logic_in_thread : set_logic {

		//! We are running in the connection thread.
		ONLY IN_THREAD;

		//! Constructor saves IN_THREAD.
		set_logic_in_thread(ONLY IN_THREAD,
				    me_t &me) : set_logic{me},
						IN_THREAD{IN_THREAD}
		{
		}

		//! Implement set_as_string()

		void set_as_string(const input_field &my_field,
				   const formatter_ret_t &s) override
		{
			my_field->set(IN_THREAD, s);
		}
	};

	//! Constructor
	template<typename F>
	validated_input_field_implObj(const input_field &my_inputfield, F &&f)
		: my_inputfield{my_inputfield},
		formatter{std::forward<F>(f)}
	{
	}

	//! Destructor
	~validated_input_field_implObj()=default;

	//! set() a new value

	//! Common logic by both non-IN_THREAD and IN_THREAd set()
	//! overrides. set() passes the new validated value to the
	//! constructor. do_locked_set() gets implemented by copying the
	//! new value into the locked validated value, then invoking
	//! set_cleanup().

	template<typename base_type>
	struct set_impl : base_type {

		//! New validated value.
		const std::optional<type> &v;

		//! Constructor
		template<typename ...Args>
		set_impl(const std::optional<type> &v,
			 Args && ...args)
			: base_type{std::forward<Args>(args)...}, v{v}
		{
		}

		//! Implement do_locked_set().
		void do_locked_set(const input_field &my_field,
				   const input_lock &lock,
				   typename base_type::value_lock_t
				   &value_lock) override
		{
			*value_lock=v;
			this->set_cleanup(my_field, lock, value_lock);
		}
	};


	//! Implement set().

	void set(const std::optional<type> &v) override
	{
		set_impl<set_logic_not_in_thread> impl{v, *this};

		impl.do_set();
	}

	//! Implement set().

	void set(ONLY IN_THREAD, const std::optional<type> &v) override
	{
		set_impl<set_logic_in_thread> impl{v, IN_THREAD, *this};

		impl.do_set();
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
				   std::declval<input_field &>(),
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
		::create(ref{this},
			 std::forward<canonical_format_functor>
			 (canonical_format));

	//! The constructed validated input field implementation type.
	typedef decltype(validated) validated_t;

	//! The validated input field's base class.

	//! Alias for the me_t type of the \c validated object.
	typedef typename refptr_traits<validated_t>::obj_t::me_t me_t;

	//! Alias for the set_logic_in_thread in the \c validated_object.
	typedef typename me_t::set_logic_in_thread set_logic_in_thread;

	//! Implement the on_validate() callback by set()ing the validated value

	struct on_validate_set_impl : set_logic_in_thread {

		//! The validator
		const validator_functor &validator;

		//! The trigger that invoked the validator.
		const callback_trigger_t &trigger;

		//! New validated value, if there is one.
		std::optional<std::optional<type>> new_value;

		//! Constructor
		on_validate_set_impl(ONLY IN_THREAD,
				     me_t &me,
				     const validator_functor &validator,
				     const callback_trigger_t &trigger)
			: set_logic_in_thread{IN_THREAD, me},
			  validator{validator},
			  trigger{trigger}
		{
		}

		//! Implement do_locked_set().

		//! Gets the value of the input field, calls the validator
		//! that computes the validated value, sets it, then
		//! calls set_cleanup().
		void do_locked_set(const input_field &my_field,
				   const input_lock &lock,
				   typename set_logic_in_thread::value_lock_t
				   &value_lock) override
		{
			// Get the contents of the input
			// field, trim off the
			// leading and the trailing
			// whitespace, invoke
			// the validator function.

			*value_lock=validator(this->IN_THREAD,
					      trim(validator_t::get(lock)),
					      my_field,
					      trigger);

			// And set the validated value.

			new_value=*value_lock;

			this->set_cleanup(my_field, lock, value_lock);
		}
	};

	on_validate
		([=]
		 (ONLY IN_THREAD,
		  const callback_trigger_t &trigger)->bool
		 {
			 on_validate_set_impl impl{IN_THREAD,
						   *validated,
						   validator,
						   trigger};

			 impl.do_set();

			 // No new value was produced, valid or not. We must
			 // be in the process of getting destroyed, so punt.
			 if (!impl.new_value)
				 return true;

			 // Declare the new value.
			 new_value(IN_THREAD, *impl.new_value);

			 return impl.new_value->has_value();
		 });

	return validated_input_field<type>{validated};
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
		  const input_field &f,
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
						    std::declval<const input_field &>
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
								  f,
								  trigger);
					 }
				 }
			 }

			 return validator(IN_THREAD,
					  value, nullptr,
					  f, trigger);
		 },

		 std::forward<canonical_format_functor>(canonical_format),
		 std::forward<new_value_functor>(new_value));
}

#include <x/w/editable_comboboxlayoutmanager_input_field.H>

LIBCXXW_NAMESPACE_END

#endif
