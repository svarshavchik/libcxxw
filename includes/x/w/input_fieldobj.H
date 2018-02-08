/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_fieldobj_h
#define x_w_input_fieldobj_h

#include <x/w/peepholed_focusableobj.H>
#include <x/w/input_fieldfwd.H>
#include <string>
#include <functional>

LIBCXXW_NAMESPACE_START

//! A text input_field object.

//! The text input field is actually a container; however the container
//! is managed internally by the library. Do not access the internal
//! container, only use the input field's methods.

//! \see input_field

class input_fieldObj : public peepholed_focusableObj {

public:
	class implObj;

	//! Internal implementation object.

	const ref<implObj> impl;

	//! Constructor
	input_fieldObj(const ref<implObj> &impl,
		       const ref<peepholed_focusableObj::implObj> &peephole_impl,
		       const ref<layoutmanagerObj::implObj> &layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~input_fieldObj() LIBCXX_HIDDEN;

	//! Set the contents of the input field.
	void set(const std::string_view &str);

	//! Set the contents of the input field, as unicode.
	void set(const std::u32string_view &str);

	//! Install a callback that gets invoked when the contents of the input field change.

	void on_change(const std::function<void(const input_change_info_t &)>&);

	//! Install a callback that gets invoked to autocomplete typed in text.

	void on_autocomplete(const std::function<bool
			     (input_autocomplete_info_t &)> &);

	//! Install a callback to validate modified input field contents.

	void on_validate(const std::function<input_field_validation_callback_t>
			 &);

	//! Inherited from elementObj

	//! The minimum overrides are set for the editor display element
	//! inside the focus frame.

	ref<elementObj::implObj> get_minimum_override_element_impl() override;
};

LIBCXXW_NAMESPACE_END

#endif
