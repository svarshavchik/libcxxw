/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

///////////////////////////////////////////////////////////////////////////
//
// Logic for handling element focus changes.
//
// This logic handles both keyboard and pointer focus. Both of them work
// on the same principle: the display element with the input/pointer focus
// is identified, and this logic generates calls to reporting function for all
// affected elements.
//
// The focus_reporter_t parameter is either &report_keyboard_focus or
// &report_pointer_focus.
//
// When an element gains input focus, its request_focus() gets called, passing
// it the element that currently receives input focus, which may be NULL if
// no element currently receives input focus.
//
// To remove input focus from an existing element without moving the focus
// to another element, call the existing element's requested_focus_from() and
// leaving_focus() with a null ptr, in that order. This is done by calling
// lose_focus().
//
// Result:
//
// Calls to report_<x>_focus() from all display elements starting with the
// element that's losing focus up to but not including the common parent
// of the loser, and the element with the new focus.
//
// Calls to report_<x>_focus() from all display elements starting with, but not
// including the common parent and the element that gains the input focus.

void elementObj::implObj::request_focus(IN_THREAD_ONLY,
					const ptr<elementObj::implObj>
					&focus_from,
					focus_reporter_t focus_reporter)
{
	auto me=ptr<elementObj::implObj>(this);

	if (focus_from && focus_from == me)
		return; // No-op

	requested_focus_to(IN_THREAD, focus_from);
	if (focus_from)
		focus_from->leaving_focus(IN_THREAD, me, focus_reporter);
	entering_focus(IN_THREAD, focus_from, focus_reporter);


	focus_movement_complete(IN_THREAD, true, focus_reporter);

	if (focus_from)
		focus_from->focus_movement_complete(IN_THREAD, false,
						    focus_reporter);
}

// Part 1: clear "original_focus" on all display elements starting with
// the display element receiving input focus up, to the top element,
// then invoke requested_focus_from() for the element that's losing focus,
// then set "new_focus" on all display elements starting with the display
// element receiving focus, up to the top element.


// This must be a top level element. If this was a child element, this gets
// overridden by child_elementObj.

void elementObj::implObj::requested_focus_to(IN_THREAD_ONLY,
					     const ptr<elementObj::implObj> &focus_from)
{
	original_focus=false;

	if (focus_from)
		focus_from->requested_focus_from(IN_THREAD);
	new_focus=true;
}

void child_elementObj::requested_focus_to(IN_THREAD_ONLY,
					  const ptr<elementObj::implObj> &focus_from)
{
	original_focus=false;

	child_container->get_element_impl().requested_focus_to(IN_THREAD,
							       focus_from);
	new_focus=true;
}

// Part 2: set "original_focus" and clear "new_focus" for all display
// elements starting with the one that's losing input focus, up to the
// top level display elements.
//
// Note that "new_focus" gets cleared before Part 1 sets it for all elements
// in the new input focus's chain.

void elementObj::implObj::requested_focus_from(IN_THREAD_ONLY)
{
	original_focus=true;
	new_focus=false;
}

void child_elementObj::requested_focus_from(IN_THREAD_ONLY)
{
	original_focus=true;
	new_focus=false;
	child_container->get_element_impl().requested_focus_from(IN_THREAD);
}

// Part 3:
//
// For each display element starting with the display element that's losing
// focus, and up to the top level display element, if new_focus is not set
// call focus_lost().

void elementObj::implObj::leaving_focus(IN_THREAD_ONLY,
					const ptr<elementObj::implObj> &leaving_for,
					focus_reporter_t focus_reporter)
{
	focus_change e=focus_change::lost;

	do_leaving_focus(IN_THREAD, e, ref<elementObj::implObj>(this),
			 leaving_for, focus_reporter);
}

void elementObj::implObj::do_leaving_focus(IN_THREAD_ONLY,
					   focus_change &event,
					   const ref<elementObj::implObj> &element,
					   const ptr<elementObj::implObj> &leaving_for,
					   focus_reporter_t focus_reporter)
{
	if (leaving_for &&
	    leaving_for == ptr<elementObj::implObj>(this))
	{
		((*this).*focus_reporter)(IN_THREAD,
					  focus_change::gained_from_child);
		return;
	}
	if (new_focus && event != focus_change::lost)
	{
		((*this).*focus_reporter)(IN_THREAD,
					  focus_change::child_moved);
		return;
	}
	((*this).*focus_reporter)(IN_THREAD, event);
	event=focus_change::child_lost;
}

void child_elementObj::do_leaving_focus(IN_THREAD_ONLY,
					focus_change &event,
					const ref<elementObj::implObj> &element,
					const ptr<elementObj::implObj> &leaving_for,
					focus_reporter_t focus_reporter)
{
	// focus_lost() gets called before we recursively go to the parent,
	// so the loser's focus_lost() gets called before its parents'.

	elementObj::implObj::do_leaving_focus(IN_THREAD, event, element,
					      leaving_for, focus_reporter);
	child_container->get_element_impl().do_leaving_focus(IN_THREAD, event,
							     element,
							     leaving_for,
							     focus_reporter);
}

// Part 4: for each element starting with the top level display element,
// until the element that's gaining focus, call focus_gained() if original_focus
// is not set.

void elementObj::implObj::entering_focus(IN_THREAD_ONLY,
					 const ptr<elementObj::implObj>
					 &focus_from,
					 focus_reporter_t focus_reporter)
{
	do_entering_focus(IN_THREAD, focus_change::gained,
			  ref<elementObj::implObj>(this),
			  focus_from, focus_reporter);
}

void child_elementObj::do_entering_focus(IN_THREAD_ONLY,
					 focus_change event,
					 const ref<elementObj::implObj> &element,
					 const ptr<elementObj::implObj> &focus_from,
					 focus_reporter_t focus_reporter)
{
	// Recurse to parent first, before calling focus_gained(), so the
	// display element that's receiving input focus gets called last.

	child_container->get_element_impl()
		.do_entering_focus(IN_THREAD,
				   focus_change::child_gained,
				   element, focus_from,
				   focus_reporter);
	elementObj::implObj::do_entering_focus(IN_THREAD, event, element,
					       focus_from, focus_reporter);
}

void elementObj::implObj::do_entering_focus(IN_THREAD_ONLY,
					    focus_change event,
					    const ref<elementObj::implObj> &element,
					    const ptr<elementObj::implObj> &focus_from,
					    focus_reporter_t focus_reporter)
{
	if (focus_from && focus_from == ptr<elementObj::implObj>(this))
	{
		((*this).*focus_reporter)(IN_THREAD,
					  focus_change::lost_to_child);
	}
	else if (original_focus && event != focus_change::gained)
	{
		((*this).*focus_reporter)(IN_THREAD,
					  focus_change::child_moved);
	}
	else
	{
		((*this).*focus_reporter)(IN_THREAD, event);
	}
}

void elementObj::implObj::focus_movement_complete(IN_THREAD_ONLY,
						  bool stop_at_original_focus,
						  focus_reporter_t focus_reporter)
{
	if (stop_at_original_focus && original_focus)
		return;

	(this->*focus_reporter)(IN_THREAD,
				focus_change::focus_movement_complete);
}

void child_elementObj::focus_movement_complete(IN_THREAD_ONLY,
					       bool stop_at_original_focus,
					       focus_reporter_t focus_reporter)
{
	elementObj::implObj::focus_movement_complete(IN_THREAD,
						     stop_at_original_focus,
						     focus_reporter);
	child_container->get_element_impl()
		.focus_movement_complete(IN_THREAD,
					 stop_at_original_focus,
					 focus_reporter);
}

LIBCXXW_NAMESPACE_END
