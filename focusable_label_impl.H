/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusable_label_impl_h
#define focusable_label_impl_h

#include "x/w/focusable_label.H"
#include "textlabel.H"
#include "child_element.H"
#include "label_elementfwd.H"
#include "focus/focusable_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN focusable_labelObj::implObj
	: public focusable_elementObj<label_elementObj<child_elementObj>> {

	//! Alias

	typedef focusable_elementObj<label_elementObj<child_elementObj>
				     > superclass_t;

 public:

	//! Constructor

	implObj(const ref<containerObj::implObj> &container,
		const text_param &text,
		double widthmm,
		halign alignment);

	//! Destructor
	~implObj();

	//! Implement uses_input_method()

	bool uses_input_method() override;

	//! Override keyboard_focus()

	//! Call report_current_cursor_position_if_active().

	void keyboard_focus(IN_THREAD_ONLY) override;

	//! Override current_position_updated()

	//! Call report_current_cursor_position_if_active().

	void current_position_updated(IN_THREAD_ONLY) override;

	//! report_current_cursor_position() when we have input focus.

	void report_current_cursor_position_if_active(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
