/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "element_screen.H"
#include "screen.H"
#include "fonts/current_fontcollection.H"
#include "fonts/fontcollection.H"
#include "focus/focusable_element.H"
#include "richtext/richtext.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtextiterator.H"
#include "richtext/richtext_draw_info.H"
#include "background_color.H"
#include "messages.H"
#include "connection_thread.H"
#include "x/w/key_event.H"
#include <x/weakcapture.H>
#include <X11/keysym.h>
#include <chrono>

LIBCXXW_NAMESPACE_START

static inline richtextstring
create_initial_string(const ref<containerObj::implObj> &container,
		      const text_param &text)
{
	auto &element=container->get_element_impl();

	auto bg_color=element.create_background_color
		("textedit_foreground_color",rgb(0, 0, 0));
	auto font=element.create_theme_font("textedit");

	text_param cpy=text;

	cpy(" ");

	auto string=element.convert({bg_color, font}, cpy);

	if (string.get_meta().size() > 1)
		throw EXCEPTION(_("Input text cannot contain embedded formatting."));

	return string;
}

////////////////////////////////////////////////////////////////////////////

// Helper object constructed on the stack before moving the cursor, and
// destroyed after the cursor is moved.
//
// If the SHIFT key or button #1 is held down, and there is no current
// selection, a new selection is started.
//
// When there's a selection, the starting cursor position is saved, and
// the destructor redraws all fragments between the former cursor position and
// the current one, in order to update the display of selected text.
//
// If the SHIFT key is not held down, and there is a current selection, it
// is removed and the formerly selected text fragments get redrawn, to show
// that the selection has been removed.
//
// If the cursor is actually moved, it is unblink-ed, and then re-blinked
// at the new position.

struct LIBCXX_HIDDEN editorObj::implObj::moving_cursor {

	IN_THREAD_ONLY;
	editorObj::implObj &me;

	bool in_selection=false;
	richtextiterator old_cursor;

	moving_cursor(IN_THREAD_ONLY, editorObj::implObj &me,
		      const input_mask &mask)
		: IN_THREAD(IN_THREAD), me(me), old_cursor(me.cursor->clone())
	{
		if (mask.shift || (mask.buttons & 1))
		{
			if (!me.selection_start(IN_THREAD))
				me.selection_start(IN_THREAD)=
					me.cursor->clone();
		}
		else
		{
			auto p=me.selection_start(IN_THREAD);

			if (p)
			{
				me.selection_start(IN_THREAD)=
					richtextiteratorptr();

				me.draw_between(IN_THREAD, p, me.cursor);
			}
		}

		if (me.selection_start(IN_THREAD))
			in_selection=true;
	}

	~moving_cursor()
	{
		bool blink_needed=false;

		if (me.cursor->compare(old_cursor))
		{
			me.unblink(IN_THREAD, old_cursor);
			blink_needed=true;
		}

		if (in_selection)
		{
			me.draw_between(IN_THREAD,
					old_cursor,
					me.cursor);
		}
		if (blink_needed)
			me.blink(IN_THREAD);
	}
};

////////////////////////////////////////////////////////////////////////////

editorObj::implObj::implObj(const ref<containerObj::implObj> &container,
			    const text_param &text,
			    const input_field_config &config)
	: implObj(container, create_initial_string(container, text), config)
{
}

editorObj::implObj::implObj(const ref<containerObj::implObj> &container,
			    richtextstring &&string,
			    const input_field_config &config)
	: superclass_t(true,
		       container, config.alignment, 0,
		       std::move(string),
		       "textedit@libcxx"),
	  cursor(this->text->end()),

	  // Capture the string's font.
	  font(string.get_meta().at(0).second.getfont()),

	  config(config)
{
	if (config.columns <= 2)
		throw EXCEPTION("Input fields must have at least two columns");

	cursor->my_richtext->read_only_lock
		([]
		 (const auto &impl)
		 {
			 (*impl)->unprintable_char=' ';
		 });
}

editorObj::implObj::~implObj()=default;

void editorObj::implObj::compute_preferred_width(IN_THREAD_ONLY)
{
	preferred_width=config.oneline() ? 0:nominal_width(IN_THREAD);
}

dim_t editorObj::implObj::nominal_width(IN_THREAD_ONLY) const
{
	return dim_t::truncate(config.columns *
			       (dim_t::value_type)
			       font->fc(IN_THREAD)->nominal_width());
}

bool editorObj::implObj::rewrap(IN_THREAD_ONLY)
{
	if (preferred_width == 0)
		return false;

	return text->rewrap(IN_THREAD, preferred_width);
}

std::pair<metrics::axis, metrics::axis>
editorObj::implObj::calculate_current_metrics(IN_THREAD_ONLY)
{
	auto metrics=text->get_metrics(IN_THREAD, preferred_width);

	// If we word-wrap, fixate to the word wrapping width. Otherwise
	// get_metrics() uses the current width as the preferred width, so
	// fixate that.
	auto w=preferred_width;

	if (config.oneline())
		w=metrics.first.preferred();

	metrics.first={w, w, w};

	return metrics;
}

void editorObj::implObj::rewrap_due_to_updated_position(IN_THREAD_ONLY)
{
}

void editorObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	// Although the superclass SHOULD hit out font, don't assume that.

	font->theme_updated(IN_THREAD);
	labelObj::implObj::theme_updated(IN_THREAD);
}

void editorObj::implObj::keyboard_focus(IN_THREAD_ONLY,
					focus_change event,
					const ref<elementObj::implObj> &ptr)
{
	superclass_t::keyboard_focus(IN_THREAD, event, ptr);

	if (!is_enabled(IN_THREAD))
	{
		blinking=nullptr;
		return;
	}

	if (blinking)
		return;

	scroll_cursor_into_view(IN_THREAD);
	schedule_blink(IN_THREAD);
	blinkon=false;
	blink(IN_THREAD);
}

void editorObj::implObj::schedule_blink(IN_THREAD_ONLY)
{
	blinking=get_screen()->impl->thread->schedule_callback
		(IN_THREAD,
		 std::chrono::milliseconds{500},
		 // Don't create a lambda that owns a strong ref to me.
		 // Use a weak pointer.
		 [me=make_weak_capture(ref<implObj>(this))]
		 (IN_THREAD_ONLY)
		 {
			 me.get([&]
				(const auto &me) {
					me->blink(IN_THREAD);
				});
		 });
}

void editorObj::implObj::unblink(IN_THREAD_ONLY)
{
	unblink(IN_THREAD, cursor);
}

void editorObj::implObj::unblink(IN_THREAD_ONLY,
				  const richtextiterator &cursor)
{
	if (blinkon)
		blink(IN_THREAD, cursor);
}

void editorObj::implObj::blink(IN_THREAD_ONLY)
{
	blink(IN_THREAD, cursor);
}

void editorObj::implObj::blink(IN_THREAD_ONLY,
			       const richtextiterator &cursor)
{
	blinkon= !blinkon;

	// We actually blink the cursor only when we are not showing a
	// selection.

	if ( (!selection_start(IN_THREAD)) ||
	     selection_start(IN_THREAD)->compare(cursor) == 0)
		cursor->set_cursor(IN_THREAD, blinkon);
	else
		// We might get here from moving_cursor's destructor. Make
		// sure the old cursor is unblinked.
		cursor->set_cursor(IN_THREAD, false);
	schedule_blink(IN_THREAD);

	// Tell do_draw() to draw only the vertical slice defined by the
	// cursor's position. Combined with redrawing only the modified fragment
	// this effectively redraws just the cursor.
	auto at=cursor->at(IN_THREAD);
	auto current_position=data(IN_THREAD).current_position;

	current_position.x=at.position.x;
	current_position.width=at.position.width;
	current_position.y=0;
	text->redraw_whatsneeded(IN_THREAD, *this,
				 {selection_start(IN_THREAD), cursor},
				 get_draw_info(IN_THREAD),
				 {current_position});
}

bool editorObj::implObj::process_key_event(IN_THREAD_ONLY, const key_event &ke)
{
	if (ke.keypress && ke.notspecial())
	{
		if (process_keypress(IN_THREAD, ke))
		{
			scroll_cursor_into_view(IN_THREAD);
			return true;
		}
	}
	return superclass_t::process_key_event(IN_THREAD, ke);
}

bool editorObj::implObj::process_keypress(IN_THREAD_ONLY, const key_event &ke)
{
	if (ke.ctrl)
		return false;

	switch (ke.keysym) {
	case XK_Left:
	case XK_KP_Left:
		{
			moving_cursor moving{IN_THREAD, *this, ke};
			cursor->prev(IN_THREAD);
		}
		return true;
	case XK_Right:
	case XK_KP_Right:
		{
			moving_cursor moving{IN_THREAD, *this, ke};
			cursor->next(IN_THREAD);
		}
		return true;
	case XK_Up:
	case XK_KP_Up:
		{
			moving_cursor moving{IN_THREAD, *this, ke};
			cursor->up(IN_THREAD);
		}
		return true;
	case XK_Down:
	case XK_KP_Down:
		{
			moving_cursor moving{IN_THREAD, *this, ke};
			cursor->down(IN_THREAD);
		}
		return true;
	case XK_Delete:
	case XK_KP_Delete:
		unblink(IN_THREAD);
		if (selection_start(IN_THREAD))
		{
			delete_selection(IN_THREAD, ke);
		}
		else
		{
			auto clone=cursor->clone();
			clone->next(IN_THREAD);
			cursor->remove(IN_THREAD, clone);
		}
		recalculate(IN_THREAD);
		draw_changes(IN_THREAD);
		blink(IN_THREAD);
		return true;
	case XK_Page_Up:
	case XK_KP_Page_Up:
		unblink(IN_THREAD);
		blink(IN_THREAD);
		return true;
	case XK_Page_Down:
	case XK_KP_Page_Down:
		unblink(IN_THREAD);
		blink(IN_THREAD);
		return true;
	case XK_Home:
		{
			moving_cursor moving{IN_THREAD, *this, ke};
			cursor->start_of_line();
		}
		return true;
	case XK_End:
		{
			moving_cursor moving{IN_THREAD, *this, ke};
			cursor->end_of_line();
		}
		return true;
	case XK_KP_Home:
		unblink(IN_THREAD);
		blink(IN_THREAD);
		return true;
	case XK_KP_End:
		unblink(IN_THREAD);
		blink(IN_THREAD);
		return true;
	case XK_Insert:
		unblink(IN_THREAD);
		blink(IN_THREAD);
		return true;
	}

	if ((!config.oneline() && ke.unicode == '\n') || ke.unicode >= ' ')
	{
		unblink(IN_THREAD);
		if (selection_start(IN_THREAD))
			delete_selection(IN_THREAD, ke);
		cursor->insert(IN_THREAD,
			       std::u32string(&ke.unicode, &ke.unicode+1));
		recalculate(IN_THREAD);
		draw_changes(IN_THREAD);
		blink(IN_THREAD);
		return true;
	}

	if (ke.unicode == '\b')
	{
		unblink(IN_THREAD);
		if (selection_start(IN_THREAD))
			delete_selection(IN_THREAD, ke);
		else
		{
			auto old=cursor->clone();
			cursor->prev(IN_THREAD);
			cursor->remove(IN_THREAD, old);
		}
		recalculate(IN_THREAD);
		draw_changes(IN_THREAD);
		blink(IN_THREAD);
		return true;
	}
	return false;
}

void editorObj::implObj::delete_selection(IN_THREAD_ONLY, const key_event &ke)
{
	cursor->remove(IN_THREAD, selection_start(IN_THREAD));
	selection_start(IN_THREAD)=richtextiteratorptr();
}

void editorObj::implObj::draw_changes(IN_THREAD_ONLY)
{
	text->redraw_whatsneeded(IN_THREAD, *this,
				 {selection_start(IN_THREAD), cursor},
				 get_draw_info(IN_THREAD));
}

void editorObj::implObj::do_draw(IN_THREAD_ONLY,
				 const draw_info &di,
				 const rectangle_set &areas)
{
	text->full_redraw(IN_THREAD, *this,
			     {selection_start(IN_THREAD), cursor},
			  di, areas);
}

void editorObj::implObj::draw_between(IN_THREAD_ONLY,
				      const richtextiterator &a,
				      const richtextiterator &b)
{
	text->redraw_between(IN_THREAD, *this,
			     a, b,
			     {selection_start(IN_THREAD), cursor},
			     get_draw_info(IN_THREAD));
}

void editorObj::implObj::scroll_cursor_into_view(IN_THREAD_ONLY)
{
	ensure_visibility(IN_THREAD, cursor->at(IN_THREAD).position);
}

bool editorObj::implObj::process_button_event(IN_THREAD_ONLY,
					      int button,
					      bool press,
					      const input_mask &mask)
{
	if (press && button == 1)
	{
		moving_cursor moving{IN_THREAD, *this, mask};

		cursor->moveto(IN_THREAD, most_recent_x, most_recent_y);
		grab(IN_THREAD);
	}
	else if (button == 1)
	{
		stop_scrolling(IN_THREAD);
	}

	// We do not consume the button event. The editor container also
	// consumes the button press, and moves the input focus here.
	return superclass_t::process_button_event(IN_THREAD, button, press,
						  mask);
}

void editorObj::implObj::motion_event(IN_THREAD_ONLY, coord_t x, coord_t y,
				      const input_mask &mask)
{
	most_recent_x=x;
	most_recent_y=y;

	if (mask.buttons & 1)
	{
		{
			moving_cursor moving{IN_THREAD, *this, mask};
			cursor->moveto(IN_THREAD, most_recent_x, most_recent_y);
		}

		if (!motion_scroll_callback)
			start_scrolling(IN_THREAD);
	}
}

void editorObj::implObj::start_scrolling(IN_THREAD_ONLY)
{
	motion_scroll_callback=get_screen()->impl->thread->schedule_callback
		(IN_THREAD,
		 std::chrono::milliseconds{250},
		 // Don't create a lambda that owns a strong ref to me.
		 // Use a weak pointer.
		 [me=make_weak_capture(ref<implObj>(this))]
		 (IN_THREAD_ONLY)
		 {
			 me.get([&]
				(const auto &me) {
					me->scroll(IN_THREAD);
				});
		 });
}

void editorObj::implObj::scroll(IN_THREAD_ONLY)
{
	stop_scrolling(IN_THREAD);
	// In order to scroll_cursor_into_view, that's it.
	//
	// If motion_event()s continue, the next one will start_scrolling()
	// again.
}

void editorObj::implObj::stop_scrolling(IN_THREAD_ONLY)
{
	motion_scroll_callback=nullptr;
	scroll_cursor_into_view(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
