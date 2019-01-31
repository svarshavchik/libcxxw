/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef returned_pointer_H
#define returned_pointer_H

#include <cstdlib>
#include <algorithm>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

// This is returned by return_pointer().
//
// A pointer that's going to get returned from xcb that must be freed,
// when we're done with it.

template<typename ptr_type>
class returned_pointer { // Inherit from it

	ptr_type ptr;

public:

	// Set it to null, and we expect it to be assigned, somehow.

	returned_pointer() : ptr(nullptr) {}

	// No copying/assigning.

	returned_pointer(const returned_pointer<ptr_type> &)=delete;
	returned_pointer<ptr_type> &operator=(const returned_pointer<ptr_type>
					      &)=delete;

	// But moving is allowed.
	returned_pointer(returned_pointer<ptr_type> &&o)
		: ptr(o.ptr)
	{
		o.ptr=nullptr;
	}

	returned_pointer<ptr_type> &operator=(returned_pointer<ptr_type> &&o)
	{
		std::swap(ptr, o.ptr);
		return *this;
	}

	// Adopt a returned pointer from some xcb_ function.

	returned_pointer(ptr_type ptrArg) : ptr(ptrArg) {}

	// Pretend we're that pointer.

	auto &operator*() const { return *ptr; }

	auto &operator[](size_t i) { return ptr[i]; }

	ptr_type operator->() const { return ptr; }

	operator ptr_type() const { return ptr; }

	operator bool() const { return ptr != nullptr; }

	bool operator !() const { return ptr == nullptr; }

	//returned_pointer &operator=(ptr_type *ptrArg) // Happens just once.
	//{
	//	ptr=ptrArg;
	//}

	ptr_type *addressof() { return &ptr; } // Expect to pass arg to xcb func
	~returned_pointer() { if (ptr) free(ptr); }
};

// Helper method: take a pointer that's returned from some xcb function,
// that must be explicitly free()d after use, and make it RAII-compliant.

template<typename ptr_type>
inline auto return_pointer(ptr_type &&val)
{
	return returned_pointer<typename std::remove_reference<ptr_type>::type >
		(std::forward<ptr_type>(val));
}

LIBCXXW_NAMESPACE_END

#endif
