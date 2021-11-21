/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_fieldobj_h
#define x_w_input_fieldobj_h

#include <x/w/peepholed_focusableobj.H>
#include <x/w/input_fieldfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/containerfwd.H>
#include <x/w/input_field_config.H>
#include <x/w/validated_input_fieldfwd.H>
#include <x/functionalrefptrfwd.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! Common methods employed by input fields and editable combo-boxes.

//! Implements methods that are common to \ref input_field "input fields" and
//! \ref editable_comboboxlayoutmanager "editable combo-box layout managers"
//! that use input fields.

class input_field_setObj {

	virtual input_fieldObj *my_input_field()=0;

public:

	template<input_field_validation_function validation_function,
		 typename canonical_format_functor,
		 typename ...Args>
	auto set_validator(validation_function &&validator,
			   canonical_format_functor &&canonical_factor,
			   Args && ...args)
		-> validated_input_field<
		typename input_field_detect_validator_functor_parameter<
			validation_function
			>::optional_return_type_t>;

	template<input_field_validation_function validation_function,
		 typename canonical_format_functor,
		 typename ...Args>
	auto set_validator(ONLY IN_THREAD,
			   validation_function &&validator,
			   canonical_format_functor &&canonical_factor,
			   Args && ...args)
		-> validated_input_field<
		typename input_field_detect_validator_functor_parameter<
			validation_function
			>::optional_return_type_t>;

	template<typename T,
		 input_field_string_validation_function<T> validation_function,
		 typename canonical_format_functor,
		 typename ...Args>
	auto set_string_validator(validation_function &&validator,
				  canonical_format_functor &&canonical_format,
				  Args && ...args)
		-> validated_input_field<T>;

	template<typename T,
		 input_field_string_validation_function<T> validation_function,
		 typename canonical_format_functor,
		 typename ...Args>
	auto set_string_validator(
		ONLY IN_THREAD,
		validation_function &&validator,
		canonical_format_functor  &&canonical_format,
		Args && ...args)
		->validated_input_field<T>;

	//! Set the contents of the input field.
	void set(const std::string_view &str, bool validated=false);

	//! Set the contents of the input field, as unicode.
	void set(const std::u32string_view &str, bool validated=false);

	//! Set the contents of the input field.
	void set(ONLY IN_THREAD,
		 const std::string_view &str, bool validated=false);

	//! Set the contents of the input field, as unicode.
	void set(ONLY IN_THREAD,
		 const std::u32string_view &str, bool validated=false);

	//! Validate the input field, if needed.

	//! Returns an indication if the input field is validated.
	bool validate_modified(ONLY IN_THREAD);

	//! Validate the input field, if needed.

	//! Returns an indication if the input field is validated.
	bool validate_modified(ONLY IN_THREAD, const callback_trigger_t &);
};

//! A text input_field object.

//! The text input field is actually a container; however the container
//! is managed internally by the library. Do not access the internal
//! container, only use the input field's methods.

//! \see input_field

class input_fieldObj : public peepholed_focusableObj,
		       public input_field_setObj {

	//! Return this
	input_fieldObj *my_input_field() override;

public:
	class LIBCXX_HIDDEN implObj;

	friend class input_field_setObj;
	//! Internal implementation object.

	const ref<implObj> impl;

	//! Constructor
	input_fieldObj(const ref<implObj> &impl,
		       const ref<peepholed_focusableObj::implObj> &peephole_impl,
		       const layout_impl &container_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~input_fieldObj() LIBCXX_HIDDEN;


	//! Install a callback that gets invoked when the contents of the input field change.

	void on_change(const functionref<void(THREAD_CALLBACK,
					      const input_change_info_t &)>&);

	//! Install a callback that gets invoked when the contents of the input field change.

	void on_change(ONLY IN_THREAD,
		       const functionref<void(THREAD_CALLBACK,
					      const input_change_info_t &)>&);

	//! Install a callback that gets invoked to autocomplete typed in text.

	void on_autocomplete(const functionref<bool
			     (THREAD_CALLBACK,
			      input_autocomplete_info_t &)> &);

	//! Install a callback that gets invoked to autocomplete typed in text.

	void on_autocomplete(ONLY IN_THREAD,
			     const functionref<bool
			     (THREAD_CALLBACK,
			      input_autocomplete_info_t &)> &);

	//! Install a callback to validate modified input field contents.

	void on_validate(const input_field_validation_callback &);

	//! Install a callback to validate modified input field contents.

	void on_validate(ONLY IN_THREAD,
			 const input_field_validation_callback &);

	//! Install a callback to filter changes to the input field.

	void on_default_filter(const functionref<bool(char32_t)> &,
			       const std::vector<size_t> &,
			       char32_t empty=' ');

	//! Install a callback to filter changes to the input field.

	void on_default_filter(ONLY IN_THREAD,
			       const functionref<bool(char32_t)> &,
			       const std::vector<size_t> &,
			       char32_t empty=' ');

	//! Install a callback to filter changes to the input field.

	void on_filter(const functionref<input_field_filter_callback_t> &);

	//! Install a callback to filter changes to the input field.

	void on_filter(ONLY IN_THREAD,
		       const functionref<input_field_filter_callback_t> &);

	//! Installs or replaces the search callback

	void on_search(const input_field_config::search_info &search_info);

	//! Installs or replaces the search callback

	void on_search(ONLY IN_THREAD,
		       const input_field_config::search_info &search_info);

	//! Inherited from elementObj

	//! The minimum overrides are set for the editor widget
	//! inside the focus frame.

	ref<elementObj::implObj> get_minimum_override_element_impl() override;

	//! Override peepholed_focusable.
	void do_get_impl(const function<internal_focusable_cb> &)
		const override LIBCXX_HIDDEN;

	typedef functionref<void (THREAD_CALLBACK,
				  input_lock &lock,
				  const callback_trigger_t &,
				  const busy &)> spin_callback_t;


	//! Install the spin control callbacks

	void on_spin(const spin_callback_t &,
		     const spin_callback_t &);

	//! Install the spin control callbacks

	void on_spin(ONLY IN_THREAD,
		     const spin_callback_t &,
		     const spin_callback_t &);

	//! Return the current contents of the input field as UTF-8.
	std::string get(const std::optional<bidi_format> &embedding=
			std::nullopt) const;

	//! Return the current contents of the input field, as unicode.
	std::u32string get_unicode(const std::optional<bidi_format> &embedding=
				   std::nullopt) const;
};

LIBCXXW_NAMESPACE_END

#endif
