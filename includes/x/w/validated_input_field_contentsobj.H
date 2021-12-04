/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_validated_input_field_contentsobj_h
#define x_w_validated_input_field_contentsobj_h

#include <x/w/validated_input_field_contentsfwd.H>
#include <x/w/input_fieldfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/mpobj.H>
#include <optional>
#include <utility>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! Inherited by validated_input_field_contentsObj and defines its abstract
//! methods.

class validated_input_field_contents_baseObj : virtual public obj {

public:
	validated_input_field_contents_baseObj();
	~validated_input_field_contents_baseObj();

	//! Implement to_text_param.

	//! This is used when creating a new input field. This retrieves
	//! the initial contents of the input field that represents the
	//! initial value, and places it into the text_param, returning
	//! true.
	//!
	//! A false return value indicates an unspecified initial value.

	virtual bool to_text_param(text_param &t)=0;

	//! Construct the validated_input_field<type> object.

	//! Implemented in a validated_input_field_contents<type> subclass,
	//! and returns a validated_input_field<type> object for the
	//! new input field.
	//!
	//! Returns an opaque object. This is used by the generator, with
	//! app-provided validators that get attached to the generated
	//! input field. The generator has no knowledge of the actual type.

	virtual ref<obj> create_validated_input_field(const input_field &)=0;
};

//! Implementation object.

//! \see validated_input_field_contents

template<typename type>
class validated_input_field_contentsObj
	: public validated_input_field_contents_baseObj {

public:
	friend class validated_input_fieldObj<type>;

	validated_input_field_contentsObj(
		const std::optional<type> &initial_value
	) : validated_value{initial_value}
	{
	}

	~validated_input_field_contentsObj()=default;

	friend class validated_input_fieldObj<type>;
	friend class input_field_setObj;

	//! Public access to the current value.

	auto value() const
	{
		return validated_value.get();
	}

	template<typename default_value_t>
	auto value_or(default_value_t &&default_value) const
	{
		typename mpobj<std::optional<type>,
			       std::recursive_mutex>::lock lock{
			validated_value
		};

		return lock->value_or(std::forward<default_value_t>(
					      default_value
				      ));
	}

	template<typename functor_t>
	auto access(functor_t &&f) const
	{
		typename mpobj<std::optional<type>,
			       std::recursive_mutex>::lock lock{
			validated_value
		};

		const auto &cv=*lock;

		return f(cv);
	}

protected:
	//! Most recently validated value.

	//! This should not be modified, call set() to do this and update
	//! the input field.
	mutable mpobj<std::optional<type>,
		      std::recursive_mutex> validated_value;

public:
	//! Replace the contents of th einput field

	virtual void set(ONLY IN_THREAD,
			 input_lock &lock,
			 const std::optional<type> &v,
			 bool update_if_invalid=true)=0;

private:
	//! Refresh the input field with the current validated value.

	//! Called in set_validator() when installing the input field in order
	//! to show its initial value.

	void reset(ONLY IN_THREAD, input_lock &lock)
	{
		typename mpobj<std::optional<type>,
			       std::recursive_mutex>::lock value_lock{
			validated_value
		};

		set(IN_THREAD, lock, *value_lock);
	}
};

LIBCXXW_NAMESPACE_END

#endif
