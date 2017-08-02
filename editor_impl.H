/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_impl_h
#define editor_impl_h

#include "editorobj.H"
#include "textlabel.H"
#include "label_elementfwd.H"
#include "reference_font_elementfwd.H"
#include "fonts/current_fontcollectionfwd.H"
#include "focus/focusable_elementfwd.H"
#include "editor_peephole_impl.H"
#include "richtext/richtextiteratorfwd.H"
#include "richtext/richtextstring.H"
#include "richtext/richtextmeta.H"
#include "x/w/input_field_config.H"
#include "x/w/input_field_lock.H"
#include "x/w/button_eventfwd.H"

#include <tuple>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN editorObj::implObj
	: public reference_font_elementObj<focusable_elementObj
					   <label_elementObj<child_elementObj>
					    >> {

	//! My superclass alias.
	typedef reference_font_elementObj<focusable_elementObj
					  <label_elementObj<child_elementObj>>
					  > superclass_t;

	//! Mcguffin for the scheduled cursor blink callback.
	ptr<obj> blinking;

	//! The current cursor position.
	const richtextiterator cursor;

	//! A container for the current selection cursor.

	//! A container for another richtextiterator cursor that marks the
	//! start (or the end) of a selection, if there is one. The regular
	//! cursor is the other end of the selection

	class selection_cursor_t {

		richtextiteratorptr cursor;

	public:

		//! Gain access to the selection cursor

		class lock {

			//! The lock on the underlying rich text object.

			internal_richtext_impl_t::lock internal_lock;

		public:

			//! The cursor reference.

			richtextiteratorptr &cursor;

			//! Constructor
			lock(implObj &impl);

			//! Destructor
			~lock();
		};
	};

	//! Whether a selection is currently in effect.

	selection_cursor_t selection_cursor;

	//! Our subclass of current_selectionObj

	class selectionObj;

	//! Typedef alias

	typedef ref<selectionObj> selection;

	//! Typedef alias

	typedef ptr<selectionObj> selectionptr;

	//! For the X server
	selectionptr primary_selection_thread_only;

	//! For the X server
	selectionptr secondary_selection_thread_only;

	struct moving_cursor;

	//! Callback
	std::function< void(const input_change_info_t &)
		       > on_change_thread_only;

	//! Callback
	std::function<bool(input_autocomplete_info_t &)
		      > on_autocomplete_thread_only;

	//! My parent peephole
	const ref<editor_peephole_implObj> parent_peephole;

 public:

	THREAD_DATA_ONLY(primary_selection);
	THREAD_DATA_ONLY(secondary_selection);
	THREAD_DATA_ONLY(on_change);
	THREAD_DATA_ONLY(on_autocomplete);

	//! This editor's configuration.

	const input_field_config config;

	//! After the reference font is initialized, recalculate the peephole.

	void initialize(IN_THREAD_ONLY) override;

	//! After the theme is updated, recalculate the peephole.

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;

	//! Multiply columns by font's nominal width.

	dim_t nominal_width(IN_THREAD_ONLY) const;

	//! Multiple rows by font's height.
	dim_t nominal_height(IN_THREAD_ONLY) const;

	//! Contsructor
	implObj(const ref<editor_peephole_implObj> &parent_peephole,
		const text_param &text,
		const input_field_config &config);

 private:
	//! Internal constructor

	implObj(const ref<editor_peephole_implObj> &parent_peephole,
		std::tuple<richtextmeta, richtextstring> &&meta_and_string,
		const input_field_config &config);

 public:
	//! Destructor
	~implObj();

	//! Override do_draw().

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas) override;

	//! Override removed()
	void removed(IN_THREAD_ONLY) override;

 protected:
	//! Override compute_preferred_width().

	//! We compute it directly from font and columns.
	void compute_preferred_width(IN_THREAD_ONLY) override;

	//! Override rewrap().

	//! Rewrap according to the preferred width.

	bool rewrap(IN_THREAD_ONLY) override;

	//! Override calculate_current_metrics().

	//! The editor is not resizable, so we adjust the text's
	//! get_metrics() to reflect that.

	std::pair<metrics::axis, metrics::axis>
		calculate_current_metrics(IN_THREAD_ONLY) override;

	//! Override rewrap_due_to_updated_position().

	//! The editor doesn't rewrap in response to resizing. The editor
	//! sets the metrics, and that's the final word on it.

	void rewrap_due_to_updated_position(IN_THREAD_ONLY) override;

	//! Inherited from elementObj::implObj

	//! Starts or stops the cursor blink.

	void keyboard_focus(IN_THREAD_ONLY) override;

	//! Starts or stop the cursor blink.
	void window_focus_change(IN_THREAD_ONLY, bool flag) override;

 private:

	//! Input focus has transferred into or out of this display element.

	//! Call scroll_cursor_into_view(), then start blinking, if enabled,
	// and has current_keyboard_focus(). Otherwise stop blinking.

	void blink_if_has_focus(IN_THREAD_ONLY);
 public:
	//! Inherited from elementObj::implObj

	//! Processes a key event.
	bool process_key_event(IN_THREAD_ONLY, const key_event &ke);

	//! This display element uses XIM.

	bool uses_input_method() override;

	//! Button 1 moves the cursor, button2 pastes the primary clipboard.

	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;
	//! Pointer motion event
	void report_motion_event(IN_THREAD_ONLY, const motion_event &)
		override;

 private:
	//! Most recently reported coordinates.
	coord_t most_recent_x;

	//! Most recently reported coordinates.
	coord_t most_recent_y;

	//! Timer for scrolling while moving the cursor

	ptr<obj> motion_scroll_callback;

	//! Install a motion_scroll_callback

	void start_scrolling(IN_THREAD_ONLY);

	//! Remove the motion_scroll_callback, and scroll the editor.

	void stop_scrolling(IN_THREAD_ONLY);

	//! Scroll while dragging the mouse button.

	void scroll(IN_THREAD_ONLY);

	//! Set a callback for the next cursor blink on/off.
	void schedule_blink(IN_THREAD_ONLY);

	//! Blink the cursor, on or off.
	void blink(IN_THREAD_ONLY);

	//! Blink the cursor, on or off.

	//! Entry point using by moving_cursor, with the saved cursor position
	//! prior the move.
	void blink(IN_THREAD_ONLY, const richtextiterator &cursor);

	//! Whether the cursor is blinked on.
	bool blinkon=false;

	//! If the cursor is currently blinked on, blink it off.
	void unblink(IN_THREAD_ONLY);

	//! If the cursor is currently blinked on, blink it off.

	//! Entry point using by moving_cursor, with the saved cursor position
	//! prior the move.
	void unblink(IN_THREAD_ONLY, const richtextiterator &cursor);

	//! Draw changed text fragments.
	void draw_changes(IN_THREAD_ONLY,
			  selection_cursor_t::lock &cursor_lock,
			  input_change_type change_made,
			  size_t deleted,
			  size_t inserted);

	//! Draw a fragment range.
	void draw_between(IN_THREAD_ONLY,
			  const richtextiterator &a,
			  const richtextiterator &b);

	//! Process key press.
	bool process_keypress(IN_THREAD_ONLY, const key_event &);

	//! ensure_visibility() of the cursor.
	void scroll_cursor_into_view(IN_THREAD_ONLY);

	//! Override set_focus_and_ensure_visibility()

	//! The scroll_cursor_into_view() ensures visibility for the cursor.
	//! This display element is in a peephole, and the peephole will take
	//! care of making everything visible.

	void set_focus_and_ensure_visibility(IN_THREAD_ONLY) override;

	//! Delete the current selection
	size_t delete_selection(IN_THREAD_ONLY);

	//! Create a new selection object.
	selection create_selection(IN_THREAD_ONLY);

	//! Prepare a new primary selection, if we can.
	void create_primary_selection(IN_THREAD_ONLY);

	//! Prepare a new secondary selection, if we can.
	void create_secondary_selection(IN_THREAD_ONLY);

	//! Remove the announced primary selection.
	void remove_primary_selection(IN_THREAD_ONLY);

	//! Remove the announced secondary selection.
	void remove_secondary_selection(IN_THREAD_ONLY);

	//! Text was pasted.

	//! Calls insert(), then scroll_cursor_into_view().

	bool pasted(IN_THREAD_ONLY,
		    const std::u32string_view &str) override;

 private:

	//! Insert text at current position.

	void insert(IN_THREAD_ONLY,
		    const std::u32string_view &str);

	//! Move cursor to the beginning of the text.
	void to_begin(IN_THREAD_ONLY, const input_mask &mask);

	//! Move cursor to the end of the text.
	void to_end(IN_THREAD_ONLY, const input_mask &mask);

	//! Select the entire text.
	void select_all(IN_THREAD_ONLY);

	//! What happens when DEL is pressed.
	size_t delete_char_or_selection(IN_THREAD_ONLY, const input_mask &mask);

 public:

	//! Return the current contents  of the editor.

	std::u32string get();

	//! Return the size of the input field.
	size_t size() const;

	//! Return the current cursor position.
	std::tuple<size_t, size_t> pos();

 private:
	//! Return the current cursor position.
	std::tuple<size_t, size_t> pos(selection_cursor_t::lock &cursor_lock);
 public:
	//! Set the current contents of the editor

	void set(IN_THREAD_ONLY, const std::u32string &string);

	//! Set the current contents of the editor
	void set(IN_THREAD_ONLY, const std::u32string &string,
		 size_t cursor_pos, size_t selection_pos);
};

LIBCXXW_NAMESPACE_END

#endif
