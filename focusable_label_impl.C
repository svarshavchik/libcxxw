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
	  double widthmm,
	  halign alignment)
	: superclass_t(container, text, alignment, widthmm, true)
{
}

focusable_labelObj::implObj::~implObj()=default;

bool focusable_labelObj::implObj::uses_input_method()
{
	return true;
}

void focusable_labelObj::implObj::keyboard_focus(IN_THREAD_ONLY,
						 const callback_trigger_t &trigger)
{
	superclass_t::keyboard_focus(IN_THREAD, trigger);
	report_current_cursor_position_if_active(IN_THREAD);

	if (current_keyboard_focus(IN_THREAD))
	{
		// If the focus was gained by tabbing into the field, show the
		// first or the last link in the focusable field.
		if (std::holds_alternative<next_key>(trigger))
			first_hotspot(IN_THREAD);
		else if (std::holds_alternative<prev_key>(trigger))
			last_hotspot(IN_THREAD);
	}
	else
	{
		hotspot_unhighlight(IN_THREAD);
	}
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
