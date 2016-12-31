/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef values_and_mask_h
#define values_and_mask_h

#include <vector>
#include <map>
#include <cstdint>
#include <utility>
#include <algorithm>

//! Assemble a list of values, and a mask flagging the numbers in the list.

//! Several X protocol parameters take the form of a mask value, and an
//! array of values, with the mask used to interpret the value array.
//!
//! The constructor takes an even list of values, a list of pair.

class values_and_mask {

public:
	std::map<uint32_t, uint32_t> m;

	template<typename ...Args> values_and_mask(Args && ...args)
	{
		_add(std::forward<Args>(args)...);
	}

	inline void _add()
	{
	}

	template<typename ...Args> inline void _add(uint32_t key,
						    uint32_t value,
						    Args && ...args)
	{
		m[key]=value;
		_add(std::forward<Args>(args)...);
	}

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

};

#endif
