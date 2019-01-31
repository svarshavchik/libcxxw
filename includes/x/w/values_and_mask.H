/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_values_and_mask_h
#define x_w_values_and_mask_h

#include <vector>
#include <map>
#include <cstdint>
#include <utility>
#include <algorithm>

#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

//! Assemble a list of values, and a mask flagging the numbers in the list.

//! Helper class for constructing parameters to an X protocol request.
//!
//! Several X protocol parameters take the form of a mask value, and an
//! array of values, with the mask used to interpret the value array.
//!
//! The constructor takes an even list of values, a list of pair.

class values_and_mask {

public:
	//! Assembled map of parameter name/value pais.

	std::map<uint32_t, uint32_t> m;

	//! Default constructor
	values_and_mask()=default;

	//! Constructor

	template<typename Arg1, typename Arg2,
		 typename ...Args>
	values_and_mask(Arg1 &&arg1, Arg2 &&arg2, Args && ...args)
	{
		_add(std::forward<Arg1>(arg1),
		     std::forward<Arg2>(arg2),
		     std::forward<Args>(args)...);
	}

	//! Overloaded _add() helper.

	inline void _add()
	{
	}

	//! Overloaded _add() helper

	//! Adds the next pair of key/values to the map.

	template<typename ...Args> inline void _add(uint32_t key,
						    uint32_t value,
						    Args && ...args)
	{
		m[key]=value;
		_add(std::forward<Args>(args)...);
	}

	//! Return the parameter mask.

	//! The parameter mask is the logical-OR of all parameter keys.

	uint32_t mask() const
	{
		uint32_t v=0;

		std::for_each(m.begin(), m.end(),
			      [&]
			      (const auto &kv)
			      {
				      v |= kv.first;
			      });
		return v;
	}

	//! Return the parameter values

	//! The list of values in the map, in key order.

	std::vector<uint32_t> values() const
	{
		std::vector<uint32_t> v;

		v.reserve(m.size());

		std::for_each(m.begin(), m.end(),
			      [&]
			      (const auto &kv)
			      {
				      v.push_back(kv.second);
			      });
		return v;
	}

	//! Comparison operator

	bool operator==(const values_and_mask &o) const
	{
		return m == o.m;
	}
};

LIBCXXW_NAMESPACE_END

#endif
