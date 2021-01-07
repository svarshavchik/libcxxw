/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusable_label_impl_h
#define focusable_label_impl_h

#include "x/w/focusable_label.H"
#include "textlabel.H"
#include "x/w/impl/child_element.H"
#include "label_elementfwd.H"
#include "x/w/impl/focus/focusable_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN focusable_labelObj::implObj
	: public focusable_elementObj<label_elementObj<child_elementObj>> {

	//! Alias

	typedef focusable_elementObj<label_elementObj<child_elementObj>
				     > superclass_t;

 public:

	//! Constructor

	implObj(const container_impl &container,
		const text_param &text,
		textlabel_config &config);

	//! Destructor
	~implObj();

	//! Implement uses_input_method()

	bool uses_input_method() override;

	//! Override keyboard_focus()

	//! Call report_current_cursor_position_if_active().

	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger) override;

	//! Override process_updated_position()

	//! Call report_current_cursor_position_if_active().

	void process_updated_position(ONLY IN_THREAD) override;

	//! report_current_cursor_position() when we have input focus.

	void report_current_cursor_position_if_active(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
