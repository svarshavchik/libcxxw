/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_autofocus_h
#define popup_autofocus_h

#include "popup/popup_autofocusfwd.H"
#include "focus/focusable.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

template<typename popup_handler>
popup_autofocusObj<popup_handler>::~popup_autofocusObj()=default;

template<typename popup_handler>
void popup_autofocusObj<popup_handler>
::inherited_visibility_updated(IN_THREAD_ONLY,
			       inherited_visibility_info &info)
{
	if (!info.flag)
	{
		auto p=parent_element_window(IN_THREAD).getptr();

		if (p)
		{
			auto &h=p->get_window_handler();

			if (h.data(IN_THREAD).inherited_visibility)
				h.set_input_focus(IN_THREAD);
		}
	}

	popup_handler::inherited_visibility_updated(IN_THREAD, info);

	if (info.flag)
	{
		this->set_input_focus(IN_THREAD);
		if (!this->most_recent_keyboard_focus(IN_THREAD))
		{
			for (const auto &element:this->focusable_fields(IN_THREAD))
				if (element->enabled(IN_THREAD))
					element->set_focus_and_ensure_visibility(IN_THREAD);
		}
	}
}

LIBCXXW_NAMESPACE_END

#endif
