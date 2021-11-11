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

//! \see validated_input_field_contents

template<typename type>
class validated_input_field_contentsObj : virtual public obj {

public:

	validated_input_field_contentsObj()=default;
	~validated_input_field_contentsObj()=default;

	friend class validated_input_fieldObj<type>;

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
	//! Implement set().

	virtual void set(ONLY IN_THREAD,
			 input_lock &lock,
			 const std::optional<type> &v)=0;
};

LIBCXXW_NAMESPACE_END

#endif
