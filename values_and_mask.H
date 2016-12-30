#ifndef values_and_mask_h
#define values_and_mask_h

#include <vector>
#include <cstdint>
#include <utility>

//! Assemble a list of values, and a mask flagging the numbers in the list.

//! Several X protocol parameters take the form of a mask value, and an
//! array of values, with the mask used to interpret the value array.
//!
//! The constructor takes an even list of values, a list of pair.
//! The first value in each pair gets ORed into mask, and the second value
//! of each gets added into the values vector.

class values_and_mask {

public:
	uint32_t mask=0;
	std::vector<uint32_t> values;

	template<typename ...Args> values_and_mask(Args && ...args)
	{
		values.reserve( sizeof...(Args)/2 );
		_add(std::forward<Args>(args)...);
	}

	inline void _add()
	{
	}

	template<typename ...Args> inline void add(uint32_t key,
						   uint32_t value,
						   Args && ...args)
	{
		mask |= key;
		values.push_back(value);
		add(std::forward<Args>(args)...);
	}
};

#endif
