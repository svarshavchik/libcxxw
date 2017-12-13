/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "element_screen.H"
#include "cursor_pointer_element.H"
#include "reference_font_element.H"
#include "screen.H"
#include "draw_info.H"
#include "busy.H"
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
#include "icon.H"
#include "catch_exceptions.H"
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

static inline richtextmeta create_default_meta(const ref<containerObj::implObj>
&container)
{
	auto &element=container->get_element_impl();

	auto bg_color=element.create_background_color
		("textedit_foreground_color");
	auto font=element.create_theme_font("textedit");

	return {bg_color, font};
}

static inline richtextstring
create_initial_string(const ref<containerObj::implObj> &container,
		      const richtextmeta &default_meta,
		      const text_param &text)
{
	auto &element=container->get_element_impl();

	text_param cpy=text;

	cpy(" ");

	auto string=element.create_richtextstring(default_meta, cpy);

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

	// moved is passed to the constructor. The destructor initialized
	// the bool to indicate whether we actually moved.

	bool &moved;
	moving_cursor(IN_THREAD_ONLY, editorObj::implObj &me,
		      const input_mask &mask, bool &moved)
		: moving_cursor(IN_THREAD, me,
				mask.shift || (mask.buttons & 1), false,
				moved)
	{
	}

	moving_cursor(IN_THREAD_ONLY, editorObj::implObj &me,
		      bool selection_in_progress,
		      bool processing_clear,
		      bool &moved)
		: IN_THREAD(IN_THREAD), me(me), old_cursor(me.cursor->clone()),
		moved{moved}
	{
		selection_cursor_t::lock cursor_lock{me};

		// Make sure to turn off the blink, before
		// starting a selection.
		me.unblink(IN_THREAD);

		if (selection_in_progress)
		{
			if (!cursor_lock.cursor)
				cursor_lock.cursor=me.cursor->clone();
		}
		else
		{
			auto p=cursor_lock.cursor;

			if (p)
			{
				// Remove the highlighted selection.

				cursor_lock.cursor=nullptr;

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

		if (cursor_lock.cursor)
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

		moved=old_cursor->compare(me.cursor) != 0;
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

editorObj::implObj::selection_cursor_t::lock::lock(implObj &impl)
	: internal_lock{impl.cursor->my_richtext->impl},
	  cursor{impl.selection_cursor.cursor}
{
}

editorObj::implObj::selection_cursor_t::lock::~lock()=default;

////////////////////////////////////////////////////////////////////////////

editorObj::implObj::implObj(const ref<editor_peephole_implObj> &parent_peephole,
			    const text_param &text,
			    const input_field_config &config)
	: implObj(parent_peephole,
		  text,
		  config,
		  create_default_meta(parent_peephole))
{
	// The first input field in a window gets focus when its shown.

	autofocus=true;
}

editorObj::implObj::implObj(const ref<editor_peephole_implObj> &parent_peephole,
			    const text_param &text,
			    const input_field_config &config,
			    const richtextmeta &default_meta)
	: implObj(parent_peephole, config, default_meta,
		  create_initial_string(parent_peephole, default_meta, text))
{
}

editorObj::implObj::implObj(const ref<editor_peephole_implObj> &parent_peephole,
			    const input_field_config &config,
			    const richtextmeta &default_meta,
			    richtextstring &&string)
	: superclass_t(// Invisible pointer cursor
		       parent_peephole->get_element_impl().get_window_handler()
		       .create_icon({"cursor-invisible"})->create_cursor(),
		       // Capture the string's font.
		       string.get_meta().at(0).second.getfont(),
		       parent_peephole, config.alignment, 0,
		       std::move(string),
		       default_meta,
		       false,
		       "textedit@libcxx.com"),
	  cursor(this->text->end()),
	  on_change_thread_only( [](const auto &) {} ),
	  on_autocomplete_thread_only([](const auto &) { return false; }),
	  parent_peephole(parent_peephole),
	  config(config)
{

#ifdef EDITOR_CONSTRUCTOR_DEBUG
	EDITOR_CONSTRUCTOR_DEBUG();
#endif
	if (config.columns < 2)
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
	dim_t w=dim_t::truncate(config.columns *
				(dim_t::value_type)
				font_nominal_width(IN_THREAD));

	auto &hv=*get_horizvert(IN_THREAD);

	if (w < hv.minimum_horiz_override(IN_THREAD))
		w=hv.minimum_horiz_override(IN_THREAD);

	return w;
}

dim_t editorObj::implObj::nominal_height(IN_THREAD_ONLY) const
{

	dim_t h=dim_t::truncate(config.rows *
				(dim_t::value_type)
				font_height(IN_THREAD));

	auto &hv=*get_horizvert(IN_THREAD);

	if (h < hv.minimum_vert_override(IN_THREAD))
		h=hv.minimum_vert_override(IN_THREAD);

	return h;
}

void editorObj::implObj::set_minimum_override(IN_THREAD_ONLY,
					      dim_t horiz_override,
					      dim_t vert_override)
{
	superclass_t::set_minimum_override(IN_THREAD, horiz_override,
					   vert_override);

	parent_peephole->recalculate(IN_THREAD, *this);
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
	initialize_if_needed(IN_THREAD);
	text->thread_lock(IN_THREAD,
			  [&, this]
			  (IN_THREAD_ONLY, const auto &impl)
			  {
				  (*impl)->minimum_width_override=
					  data(IN_THREAD).current_position
					  .width;
			  });

	text->rewrap(IN_THREAD, preferred_width);
}

void editorObj::implObj::keyboard_focus(IN_THREAD_ONLY)
{
	superclass_t::keyboard_focus(IN_THREAD);

	blink_if_has_focus(IN_THREAD);

#ifdef EDITOR_FOCUS_DEBUG
	EDITOR_FOCUS_DEBUG();
#endif

	bool deselect=false;

	if (config.autoselect)
	{
		if (current_keyboard_focus(IN_THREAD))
		{
			select_all(IN_THREAD);
			autoselected=true;
		}
		else if (autoselected)
		{
			// If the entire selection was autoselected on
			// focus gain, remove the selection on focus loss.

			deselect=true;
		}
	}

	if (config.autodeselect && !current_keyboard_focus(IN_THREAD))
		deselect=true;

	if (deselect)
	{
		// Leverage the existing moving_cursor logic.

		bool ignored;

		moving_cursor moving{IN_THREAD, *this, false, false, ignored};
	}
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
			 auto got=me.get();

			 if (got)
			 {
				 auto &[me]=*got;

				 me->blink(IN_THREAD);
			 }
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
	selection_cursor_t::lock cursor_lock{*this};

	// We actually blink the cursor only when we are not showing a
	// selection.

	if ( (!cursor_lock.cursor) ||
	     cursor_lock.cursor->compare(cursor) == 0)
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
				 {cursor_lock.cursor, cursor},
				 get_draw_info(IN_THREAD),
				 {current_position});
}

bool editorObj::implObj::process_key_event(IN_THREAD_ONLY, const key_event &ke)
{
	// Hide the pointer by installing the invisible pointer, here.

	// The peephole's report_motion_event() restores the default pointer.

	if (ke.keypress)
		parent_peephole->get_element_impl()
			.set_cursor_pointer(IN_THREAD,
					    tagged_cursor_pointer(IN_THREAD));

	if (ke.keypress && ke.notspecial())
	{
		if (process_keypress(IN_THREAD, ke))
		{
			autoselected=false;
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

	bool moved;

	switch (ke.keysym) {
	case XK_Left:
	case XK_KP_Left:
		{
			moving_cursor moving{IN_THREAD, *this, ke, moved};
			cursor->prev(IN_THREAD);
		}
		return moved;
	case XK_Right:
	case XK_KP_Right:
		{
			moving_cursor moving{IN_THREAD, *this, ke, moved};
			cursor->next(IN_THREAD);
		}
		return moved;
	case XK_Up:
	case XK_KP_Up:
		{
			moving_cursor moving{IN_THREAD, *this, ke, moved};
			cursor->up(IN_THREAD);
		}
		return moved;
	case XK_Down:
	case XK_KP_Down:
		{
			moving_cursor moving{IN_THREAD, *this, ke, moved};
			cursor->down(IN_THREAD);
		}
		return moved;
	case XK_Delete:
	case XK_KP_Delete:
		unblink(IN_THREAD);
		{
			selection_cursor_t::lock cursor_lock{*this};

			size_t deleted=delete_char_or_selection(IN_THREAD, ke);
			recalculate(IN_THREAD);
			draw_changes(IN_THREAD, cursor_lock,
				     input_change_type::deleted, deleted, 0);
		}
		blink(IN_THREAD);
		return true;
	case XK_Page_Up:
	case XK_KP_Page_Up:
		{
			moving_cursor moving{IN_THREAD, *this, ke, moved};
			cursor->page_up(IN_THREAD,
					bounds(get_draw_info(IN_THREAD)
					       .element_viewport).height);
		}
		return moved;
	case XK_Page_Down:
	case XK_KP_Page_Down:
		{
			moving_cursor moving{IN_THREAD, *this, ke, moved};
			cursor->page_down(IN_THREAD,
					  bounds(get_draw_info(IN_THREAD)
						 .element_viewport).height);
		}
		return moved;
	case XK_Home:
		{
			moving_cursor moving{IN_THREAD, *this, ke, moved};
			cursor->start_of_line();
		}
		return moved;
	case XK_End:
		{
			moving_cursor moving{IN_THREAD, *this, ke, moved};
			cursor->end_of_line();
		}
		return moved;
	case XK_KP_Home:
		return to_begin(IN_THREAD, ke);
	case XK_KP_End:
		return to_end(IN_THREAD, ke);
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
		selection_cursor_t::lock cursor_lock{*this};

		unblink(IN_THREAD);

		size_t deleted=0;

		if (cursor_lock.cursor)
			deleted=delete_selection(IN_THREAD);

		auto old=cursor->clone();
		cursor->prev(IN_THREAD);

		deleted += (cursor->pos() == old->pos() ? 0:1);

		cursor->remove(IN_THREAD, old);

		recalculate(IN_THREAD);
		draw_changes(IN_THREAD, cursor_lock,
			     input_change_type::deleted, deleted, 0);
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
	selection_cursor_t::lock cursor_lock{*this};

	unblink(IN_THREAD);

	size_t deleted=0;

	if (cursor_lock.cursor)
		deleted=delete_selection(IN_THREAD);
	cursor->insert(IN_THREAD, str);
	recalculate(IN_THREAD);
	draw_changes(IN_THREAD, cursor_lock,
		     input_change_type::inserted, deleted, str.size());
	blink(IN_THREAD);
}

void editorObj::implObj::draw_changes(IN_THREAD_ONLY,
				      selection_cursor_t::lock &cursor_lock,
				      input_change_type change_made,
				      size_t deleted,
				      size_t inserted)
{
	text->redraw_whatsneeded(IN_THREAD, *this,
				 {cursor_lock.cursor, cursor},
				 get_draw_info(IN_THREAD));

	try {
		on_change(IN_THREAD)({change_made, inserted, deleted,
					size()});
	} CATCH_EXCEPTIONS;

	// Invoke the autocomplete callback if the conditions are right.

	size_t s=size();

	if (change_made != input_change_type::set &&
	    s > 0 && cursor->pos() == s &&
	    (!cursor_lock.cursor || cursor_lock.cursor->pos() == cursor->pos()))
	{
		busy_impl mcguffin{*this};

		input_autocomplete_info_t info{get(), 0, mcguffin};

		bool flag=false;

		try {
			flag=on_autocomplete(IN_THREAD)(info);
		} CATCH_EXCEPTIONS;

		if (flag)
			set(IN_THREAD, info.string, info.string.size(),
			    info.selection_start);
	}
}

void editorObj::implObj::do_draw(IN_THREAD_ONLY,
				 const draw_info &di,
				 const rectangle_set &areas)
{
#ifdef EDITOR_DRAW
	EDITOR_DRAW();
#endif
	selection_cursor_t::lock cursor_lock{*this};

	text->full_redraw(IN_THREAD, *this,
			     {cursor_lock.cursor, cursor},
			  di, areas);
}

void editorObj::implObj::draw_between(IN_THREAD_ONLY,
				      const richtextiterator &a,
				      const richtextiterator &b)
{
	selection_cursor_t::lock cursor_lock{*this};

	text->redraw_between(IN_THREAD, *this,
			     a, b,
			     {cursor_lock.cursor, cursor},
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
	autoselected=false;

	if (be.press && (be.button == 1 || be.button == 2))
	{
		bool ignored;

		moving_cursor moving{IN_THREAD, *this, be, ignored};

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

	// If we're hiding the pointer, remove it.

	most_recent_x=me.x;
	most_recent_y=me.y;

	if (me.mask.buttons & 1)
	{
		bool ignored;
		{
			moving_cursor moving{IN_THREAD, *this, me.mask,
					ignored};
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
			 auto got=me.get();

			 if (got)
			 {
				 auto & [me]=*got;
				 me->scroll(IN_THREAD);
			 }
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
		bool ignored;

		// Leverage moving_cursor for all the heavy lifting.
		//
		// We pretend that we're moving the cursor without
		// a selection in progress, that's all.
		moving_cursor dummy{IN_THREAD, *p, false, true, ignored};

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

size_t editorObj::implObj::delete_selection(IN_THREAD_ONLY)
{
	selection_cursor_t::lock cursor_lock{*this};

	size_t p1=cursor->pos();

	size_t p2=cursor_lock.cursor->pos();

	if (p1 > p2)
		std::swap(p1, p2);

	cursor->remove(IN_THREAD, cursor_lock.cursor);
	cursor_lock.cursor=richtextiteratorptr();
	remove_primary_selection(IN_THREAD);

	return p2-p1;
}

editorObj::implObj::selection
editorObj::implObj::create_selection(IN_THREAD_ONLY)
{
	selection_cursor_t::lock cursor_lock{*this};

	return selection::create(get_screen()->impl->thread
				 ->timestamp(IN_THREAD),
				 ref<implObj>(this),
				 cursor,
				 cursor_lock.cursor);
}

void editorObj::implObj::create_primary_selection(IN_THREAD_ONLY)
{
	if (!config.update_clipboards)
		return;

	selection_cursor_t::lock cursor_lock{*this};

	if (!cursor_lock.cursor)
		return;

	if (cursor_lock.cursor->compare(cursor) == 0)
		return; // Nothing selected, really.
	// Remove the previous one.

	remove_primary_selection(IN_THREAD);

	auto s=create_selection(IN_THREAD);
	primary_selection(IN_THREAD)=s;
	get_window_handler().selection_announce(IN_THREAD, XCB_ATOM_PRIMARY, s);
}

void editorObj::implObj::create_secondary_selection(IN_THREAD_ONLY)
{
	if (!config.update_clipboards)
		return;

	selection_cursor_t::lock cursor_lock{*this};

	if (!cursor_lock.cursor)
		return;

	if (cursor_lock.cursor->compare(cursor) == 0)
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
	if (!config.update_clipboards)
		return;

	if (!primary_selection(IN_THREAD))
		return;

	primary_selection(IN_THREAD)->valid_flag(IN_THREAD)=false;
	primary_selection(IN_THREAD)=nullptr;

	get_window_handler().selection_discard(IN_THREAD, XCB_ATOM_PRIMARY);
}

void editorObj::implObj::remove_secondary_selection(IN_THREAD_ONLY)
{
	if (!config.update_clipboards)
		return;

	if (!secondary_selection(IN_THREAD))
		return;

	secondary_selection(IN_THREAD)->valid_flag(IN_THREAD)=false;
	secondary_selection(IN_THREAD)=nullptr;

	get_window_handler().selection_discard(IN_THREAD, XCB_ATOM_SECONDARY);
}

bool editorObj::implObj::to_begin(IN_THREAD_ONLY, const input_mask &mask)
{
	bool moved;
	{
		moving_cursor moving{IN_THREAD, *this, mask, moved};

		cursor->swap(cursor->begin());
	}
	return moved;
}

bool editorObj::implObj::to_end(IN_THREAD_ONLY, const input_mask &mask)
{
	bool moved;

	{
		moving_cursor moving{IN_THREAD, *this, mask, moved};

		cursor->swap(cursor->end());
	}
	return moved;
}

void editorObj::implObj::select_all(IN_THREAD_ONLY)
{
	input_mask mask;

	to_begin(IN_THREAD, mask);

	mask.shift=true;

	to_end(IN_THREAD, mask);
}

size_t editorObj::implObj::delete_char_or_selection(IN_THREAD_ONLY,
						  const input_mask &mask)
{
	selection_cursor_t::lock cursor_lock{*this};

	if (cursor_lock.cursor)
	{
		if (mask.shift)
			create_secondary_selection(IN_THREAD);
		return delete_selection(IN_THREAD);
	}

	auto clone=cursor->clone();
	clone->next(IN_THREAD);

	size_t p=cursor->pos() == clone->pos() ? 0:1;

	cursor->remove(IN_THREAD, clone);

	return p;
}

std::u32string editorObj::implObj::get()
{
	return cursor->begin()->get(cursor->end()).get_string();
}

size_t editorObj::implObj::size() const
{
	return cursor->my_richtext->read_only_lock
		([]
		 (const auto &impl)
		 {
			 return (*impl)->num_chars-1;
		 });
}

std::tuple<size_t, size_t> editorObj::implObj::pos()
{
	selection_cursor_t::lock cursor_lock{*this};

	return pos(cursor_lock);
}

std::tuple<size_t, size_t>
editorObj::implObj::pos(selection_cursor_t::lock &cursor_lock)
{
	size_t p=cursor->pos();

	size_t p2=p;

	if (cursor_lock.cursor)
		p2=cursor_lock.cursor->pos();

	return {p, p2};
}

void editorObj::implObj::set(IN_THREAD_ONLY, const std::u32string &string)
{
	set(IN_THREAD, string, string.size(), string.size());
}

void editorObj::implObj::set(IN_THREAD_ONLY, const std::u32string &string,
			     size_t cursor_pos, size_t selection_pos)
{
	size_t s=string.size();

	if (cursor_pos > s)
		cursor_pos=s;
	if (selection_pos > s)
		selection_pos=s;

	selection_cursor_t::lock cursor_lock{*this};

	bool will_have_selection=cursor_pos != selection_pos;

	bool ignored;

	moving_cursor moving{IN_THREAD, *this, will_have_selection, false,
			ignored};

	cursor->swap(cursor->end());

	size_t deleted=cursor->pos();

	cursor->remove(IN_THREAD, cursor->begin());
	remove_primary_selection(IN_THREAD);
	cursor->insert(IN_THREAD, string);

	cursor->swap(cursor->pos(cursor_pos));

	if (will_have_selection)
	{
		cursor_lock.cursor=cursor->pos(selection_pos);
	}
	else
	{
		cursor_lock.cursor=nullptr;
	}

	recalculate(IN_THREAD);
	draw_changes(IN_THREAD, cursor_lock,
		     input_change_type::set, deleted, string.size());
}

LIBCXXW_NAMESPACE_END
