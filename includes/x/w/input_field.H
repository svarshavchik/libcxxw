/*
** Copyright 2017-2020 Double Precision, Inc.
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

	mutable mpobj<std::optional<type>> validated_value;

	//! Set a new value in the input field.

	virtual void set(const std::optional<type> &v)=0;
};

//! Implement the \ref validated_input_field "input field validation" object.

//! \internal

template<typename type, typename canonical_formatter>
class validated_input_field_implObj : public validated_input_fieldObj<type> {

public:

	//! The input field.

	const weakptr<input_fieldptr> my_inputfield;

	//! Captured formatting callable object.
	const canonical_formatter formatter;

	//! Constructor
	template<typename F>
	validated_input_field_implObj(const input_field &my_inputfield, F &&f)
		: my_inputfield{my_inputfield},
		formatter{std::forward<F>(f)}
	{
	}

	//! Destructor
	~validated_input_field_implObj()=default;

	//! Implement set().

	void set(const std::optional<type> &v) override
	{
		auto my_field_ptr=this->my_inputfield.getptr();

		if (!my_field_ptr)
			return;

		// An input_lock is obtained first...

		input_lock lock{my_field_ptr};

		// ... then the validated_value lock.
		//
		// Same locking order as in install_validator().

		typename mpobj<std::optional<type>>::lock
			value_lock{this->validated_value};

		set(v, my_field_ptr, lock, value_lock);
	}

	//! Implement set().

	//! Everything is locked. Set the most recently validated value,
	//! and use the formatter to convert it to a text value, then
	//! set() the input field.

	void set(const std::optional<type> &v,
		 const input_field &my_input_field,
		 input_lock &my_input_field_lock,
		 typename mpobj<std::optional<type>>::lock &value_lock)
	{
		*value_lock=v;

		if (!v)
			// Do not set() the input field. No value means it's
			// not valid, and set()ing it validates the contents
			// of the input field.
			return;

		auto s=formatter(*v);

		// Do not bother with set() if the value is the same.

		if (s == input_field_get_for_validator<decltype(s)>
		    ::get(my_input_field_lock))
			return;

		// We just validated this, so note this value as a validated
		// one.
		my_input_field->set(s, true);
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
		::create(ref(this),
			 std::forward<canonical_format_functor>
			 (canonical_format));

	on_validate
		([=]
		 (ONLY IN_THREAD,
		  const callback_trigger_t &trigger)->bool
		 {
			 auto my_field_ptr=validated->my_inputfield.getptr();

			 if (!my_field_ptr)
				 return true;

			 // An input_lock is obtained first...
			 // ... then the validated_value lock.
			 //
			 // Same locking order as above, but release the locks
			 // before calling the new_value callback, so that it
			 // gets called with all the locks released.

			 auto v=({
					 input_lock my_input_field_lock{
						 my_field_ptr};

					 typename mpobj<std::optional<type>>
						 ::lock
						 lock{validated
							 ->validated_value};

					 // Get the contents of the input
					 // field, trim off the
					 // leading and the trailing
					 // whitespace, invoke
					 // the validator function.

					 auto v=validator
						 (IN_THREAD,
						  trim(validator_t::get
						       (my_input_field_lock)),
						  my_field_ptr, trigger);

					 // And set the validated value.

					 validated->set(v, my_field_ptr,
							my_input_field_lock,
							lock);

					 v;
				 });

			 new_value(IN_THREAD, v);
			 return v.has_value();
		 });

	return validated_input_field<type>(validated);
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

//! Identifies the kind of a filtering event

//! \see input_field_filter_info

enum class input_filter_type { inserting, deleting, replacing, move_only };

//! Parameter to the input field filter callback.

//! An input_field's on_filter() callback receives this parameter.
//!
//! A description of the change to the input field that results from a key
//! press (or a cut/paste operation).
//!
//! \see input_field

struct input_field_filter_info {

	//! Type of the input change.

	//! - deleting: deleting characters in the input field.
	//!
	//! - inserting: adding characters to the input field.
	//!
	//! - replacing: replacing the contents of the input field in any
	//! other way (such as by calling input_field's set() method).
	//!
	//! - move_only: the cursor is being moved, no changes to the input
	//! field.

	input_filter_type type;

	//! Starting character index
	size_t starting_pos;

	//! Number of characters to be removed
	size_t n_delete;

	//! New contents to be inserted at the starting position.

	//! n_delete may also be set to a non-0 value. For example, highlighting
	//! a portion of the input field and executing a paste operation sets
	//! n_delete to the number of characters that are highlighted, and
	//! new_contents to the pasted contents.

	std::u32string_view new_contents;

	//! Current number of characters in the input field.
	size_t size;

	//! Maximum size of the input field. Originally from input_field_config.
	size_t maximum_size;

	//! Return the original position of the cursor in the input field

	//! For type=move_only, starting_pos is the position where the cursor
	//! is getting moved to, and this returns the original cursor
	//! starting position; n_delete is 0, and new_contents is an empty
	//! string.
	//!
	//! For all other types this returns the same value as starting_pos.
	virtual size_t original_pos() const=0;

	//! Constructor
	input_field_filter_info(input_filter_type type,
				size_t starting_pos,
				size_t n_delete,
				const std::u32string_view &new_contents,
				size_t size,
				size_t maximum_size);

	//! Apply this change.
	void update() const LIBCXX_PUBLIC;

	//! Apply some other kind of a change.
	virtual void update(size_t starting_pos,
			    size_t n_delete,
			    const std::u32string_view &new_contents) const
		LIBCXX_PUBLIC=0;

	//! Move the cursor to the given position.
	virtual void move(size_t pos) const LIBCXX_PUBLIC=0;

	//! Move the cursor to select the entire contents of the input field.

	virtual void select_all() const LIBCXX_PUBLIC=0;
};

LIBCXXW_NAMESPACE_END

#endif
