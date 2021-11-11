/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/editor_search_impl.H"
#include "input_field/input_field_search.H"
#include "input_field/input_field_search_popup_handler.H"
#include "input_field/input_field_search_thread.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/richtext/richtextiterator.H"
#include "popup/popup.H"
#include <sstream>
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

editor_search_implObj::editor_search_implObj(init_args &args,
					     const ref<input_field_searchObj>
					     &search_container)
	: superclass_t{args},
	  input_field_search_threadObj{
		  args.config.input_field_search.value()
	  },
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

	// Upon gaining focus, do not kick off a search right away. Require
	// at least some key to be pressed. If had a search popup opened,
	// cursor-down-ed into it, then hit escape closing it, we regain
	// input focus and don't want to reopen the same popup!
	if (new_state.enabled_cursor_at_end)
		return;

	request_or_abort_search(IN_THREAD, new_state);
}

bool editor_search_implObj::process_key_event(ONLY IN_THREAD,
					      const key_event &ke)
{
	// Cursor down enables the popup.

	if (ke.keypress && ke.notspecial() &&
	    search_container->popup_handler->data(IN_THREAD)
	    .requested_visibility &&
	    !search_container->popup_handler->search_popup_activated(IN_THREAD))
		switch (ke.keysym) {
		case XK_Down:
		case XK_KP_Down:
			search_container->popup_handler
				->search_popup_activated(IN_THREAD)=true;

			// Abort any search in progress, so that it
			// doesn't update the popup and sweep the rug
			// from under our fee.
			search_abort(IN_THREAD);

			search_container->popup_handler
				->set_default_focus(IN_THREAD, next_key{});
			search_container->popup_handler
				->handle_key_event(IN_THREAD, ke);

			// Unset our keyboard focus (stop blinking).
			// When the popup gets closed, we'll get it
			// back because we're autorestorable_focusable.
			//
			// Preserve the current validation status of
			// the input field's contents. We don't want
			// to trigger validation as a result of losing
			// input focus.

			auto status=validation_required(IN_THREAD);
			validation_required(IN_THREAD)=false;

			get_window_handler()
				.unset_keyboard_focus(IN_THREAD, &ke);

			// Restore it.
			validation_required(IN_THREAD)=status;

			return true;
		}


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
				size_t cursor_pos, size_t selection_pos,
				const callback_trigger_t &trigger)
{
	superclass_t::set(IN_THREAD, string, cursor_pos, selection_pos,
			  trigger);

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

		// If the search popup has been activated for keyboard entry,
		// don't close it here.
		//
		// input_field_search_popup_handlerObj will clear this flag
		// in set_inherited_visibility_unmapped.

		if (search_container->popup_handler
		    ->search_popup_activated(IN_THREAD))
			return;

		// Because we use the public API below to show() the popup,
		// we also must use the public API even though we're IN_THREAD.
		search_container->my_popup->hide();

		most_recent_search_results(IN_THREAD).clear();
		// Recycle memory.
	}
}


std::u32string editor_search_implObj::get_search_string(ONLY IN_THREAD)
{
	// Retrieve the contents in the search format.

	return get(search_info(IN_THREAD).search_format);
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
		 const std::vector<list_item_param> &search_result_items)
{
	// Use the public API to update the popup and make it visible.
	listlayoutmanager lm=search_container->my_popup->get_layoutmanager();

	lm->replace_all_items(search_result_items);

	text_state current_state{IN_THREAD, *this};

	if (search_result_items.empty() || !current_state.enabled_cursor_at_end)
	{
		most_recent_search_results(IN_THREAD).clear();
		search_container->my_popup->hide();
	}
	else
	{
		most_recent_search_results(IN_THREAD)=search_result_text;
		search_container->my_popup->show_all();
	}
}

void editor_search_implObj
::selected(ONLY IN_THREAD, size_t item_number,
	   const callback_trigger_t &trigger)
{
	if (item_number >= most_recent_search_results(IN_THREAD).size())
	{
		std::ostringstream o;

		o << "There were only "
		  << most_recent_search_results(IN_THREAD).size()
		  << " search results that\nwere returned by the search"
			" function.";
		search_stop_message(o.str());
		return;
	}

	const auto &s=most_recent_search_results(IN_THREAD)[item_number];

	// We override set(), so must go straight to the horse's mouth.
	editor_implObj::set(IN_THREAD, s, s.size(), s.size(), trigger);

	validation_required(IN_THREAD)=true;
	validate_modified(IN_THREAD, trigger);
}

void editor_search_implObj::search_exception_message(const exception &e)
{
	exception_message(e);
}

void editor_search_implObj::search_stop_message(const text_param &t)
{
	stop_message(t);
}

void editor_search_implObj
::on_search(ONLY IN_THREAD,
	    const input_field_config::search_info &arg)
{
	search_info(IN_THREAD)=arg;
}

LIBCXXW_NAMESPACE_END
