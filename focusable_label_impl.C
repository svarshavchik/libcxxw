/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focusable_label_impl.H"
#include "label_element.H"
#include "focus/focusable_element.H"

LIBCXXW_NAMESPACE_START

focusable_labelObj::implObj
::implObj(const ref<containerObj::implObj> &container,
	  const text_param &text,
	  halign alignment)
	: superclass_t(container, text, alignment, 0, true)
{
}

focusable_labelObj::implObj::~implObj()=default;

bool focusable_labelObj::implObj::uses_input_method()
{
	return true;
}

void focusable_labelObj::implObj::keyboard_focus(IN_THREAD_ONLY)
{
	superclass_t::keyboard_focus(IN_THREAD);
	report_current_cursor_position_if_active(IN_THREAD);
}

void focusable_labelObj::implObj::current_position_updated(IN_THREAD_ONLY)
{
	superclass_t::current_position_updated(IN_THREAD);
	report_current_cursor_position_if_active(IN_THREAD);
}

void focusable_labelObj::implObj
::report_current_cursor_position_if_active(IN_THREAD_ONLY)
{
	if (!current_keyboard_focus(IN_THREAD))
		return;

	// Pretend that there's one big giant cursor for the entire label.
	auto r=data(IN_THREAD).current_position;

	r.x=0;
	r.y=0;
	report_current_cursor_position(IN_THREAD, r);
}

LIBCXXW_NAMESPACE_END
