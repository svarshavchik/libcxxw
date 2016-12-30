#ifndef values_and_mask_h
#define values_and_mask_h

#include <vector>
#include <cstdint>
#include <utility>

// Several X protocol parameters take the form of a mask value, and an
// array of values, with the mask used to interpret the value array.

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
