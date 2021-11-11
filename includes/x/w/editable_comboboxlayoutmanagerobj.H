/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editable_comboboxlayoutmanagerobj_h
#define x_w_editable_comboboxlayoutmanagerobj_h

#include <x/w/editable_comboboxlayoutmanagerfwd.H>
#include <x/w/standard_comboboxlayoutmanagerobj.H>
#include <x/w/input_field_lock.H>
#include <x/w/input_fieldfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see editable_comboboxlayoutmanager
//!
//! Inherits from input_lock first, so that the editor element gets locked
//! first, before the list item layout manager's lock gets created.
//!
//! The internal on_change() callback gets invoked if set() gets called
//! here, with editor locked, and constructs its own public object.
//!
//! This maintains the same locking order.

class editable_comboboxlayoutmanagerObj :
	private input_lock,
	public standard_comboboxlayoutmanagerObj {

	//! The combo-box's input field.

	const input_field locked_input_field;

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	editable_comboboxlayoutmanagerObj(const ref<implObj> &impl,
					  const ref<listlayoutmanagerObj
					  ::implObj>
					  &list_layout_impl)
		LIBCXX_HIDDEN;

private:
	//! Delegating constructor
	editable_comboboxlayoutmanagerObj(
		const ref<implObj> &impl,
		const input_field &locked_input_field_arg,
		const ref<listlayoutmanagerObj::implObj> &list_layout_impl)
		LIBCXX_HIDDEN;
public:
	//! Destructor
	~editable_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Install a new selection changed callback.
	void on_selection_changed(const editable_combobox_selection_changed_t &)
		;
	//! Install a new selection changed callback.
	void on_selection_changed(ONLY IN_THREAD,
				  const editable_combobox_selection_changed_t &)
		;

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using standard_comboboxlayoutmanagerObj::generate;

	/////////////////////////////////////////////////////////////////////
	//
	// Access the underlying input field.

	using input_lock::size;
	using input_lock::pos;
	using input_lock::get;
	using input_lock::get_unicode;

	//! Forward set() to the underlying input field

	template<typename ...Args> struct set_args_t;

	template<typename ...Args>
	typename set_args_t<Args...>::void_t set(Args && ...args);

	//! Contents of the input field were updated directly

	//! Equivalent to on_validate() for the underlying input field.

	void on_validate(const functionref<input_field_validation_callback_t>
			 &);

	//! Install a validator

	//! Equivalent to calling set_validator() for the underlying
	//! input_field.
	template<typename ...Args>
	auto set_validator(Args && ...args);

	//! Install a validator

	//! Equivalent to calling set_validator() for the underlying
	//! input_field.
	template<typename ...Args>
	auto set_string_validator(Args && ...args);

	//! Check validation status

	//! Equivalent to calling validate_modified() for the underlying
	//! input_field.
	template<typename ...Args>
	bool validate_modified(Args && ...args);
};

LIBCXXW_NAMESPACE_END

#endif
