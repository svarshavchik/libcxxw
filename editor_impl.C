/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "element_screen.H"
#include "reference_font_element.H"
#include "screen.H"
#include "draw_info.H"
#include "fonts/current_fontcollection.H"
#include "fonts/fontcollection.H"
#include "focus/focusable_element.H"
#include "label_element.H"
#include "richtext/richtext.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtextiterator.H"
#include "richtext/richtext_draw_info.H"
#include "selection/current_selection.H"
#include "background_color.H"
#include "messages.H"
#include "connection_thread.H"
#include "generic_window_handler.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include <x/vector.H>
#include <x/weakcapture.H>
#include <X11/keysym.h>
#include <courier-unicode.h>
#include <chrono>
#include <iterator>

LIBCXXW_NAMESPACE_START

static inline std::tuple<richtextmeta, richtextstring>
create_initial_string(const ref<containerObj::implObj> &container,
		      const text_param &text)
{
	auto &element=container->get_element_impl();

	auto bg_color=element.create_background_color
		("textedit_foreground_color");
	auto font=element.create_theme_font("textedit");

	text_param cpy=text;

	cpy(" ");

	richtextmeta default_meta{bg_color, font};

	auto string=element.create_richtextstring(default_meta, cpy);

	if (string.get_meta().size() > 1)
		throw EXCEPTION(_("Input text cannot contain embedded formatting."));

	return {default_meta, string};
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
		: moving_cursor(IN_THREAD, me,
				mask.shift || (mask.buttons & 1), false)
	{
	}

	moving_cursor(IN_THREAD_ONLY, editorObj::implObj &me,
		      bool selection_in_progress,
		      bool processing_clear)
		: IN_THREAD(IN_THREAD), me(me), old_cursor(me.cursor->clone())
	{
		// Make sure to turn off the blink, before
		// starting a selection.
		me.unblink(IN_THREAD);

		if (selection_in_progress)
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
				// Remove the highlighted selection.

				me.selection_start(IN_THREAD)=
					richtextiteratorptr();

				me.draw_between(IN_THREAD, p, me.cursor);

				// processing_clear is set when clear()ing
				// the current selection in response to
				// someone else installing a new selection.
				//
				// Otherwise, we are removing the selection
				// ourselves, and need to announce it.
				if (!processing_clear)
					me.remove_primary_selection(IN_THREAD);
			}
		}

		if (me.selection_start(IN_THREAD))
			in_selection=true;
	}

	~moving_cursor()
	{
		if (in_selection)
		{
			me.draw_between(IN_THREAD,
					old_cursor,
					me.cursor);
		}

		if (me.current_keyboard_focus(IN_THREAD))
			me.blink(IN_THREAD);
	}
};

////////////////////////////////////////////////////////////////////////////

// Implements current_selectionObj for editor's selected/cut text.

class editorObj::implObj::selectionObj : public current_selectionObj {

	// Indicates a removed selection.
	bool valid_flag_thread_only=true;

	// My editor object. Must be weakly captured to avoid circular refs.

	weakptr<ptr<implObj>> me;

	// Cut text
	std::u32string cut_text;

public:
	THREAD_DATA_ONLY(valid_flag);

	selectionObj(xcb_timestamp_t timestamp, const ref<implObj> &me,
		     const richtextiterator &a,
		     const richtextiterator &b);

	~selectionObj()=default;

	bool stillvalid(IN_THREAD_ONLY) override;

	void clear(IN_THREAD_ONLY) override;

	ptr<convertedValueObj> convert(IN_THREAD_ONLY, xcb_atom_t type)
		override;

	std::vector<xcb_atom_t> supported(IN_THREAD_ONLY);
};

////////////////////////////////////////////////////////////////////////////

editorObj::implObj::implObj(const ref<editor_peephole_implObj> &parent_peephole,
			    const text_param &text,
			    const input_field_config &config)
	: implObj(parent_peephole,
		  create_initial_string(parent_peephole, text), config)
{
}

editorObj::implObj::implObj(const ref<editor_peephole_implObj> &parent_peephole,
			    std::tuple<richtextmeta, richtextstring>
			    &&meta_and_string,
			    const input_field_config &config)
	: superclass_t(// Capture the string's font.
		       std::get<richtextstring>(meta_and_string)
		       .get_meta().at(0).second.getfont(),
		       parent_peephole, config.alignment, 0,
		       std::get<richtextstring>(meta_and_string),
		       std::get<richtextmeta>(meta_and_string),
		       "textedit@libcxx"),
	  cursor(this->text->end()),
	  parent_peephole(parent_peephole),
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

void editorObj::implObj::initialize(IN_THREAD_ONLY)
{
	superclass_t::initialize(IN_THREAD);
	parent_peephole->recalculate(IN_THREAD, *this);
}

void editorObj::implObj::theme_updated(IN_THREAD_ONLY,
				       const defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	parent_peephole->recalculate(IN_THREAD, *this);
}

void editorObj::implObj::compute_preferred_width(IN_THREAD_ONLY)
{
	preferred_width=config.oneline() ? 0:nominal_width(IN_THREAD);
}

dim_t editorObj::implObj::nominal_width(IN_THREAD_ONLY) const
{
	return dim_t::truncate(config.columns *
			       (dim_t::value_type)
			       font_nominal_width(IN_THREAD));
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
	auto metrics=text->get_metrics(IN_THREAD, preferred_width, true);

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

void editorObj::implObj::keyboard_focus(IN_THREAD_ONLY)
{
	superclass_t::keyboard_focus(IN_THREAD);

	blink_if_has_focus(IN_THREAD);
}

void editorObj::implObj::window_focus_change(IN_THREAD_ONLY, bool flag)
{
	blink_if_has_focus(IN_THREAD);
	superclass_t::window_focus_change(IN_THREAD, flag);
}

void editorObj::implObj::blink_if_has_focus(IN_THREAD_ONLY)
{
	if (current_keyboard_focus(IN_THREAD))
	{
		if (!blinking)
			blink(IN_THREAD);
		scroll_cursor_into_view(IN_THREAD);
	}
	else
	{
		unblink(IN_THREAD);
		blinking=nullptr;
	}
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
	// We actually blink the cursor only when we are not showing a
	// selection.

	if ( (!selection_start(IN_THREAD)) ||
	     selection_start(IN_THREAD)->compare(cursor) == 0)
	{
		blinkon= !blinkon;

		cursor->set_cursor(IN_THREAD, blinkon);
		schedule_blink(IN_THREAD);
	}
	else
	{
		blinking=nullptr;
		return;
	}

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

	if (!ke.keypress)
		switch (ke.keysym) {
		case XK_Shift_L:
		case XK_Shift_R:
			create_primary_selection(IN_THREAD);
		}

	return superclass_t::process_key_event(IN_THREAD, ke);
}

bool editorObj::implObj::process_keypress(IN_THREAD_ONLY, const key_event &ke)
{
	if (ke.ctrl)
	{
		if (ke.keysym == XK_Insert)
		{
			create_secondary_selection(IN_THREAD);
			return true;
		}

		if (ke.unicode == 1) // CTRL-A
		{
			select_all(IN_THREAD);
			return true;
		}

		if (ke.unicode == ' ')
		{
			get_window_handler()
				.paste(IN_THREAD, XCB_ATOM_PRIMARY,
				       IN_THREAD->timestamp(IN_THREAD));
		}

		return false;
	}

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
		delete_char_or_selection(IN_THREAD, ke);
		recalculate(IN_THREAD);
		draw_changes(IN_THREAD);
		blink(IN_THREAD);
		return true;
	case XK_Page_Up:
	case XK_KP_Page_Up:
		{
			moving_cursor moving{IN_THREAD, *this, ke};
			cursor->page_up(IN_THREAD,
					bounds(get_draw_info(IN_THREAD)
					       .element_viewport).height);
		}
		return true;
	case XK_Page_Down:
	case XK_KP_Page_Down:
		{
			moving_cursor moving{IN_THREAD, *this, ke};
			cursor->page_down(IN_THREAD,
					  bounds(get_draw_info(IN_THREAD)
						 .element_viewport).height);
		}
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
		to_begin(IN_THREAD, ke);
		return true;
	case XK_KP_End:
		to_end(IN_THREAD, ke);
		return true;
	case XK_Insert:
		if (ke.shift)
			get_window_handler()
				.paste(IN_THREAD,
				       XCB_ATOM_SECONDARY,
				       IN_THREAD->timestamp(IN_THREAD));
		return true;
	}

	if ((!config.oneline() && ke.unicode == '\n') || ke.unicode >= ' ')
	{
		insert(IN_THREAD, {&ke.unicode, 1});
		return true;
	}

	if (ke.unicode == '\b')
	{
		unblink(IN_THREAD);
		if (selection_start(IN_THREAD))
			delete_selection(IN_THREAD);
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

bool editorObj::implObj::uses_input_method()
{
	return true;
}

bool editorObj::implObj::pasted(IN_THREAD_ONLY,
				const std::u32string_view &str)
{
	insert(IN_THREAD, str);
	scroll_cursor_into_view(IN_THREAD);
	return true;
}

void editorObj::implObj::insert(IN_THREAD_ONLY,
				const std::u32string_view &str)
{
	unblink(IN_THREAD);

	if (selection_start(IN_THREAD))
		delete_selection(IN_THREAD);
	cursor->insert(IN_THREAD, str);
	recalculate(IN_THREAD);
	draw_changes(IN_THREAD);
	blink(IN_THREAD);
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
#ifdef EDITOR_DRAW
	EDITOR_DRAW();
#endif
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

void editorObj::implObj::set_focus_and_ensure_visibility(IN_THREAD_ONLY)
{
	set_focus_only(IN_THREAD);
}

void editorObj::implObj::scroll_cursor_into_view(IN_THREAD_ONLY)
{
	auto pos=cursor->at(IN_THREAD).position;

	ensure_visibility(IN_THREAD, pos);
	report_current_cursor_position(IN_THREAD, pos);
}

bool editorObj::implObj::process_button_event(IN_THREAD_ONLY,
					      const button_event &be,
					      xcb_timestamp_t timestamp)
{
	if (be.press && (be.button == 1 || be.button == 2))
	{
		moving_cursor moving{IN_THREAD, *this, be};

		cursor->moveto(IN_THREAD, most_recent_x, most_recent_y);

		// We grab the pointer while the button is held down.
		grab(IN_THREAD);

		if (be.button == 2)
			get_window_handler().paste(IN_THREAD, XCB_ATOM_PRIMARY,
						   timestamp);
	}
	else if (be.button == 1)
	{
		stop_scrolling(IN_THREAD);
		create_primary_selection(IN_THREAD);
	}

	// We do not consume the button event. The peephole with this editor
	// element also consumes the button press, and moves the input focus
	// here, see editor_peephole_implObj.

	return superclass_t::process_button_event(IN_THREAD, be, timestamp);
}

void editorObj::implObj::report_motion_event(IN_THREAD_ONLY,
					     const motion_event &me)
{
	superclass_t::report_motion_event(IN_THREAD, me);

	most_recent_x=me.x;
	most_recent_y=me.y;

	if (me.mask.buttons & 1)
	{
		{
			moving_cursor moving{IN_THREAD, *this, me.mask};
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

void editorObj::implObj::removed(IN_THREAD_ONLY)
{
	// When this element is removed from its container, remove all the
	// selections, too...

	remove_primary_selection(IN_THREAD);
	remove_secondary_selection(IN_THREAD);
}

/////////////////////////////////////////////////////////////////////////////
//
// Selections

editorObj::implObj::selectionObj::
selectionObj(xcb_timestamp_t timestamp, const ref<implObj> &me,
	     const richtextiterator &a,
	     const richtextiterator &b)
	: current_selectionObj(timestamp), me(me),
	  cut_text{a->get(b).get_string()}
{
}

bool editorObj::implObj::selectionObj::stillvalid(IN_THREAD_ONLY)
{
	return valid_flag(IN_THREAD);
}

void editorObj::implObj::selectionObj::clear(IN_THREAD_ONLY)
{
	auto p=me.getptr();

	if (p)
	{
		// Leverage moving_cursor for all the heavy lifting.
		//
		// We pretend that we're moving the cursor without
		// a selection in progress, that's all.
		moving_cursor dummy{IN_THREAD, *p, false, true};

		p->primary_selection(IN_THREAD)=nullptr;
	}
}

std::vector<xcb_atom_t> editorObj::implObj::selectionObj
::supported(IN_THREAD_ONLY)
{
	std::vector<xcb_atom_t> v;

	auto p=me.getptr();

	if (p)
	{
		const auto &atoms=IN_THREAD->info->atoms_info;

		v.push_back(atoms.utf8_string);
		v.push_back(atoms.string);
	}

	return v;
}

// Somebody is asking for our selection.

ptr<current_selectionObj::convertedValueObj> editorObj::implObj::selectionObj
::convert(IN_THREAD_ONLY, xcb_atom_t type)
{
	const char *charset=nullptr;

	auto p=me.getptr();

	if (p)
	{
		const auto &atoms=p->get_screen()->impl->thread
			->info->atoms_info;

		if (type == atoms.string)
			charset=unicode::iso_8859_1;
		if (type == atoms.utf8_string)
			charset=unicode::utf_8;
	}

	if (!charset)
		return ptr<convertedValueObj>();

	auto bytes=vector<uint8_t>::create();

	bytes->reserve(cut_text.size()*2);

	bool ignore;

	unicode::iconvert::fromu::convert(cut_text.begin(),
					  cut_text.end(),
					  charset,
					  std::back_insert_iterator<std::vector
					  <uint8_t>>(*bytes), ignore);

	return ref<current_selectionObj::convertedValueObj>
		::create(type, 8, bytes);
}

void editorObj::implObj::delete_selection(IN_THREAD_ONLY)
{
	cursor->remove(IN_THREAD, selection_start(IN_THREAD));
	selection_start(IN_THREAD)=richtextiteratorptr();
	remove_primary_selection(IN_THREAD);
}

editorObj::implObj::selection
editorObj::implObj::create_selection(IN_THREAD_ONLY)
{
	return selection::create(get_screen()->impl->thread
				 ->timestamp(IN_THREAD),
				 ref<implObj>(this),
				 cursor,
				 selection_start(IN_THREAD));
}

void editorObj::implObj::create_primary_selection(IN_THREAD_ONLY)
{
	if (!selection_start(IN_THREAD))
		return;

	if (selection_start(IN_THREAD)->compare(cursor) == 0)
		return; // Nothing selected, really.
	// Remove the previous one.

	remove_primary_selection(IN_THREAD);

	auto s=create_selection(IN_THREAD);
	primary_selection(IN_THREAD)=s;
	get_window_handler().selection_announce(IN_THREAD, XCB_ATOM_PRIMARY, s);
}

void editorObj::implObj::create_secondary_selection(IN_THREAD_ONLY)
{
	if (!selection_start(IN_THREAD))
		return;

	if (selection_start(IN_THREAD)->compare(cursor) == 0)
		return; // Nothing selected, really.
	// Remove the previous one.

	remove_secondary_selection(IN_THREAD);

	auto s=create_selection(IN_THREAD);
	secondary_selection(IN_THREAD)=s;
	get_window_handler().selection_announce(IN_THREAD, XCB_ATOM_SECONDARY,
						s);
}

void editorObj::implObj::remove_primary_selection(IN_THREAD_ONLY)
{
	if (!primary_selection(IN_THREAD))
		return;

	primary_selection(IN_THREAD)->valid_flag(IN_THREAD)=false;
	primary_selection(IN_THREAD)=nullptr;

	get_window_handler().selection_discard(IN_THREAD, XCB_ATOM_PRIMARY);
}

void editorObj::implObj::remove_secondary_selection(IN_THREAD_ONLY)
{
	if (!secondary_selection(IN_THREAD))
		return;

	secondary_selection(IN_THREAD)->valid_flag(IN_THREAD)=false;
	secondary_selection(IN_THREAD)=nullptr;

	get_window_handler().selection_discard(IN_THREAD, XCB_ATOM_SECONDARY);
}

void editorObj::implObj::to_begin(IN_THREAD_ONLY, const input_mask &mask)
{
	moving_cursor moving{IN_THREAD, *this, mask};

	cursor->swap(cursor->begin());
}

void editorObj::implObj::to_end(IN_THREAD_ONLY, const input_mask &mask)
{
	moving_cursor moving{IN_THREAD, *this, mask};

	cursor->swap(cursor->end());
}

void editorObj::implObj::select_all(IN_THREAD_ONLY)
{
	input_mask mask;

	to_begin(IN_THREAD, mask);

	mask.shift=true;

	to_end(IN_THREAD, mask);
}

void editorObj::implObj::delete_char_or_selection(IN_THREAD_ONLY,
						  const input_mask &mask)
{
	if (selection_start(IN_THREAD))
	{
		if (mask.shift)
			create_secondary_selection(IN_THREAD);
		delete_selection(IN_THREAD);
	}
	else
	{
		auto clone=cursor->clone();
		clone->next(IN_THREAD);
		cursor->remove(IN_THREAD, clone);
	}
}

std::u32string editorObj::implObj::get()
{
	return cursor->begin()->get(cursor->end()).get_string();
}

void editorObj::implObj::set(IN_THREAD_ONLY, const std::u32string &string)
{
	input_mask dummy;

	moving_cursor moving{IN_THREAD, *this, dummy};

	to_end(IN_THREAD, dummy);
	selection_start(IN_THREAD)=cursor->begin();
	delete_char_or_selection(IN_THREAD, dummy);
	cursor->insert(IN_THREAD, string);
	recalculate(IN_THREAD);
	draw_changes(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
