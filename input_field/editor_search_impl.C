/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/editor_search_impl.H"
#include "input_field/input_field_search.H"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtextiterator.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

editor_search_implObj::editor_search_implObj(init_args &args,
					     const ref<input_field_searchObj>
					     &search_container)
	: superclass_t{args},
	  search_container{search_container}
{
}

editor_search_implObj::~editor_search_implObj()=default;

// We search when the editor element has keyboard focus, and the cursor
// is at the end of the input field.

struct editor_search_implObj::text_state {

	// In some cases we want to make sure that the text has changed,
	// capturing the current text size is sufficient.
	size_t text_size;

	// Flag: editor element is enabled and cursor is at the end of the
	// input field.
	bool enabled_cursor_at_end;

	text_state(ONLY IN_THREAD, editor_search_implObj &me)
		: text_size{me.text->size(IN_THREAD)},
		  enabled_cursor_at_end{false}
	{
		if (!me.current_keyboard_focus(IN_THREAD))
			return;

		if (selection_cursor_t::lock{IN_THREAD, me, true}.cursor)
			return;

		if (me.cursor->pos()+1 != text_size)
			return;

		enabled_cursor_at_end=true;
	}
};

void editor_search_implObj::keyboard_focus(ONLY IN_THREAD,
					   const callback_trigger_t &trigger)
{
	superclass_t::keyboard_focus(IN_THREAD, trigger);

	text_state new_state{IN_THREAD, *this};

	request_or_abort_search(IN_THREAD, new_state);
}

bool editor_search_implObj::process_key_event(ONLY IN_THREAD,
					      const key_event &ke)
{
	text_state old_state{IN_THREAD, *this};

	bool flag=superclass_t::process_key_event(IN_THREAD, ke);

	text_state new_state{IN_THREAD, *this};

	// Make sure that something has changed. If the text size has not
	// changed, and the eligibility did not change, then ignore this
	// key event (it was a no-op).

	if (old_state.text_size != new_state.text_size ||
	    old_state.enabled_cursor_at_end != new_state.enabled_cursor_at_end)
	{
		request_or_abort_search(IN_THREAD, new_state);
	}

	return flag;
}

bool editor_search_implObj::process_button_event(ONLY IN_THREAD,
						 const button_event &be,
						 xcb_timestamp_t timestamp)
{
	bool flag=superclass_t::process_button_event(IN_THREAD, be, timestamp);

	text_state new_state{IN_THREAD, *this};

	request_or_abort_search(IN_THREAD, new_state);

	return flag;
}

void editor_search_implObj::set(ONLY IN_THREAD, const std::u32string &string,
				size_t cursor_pos, size_t selection_pos)
{
	superclass_t::set(IN_THREAD, string, cursor_pos, selection_pos);

	text_state new_state{IN_THREAD, *this};

	request_or_abort_search(IN_THREAD, new_state);
}

void editor_search_implObj::request_or_abort_search(ONLY IN_THREAD,
						    const text_state &new_state)
{
}

LIBCXXW_NAMESPACE_END
