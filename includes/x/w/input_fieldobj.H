/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_fieldobj_h
#define x_w_input_fieldobj_h

#include <x/w/containerobj.H>
#include <x/w/focusableobj.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! A text input_field object.

//! The text input field is actually a container; however the container
//! is managed internally by the library. Do not access the internal
//! container, only use the input field's methods.

//! \see input_field

class input_fieldObj : public containerObj, public focusableObj {

public:
	class implObj;

	//! Internal implementation object.

	const ref<implObj> impl;

	//! Constructor
	input_fieldObj(const ref<implObj> &impl,
		       const ref<layoutmanagerObj::implObj> &layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~input_fieldObj() LIBCXX_HIDDEN;

	//! Return the current contents of the input field.
	std::string get() const;

	//! Return the current contents of the input field, as unicode.
	std::u32string get_unicode() const;

	//! Set the contents of the input field.
	void set(const std::experimental::string_view &str);

	//! Set the contents of the input field, as unicode.
	void set(const std::experimental::u32string_view &str);

	//! Inherited from focusableObj

	//! \internal

	ref<focusableImplObj> get_impl() const override LIBCXX_HIDDEN;

	//! Inherited from focusableObj

	//! \internal

	size_t internal_impl_count() const override LIBCXX_HIDDEN;

	//! Inherited from focusableObj

	//! \internal

	ref<focusableImplObj> get_impl(size_t) const override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
