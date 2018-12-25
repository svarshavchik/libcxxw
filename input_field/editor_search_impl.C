/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/editor_search_impl.H"
#include "input_field/input_field_search.H"
#include "input_field/input_field_search_thread.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtextiterator.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

editor_search_implObj::editor_search_implObj(init_args &args,
					     const ref<input_field_searchObj>
					     &search_container)
	: superclass_t{args},
	  input_field_search_threadObj{args.config.input_field_search_callback},
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

		if (text_size <= 1) // Empty input field
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
	if (new_state.enabled_cursor_at_end)
	{
		// Initiate a search.
		search_request(IN_THREAD);
	}
	else
	{
		if (current_keyboard_focus(IN_THREAD))
		{
			// Abort any current search.
			search_abort(IN_THREAD);
		}
		else
		{
			// We lost the keyboard focus. Don't merely abort
			// any search in progress, stop the entire thread.
			search_thread_request_stop(IN_THREAD);
		}

		// Because we use the public API below to show() the popup,
		// we also must use the public API even though we're IN_THREAD.
		search_container->my_popup->hide();
	}
}


std::u32string editor_search_implObj::get_search_string(ONLY IN_THREAD)
{
	return get();
}

void editor_search_implObj
::search_executed(const search_thread_info &info,
		  const search_thread_results &mcguffin)
{
	search_container->my_popup->in_thread
		([me=ref{this}, info, mcguffin]
		 (ONLY IN_THREAD)
		 {
			 me->search_completed(IN_THREAD, info, mcguffin);
		 });
}

void editor_search_implObj
::search_results(ONLY IN_THREAD,
		 const std::vector<std::u32string> &search_result_text,
		 const std::vector<text_param> &search_result_items)
{
	// Use the public API to update the popup and make it visible.
	listlayoutmanager lm=search_container->my_popup->get_layoutmanager();

	lm->replace_all_items(std::vector<list_item_param>{
			search_result_items.begin(),
				search_result_items.end()});

	search_container->my_popup->show_all();
}

void editor_search_implObj::search_exception_message(const exception &e)
{
	exception_message(e);
}

void editor_search_implObj::search_stop_message(const text_param &t)
{
	stop_message(t);
}

LIBCXXW_NAMESPACE_END
