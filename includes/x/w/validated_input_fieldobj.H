/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_validated_input_fieldobj_h
#define x_w_validated_input_fieldobj_h

#include <x/w/validated_input_fieldfwd.H>
#include <x/w/validated_input_field_contentsfwd.H>
#include <x/w/input_fieldobj.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see validated_input_field

template<typename type>
class validated_input_fieldObj : virtual public obj {

	const input_field my_input_field;

public:

	const validated_input_field_contents<type> contents;

	validated_input_fieldObj(
		const validated_input_field_contents<type> &contents,
		const input_field &my_input_field)
		: my_input_field{my_input_field},
		  contents{contents}
	{
	}

	~validated_input_fieldObj()=default;

	auto value() const
	{
		return contents->value();
	}

	template<typename default_value_t>
	auto value_or(default_value_t &&default_value) const
	{
		return contents->value_or(std::forward<default_value_t>(
						  default_value
					  ));
	}

	template<typename functor_t>
	auto access(functor_t &&f) const
	{
		const auto &obj=*contents;

		return obj.access(std::forward<functor_t>(f));
	}

	//! Set a new value in the input field.

	void set(const std::optional<type> &v)
	{
		my_input_field->in_thread(
			[v, me=ref{this}]
			(ONLY IN_THREAD)
			{
				me->set(IN_THREAD, v);
			});
	}

	//! Set a new value in the input field.

	void set(ONLY IN_THREAD,
		 const std::optional<type> &v)
	{
		input_lock lock{my_input_field};

		contents->set(IN_THREAD, lock, v);
	}
};

LIBCXXW_NAMESPACE_END

#endif
