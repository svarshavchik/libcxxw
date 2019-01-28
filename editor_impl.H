/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_impl_h
#define editor_impl_h

#include "editorobj.H"
#include "textlabel.H"
#include "label_elementfwd.H"
#include "cursor_pointer_elementfwd.H"
#include "x/w/impl/theme_font_elementfwd.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/impl/fonts/current_fontcollectionfwd.H"
#include "x/w/impl/focus/focusable_elementfwd.H"
#include "editor_peephole_impl.H"
#include "x/w/impl/richtext/richtextiteratorfwd.H"
#include "x/w/impl/richtext/richtextstringfwd.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "richtext/richtext_password_info.H"
#include "x/w/impl/richtext/richtext_draw_infofwd.H"
#include "drag_source_elementfwd.H"
#include "drag_destination_elementfwd.H"
#include "selection/current_selectionfwd.H"

#include "x/w/input_fieldfwd.H"
#include "x/w/input_field_config.H"
#include "x/w/input_field_lock.H"
#include "x/w/button_eventfwd.H"
#include "x/w/label.H"

#include <functional>

#include <tuple>

LIBCXXW_NAMESPACE_START

struct textedit_background_color;
struct textedit_disabled_background_color;

struct invisible_pointer;
struct dragging_pointer;
struct dragging_wontdrop_pointer;

//! Compose the superclass for editorObj::implObj.

//! Pulls in various mixins:
//!
//! - background_color_element, for the regular and disabled background
//! colors.
//!
//! - reference_font, used to compute the actual element metrics, based
//! on the nominal font size, and the requested number of rows and columns.
//!
//! - and the last, but not least, this is a focusable label element.

typedef background_color_elementObj<
	cursor_pointer_elementObj<drag_source_elementObj
				  <drag_destination_elementObj
				   <theme_font_elementObj
				    <focusable_elementObj
				     <label_elementObj<child_elementObj>
				      >>>>,
				  invisible_pointer,
				  dragging_pointer,
				  dragging_wontdrop_pointer>,
	textedit_background_color,
	textedit_disabled_background_color> editor_impl_superclass_t;

//! Implementation object for the editor element.

//! This editor element is derived from, guess what, the label element.
//! The label element is based on the richtext object, which offers
//! an API that can be used to handle key events to edit the contents of the
//! label, which the label display element takes care of updating.
//!
//! But before this object derives from the label element, it derives from
//! richtext_password_info first, because the richtext_password_info gets
//! used in the constructor initialization, and this is a cheap, dirty trick
//! to have this initialized beforehand.
//!
//! editorObj::implObj takes care of handling keyboard (and button) events,
//! and drawing the blinking cursor by manipulating the richtext API.

class LIBCXX_HIDDEN editorObj::implObj
	: public richtext_password_info,
	  public editor_impl_superclass_t {

	//! My superclass alias.
	typedef editor_impl_superclass_t superclass_t;

	//! Mcguffin for the scheduled cursor blink callback.
	ptr<obj> blinking;

 protected:
	//! The current cursor position.
	const richtextiterator cursor;
 private:
	//! The current dragging position

	//! dragging_location() updates this cursor. If we ever drop something,
	//! it'll go here.

	const richtextiterator dragged_pos;
	//! All access to the contents of the input field goes through here.

	//! Removes the characters between the \c cursor and the \c other
	//! iterator. If this is a password input field we also remove
	//! the corresponding characters from the real_string.

	void remove_content(ONLY IN_THREAD,
			    const richtextiterator &other);

	//! All access to the contents of the input field goes through here.

	//! Inserts into the input field. If this is a password input field
	//! the new unicode text gets inserted into the \c real_string, and
	//! the actual field gets password_char-s instead.

	void insert_content(ONLY IN_THREAD,
			    const std::u32string_view &str);

	//! All access to the contents of the input field goes through here.

	//! Returns the characters between the \c cursor and the \c other
	//! iterator. If this is a password field, the characters get
	//! retrieved from the real_string instead.
	richtextstring get_content(const richtextiterator &other);

	//! All access to the contents of the input field goes through here.

	//! Returns the characters between the two iterators.
	//! If this is a password field, the characters get
	//! retrieved from the real_string instead.

	richtextstring get_content(const richtextiterator &a,
				   const richtextiterator &b);

	//! Whether the last character inserted into a password field is shown

	//! This is the mcguffin for hiding the peek at the last password
	//! character.

	ptr<obj> password_peeking;

 public:

	void clear_password_peek(ONLY IN_THREAD);

 protected:

	struct LIBCXX_HIDDEN modifying_text;

	struct LIBCXX_HIDDEN moving_cursor;

	//! A container for the current selection cursor.

	//! A container for another richtextiterator cursor that marks the
	//! start (or the end) of a selection, if there is one. The regular
	//! cursor is the other end of the selection

	class selection_cursor_t {

		richtextiteratorptr cursor;

	public:

		//! Gain access to the selection cursor

		//! Also constructs the richtext_draw_info() object for
		//! passing to the richtext drawing functions.

		class const_lock {

			//! The lock on the underlying rich text object.

			internal_richtext_impl_t::lock internal_lock;

		protected:

			//! The cursor reference.

			richtextiteratorptr &cursor;
		public:
			//! Constructor
			const_lock(implObj &impl);

			//! Destructor
			~const_lock();

			std::optional<size_t> cursor_pos() const;

			richtext_draw_info get_richtext_draw_info(implObj &me)
				const;
		};

		class lock : public const_lock {

		public:

			using const_lock::cursor;

			//! Constructor
			lock(ONLY IN_THREAD, implObj &impl,
			     bool blinking_or_clearing=false);

			//! Destructor
			~lock();
		};
	};

	//! Whether a selection is currently in effect.

	selection_cursor_t selection_cursor;
 private:
	//! Our subclass of current_selectionObj

	class selectionObj;

	class primary_selectionObj;
	class secondary_selectionObj;
	class xdnd_selectionObj;

	//! Typedef alias

	typedef ref<selectionObj> selection;

	//! Typedef alias

	typedef ptr<selectionObj> selectionptr;

	//! Typedef alias
	typedef ptr<primary_selectionObj> primary_selectionptr;

	//! For the X server
	ptr<secondary_selectionObj> secondary_selection_thread_only;

	//! Callback
	functionptr< void(THREAD_CALLBACK, const input_change_info_t &)
		       > on_change_thread_only;

	//! Callback
	functionptr<bool(THREAD_CALLBACK, input_autocomplete_info_t &)
		    > on_autocomplete_thread_only;

	//! My parent peephole
	const ref<editor_peephole_implObj> parent_peephole;

	//! Whether we just autoselect-ed the entire contents of the field

	//! Set when input focus is gained, and the whole thing is selected.
	//! Cleared by a button event or any processed key event.

	bool autoselected=false;

	//! Implement show_droppable_pointer

	//! Invoked from the drag_source_element mixin to change the pointer
	//! to indicate that the dragged text can be dropped now.
	void show_droppable_pointer(ONLY IN_THREAD) override;

	//! Implement show_notdroppable_pointer

	//! Invoked from the drag_source_element mixin to change the pointer
	//! to indicate that the dragged text cannot be dropped now.
	void show_notdroppable_pointer(ONLY IN_THREAD) override;

 public:

	//! There's currently a primary selection

	//! For the X server's use.
	mpobj<primary_selectionptr> current_primary_selection;

	THREAD_DATA_ONLY(secondary_selection);
	THREAD_DATA_ONLY(on_change);
	THREAD_DATA_ONLY(on_autocomplete);

	//! This editor's configuration.

	//! The relevant bits of input_field_config that we keep.
	struct editor_config {
		size_t columns;
		size_t rows;
		bool autoselect;
		bool autodeselect;
		constexpr bool oneline() const { return rows <= 1; }
		size_t maximum_size;
		bool update_clipboards;

		//! Initialize from input_field_config

		editor_config(const input_field_config &);
	};

	const editor_config config;

	//! The input field hint, if one was specified in config
	const richtextptr hint;

 private:
	//! Whether we were showing the hint the last time we checked.

	bool is_showing_hint=false;

	//! Determine whether we should be showing the input hint right now.

	bool show_hint(ONLY IN_THREAD);

	//! Determine whether we should schedule a redraw in order to show hints

	//! Checks if show_hint() is now different than is_showing_hint,
	//! returning true if so (and updating is_showing_hint).

	bool should_redraw_to_show_hint(ONLY IN_THREAD);

 public:
	//! Override keyboard_focus()

	//! Whether we have input focus is factor in whether we should
	//! show_hint().

	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger) override;

	//! After the reference font is initialized, recalculate the peephole.

	void initialize(ONLY IN_THREAD) override;

	//! After the theme is updated, recalculate the peephole.

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;

	//! Multiply rows/columns by font's nominal width/height.

	std::tuple<dim_t, dim_t> nominal_size(ONLY IN_THREAD) const;

	//! Override set_minimum_override()

	//! Because nominal_width() and nominal_height() takes them into
	//! account, we'll call parent_peephole->recalculate().

	void set_minimum_override(ONLY IN_THREAD,
				  dim_t horiz_override,
				  dim_t vert_override) override;

	//! Parameters needed by the constructor

	//! The three parameters - the peephole, text, and config, do not
	//! get passed directly. Rather, they construct the init_args object,
	//! then the init_args object gets passed to the constructor.

	struct init_args {
		const ref<editor_peephole_implObj> &parent_peephole;
		const text_param &text;
		const input_field_config &config;
		current_theme_t::lock theme_lock;

		// Forwarded to the internal label implementation constructor:
		label_config label_config_args;
		textlabel_config textlabel_config_args;

		//! init_args also constructs some additional objects.

		init_args(const ref<editor_peephole_implObj> &parent_peephole,
			  const text_param &text,
			  const input_field_config &config);

		//! Default metadata.
		richtextmeta default_meta;

		//! Default metadata for the input field hint
		richtextmeta hint_meta;

		~init_args();
	};

	//! Constructor

	//! Construct the init_args, then pass it here.

	implObj(init_args &args);

	//! Destructor
	~implObj();

	//! Override do_draw().

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas) override;

	//! Override removed()
	void removed(ONLY IN_THREAD) override;

 protected:
	//! Override rewrap_due_to_updated_position().

	//! The editor doesn't rewrap in response to resizing. Instead, the
	//! editor sets the metrics, and sets richtext->minimum_width_override
	//! so that if the editing element is centered or right-aligned,
	//! the rich text object renders it to the full width of the
	//! display element.

	void rewrap_due_to_updated_position(ONLY IN_THREAD) override;

	//! Starts or stop the cursor blink.
	void window_focus_change(ONLY IN_THREAD, bool flag) override;

 private:

	//! Input focus has transferred into or out of this display element.

	//! Call scroll_cursor_into_view(), then start blinking, if enabled,
	// and has current_keyboard_focus(). Otherwise stop blinking.

	void blink_if_has_focus(ONLY IN_THREAD);
 public:
	//! Inherited from elementObj::implObj

	//! Processes a key event.
	bool process_key_event(ONLY IN_THREAD, const key_event &ke) override;

	//! Set the cursor pointer.

	//! We actually set the cursor pointer on our parent peephole.

	void set_cursor_pointer(ONLY IN_THREAD,
				const cursor_pointer &p);
	//! This display element uses XIM.

	bool uses_input_method() override;

	//! Override process_button_event().

	//! Button 1 moves the cursor, button2 pastes the primary clipboard.

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Pointer motion event
	void report_motion_event(ONLY IN_THREAD, const motion_event &)
		override;

	//! Check if we will accept dragged content.

	bool accepts_drop(ONLY IN_THREAD,
			  const source_dnd_formats_t &source_formats,
			  xcb_timestamp_t timestamp)
		override;

	//! Update dragging location.

	//! If accepts_drop(), the accepting element's dragging_location()
	//! gets called.
	void dragging_location(ONLY IN_THREAD, coord_t x, coord_t y,
			       xcb_timestamp_t timestamp) override;

	//! Ok, time to drop was the cat dragged in.

	current_selection_handlerptr drop(ONLY IN_THREAD,
					  xcb_atom_t &type,
					  const ref<obj> &finish_mcguffin)
		override;
 private:
	//! Current theme's secondary clipboard
	xcb_atom_t secondary_clipboard(ONLY IN_THREAD);

	//! The that was dropped.
	xcb_atom_t dropped_atom=XCB_NONE;

	//! Most recently reported coordinates.
	coord_t most_recent_x;

	//! Most recently reported coordinates.
	coord_t most_recent_y;

	//! Timer for scrolling while moving the cursor

	ptr<obj> motion_scroll_callback;

	//! Install a motion_scroll_callback

	void start_scrolling(ONLY IN_THREAD);

	//! Remove the motion_scroll_callback, and scroll the editor.

	void stop_scrolling(ONLY IN_THREAD);

	//! Scroll while dragging the mouse button.

	void scroll(ONLY IN_THREAD);

	//! Set a callback for the next cursor blink on/off.

	//! Invoked only from blink().
	void schedule_blink(ONLY IN_THREAD);

	//! Blink the cursor, on or off.

	//! Invoked only from modifying_text, blink_if_has_focus(),
	//! schedule_blink(), and unblink().

	void blink(ONLY IN_THREAD);

	//! Whether the cursor is blinked on.
	bool blinkon=false;

	//! If the cursor is currently blinked on, blink it off.

	//! Invoked only from blink_if_has_focus(),
	void unblink(ONLY IN_THREAD);

	//! Override enablability_changed()

	//! Update the element's background color in response to it being
	//! enabled or disabled.

	void enablability_changed(ONLY IN_THREAD) override;

	//! Draw changed text fragments.
	void draw_changes(ONLY IN_THREAD,
			  selection_cursor_t::lock &cursor_lock,
			  input_change_type change_made,
			  size_t deleted,
			  size_t inserted);

	//! Draw a fragment range.
	void draw_between(ONLY IN_THREAD,
			  const richtextiterator &a,
			  const richtextiterator &b);

	//! Process key press.
	bool process_keypress(ONLY IN_THREAD, const key_event &);

	//! ensure_visibility() of the cursor.
	void scroll_cursor_into_view(ONLY IN_THREAD);

	//! Override set_focus_and_ensure_visibility()

	//! The scroll_cursor_into_view() ensures visibility for the cursor.
	//! This display element is in a peephole, and the peephole will take
	//! care of making everything visible.

	void set_focus_and_ensure_visibility(ONLY IN_THREAD,
					     const callback_trigger_t &)
		override;

	//! Delete the current selection.

	//! Constructed to begin the process of deleting the current selection.
	//! to_be_deleted() returns the number of characters in the current
	//! selection that will be deleted (0 if there is no current selection).
	//!
	//! do_delete() executes the deletion.
	//!
	//! to_be_deleted() is a slight misnomer. It can also be called after
	//! do_delete(), and returns the number of characters that were
	//! deleted.

	struct delete_selection_info {

		//! Who created me

		implObj &me;

	public:
		//! The lock on me's cursor.

		selection_cursor_t::lock cursor_lock;

	private:

		//! The constructor computed the character count.

		size_t n;

	public:
		//! Constructor
		delete_selection_info(ONLY IN_THREAD, implObj &me);

		//! Return the computed character count.
		inline size_t to_be_deleted() const
		{
			return n;
		}

		//! Delete the current selection.

		void do_delete(ONLY IN_THREAD);
	};

	//! This element can receive a selection
	bool selection_can_be_received() override;

	//! Override cut_or_copy_selection(), and implement it.
	bool cut_or_copy_selection(cut_or_copy_op, xcb_atom_t) override;

	//! Override cut_or_copy_selection(), and implement it.
	bool cut_or_copy_selection(ONLY IN_THREAD,
				   cut_or_copy_op, xcb_atom_t) override;

	//! Prepare a new primary selection, if we can.
	void create_primary_selection(ONLY IN_THREAD);

	//! Prepare a new secondary selection, if we can.
	void create_secondary_selection(ONLY IN_THREAD,
					xcb_atom_t selection);

	//! Prepare a new secondary selection, if we can.

	//! Returns false if no selection was created.
	bool create_secondary_selection(ONLY IN_THREAD,
					xcb_atom_t selection,
					selection_cursor_t::lock &lock);

	//! Remove the announced primary selection.
	void remove_primary_selection(ONLY IN_THREAD);

	//! Remove the announced secondary selection.
	void remove_secondary_selection(ONLY IN_THREAD);

	//! Text was pasted.

	//! Calls insert(), then scroll_cursor_into_view().

	bool pasted(ONLY IN_THREAD,
		    const std::u32string_view &str) override;

 private:

	//! Insert text at current position.

	void insert(ONLY IN_THREAD,
		    const std::u32string_view &str);

	//! Move cursor to the beginning of the text.
	bool to_begin(ONLY IN_THREAD, const input_mask &mask);

	//! Move cursor to the end of the text.
	bool to_end(ONLY IN_THREAD, const input_mask &mask);

	//! Select the entire text.
	void select_all(ONLY IN_THREAD);

	//! What happens when DEL is pressed.
	size_t delete_char_or_selection(ONLY IN_THREAD, const input_mask &mask);

 public:

	//! Return the current contents  of the editor.

	std::u32string get();

	//! Return the size of the input field.
	size_t size() const;

	//! Return the current cursor position.
	std::tuple<size_t, size_t> pos();

 private:
	//! Return the current cursor position.
	std::tuple<size_t, size_t> pos(selection_cursor_t::const_lock &);
 public:
	//! Set the current contents of the editor

	void set(ONLY IN_THREAD, const std::u32string &string);

	//! Set the current contents of the editor
	virtual void set(ONLY IN_THREAD, const std::u32string &string,
			 size_t cursor_pos, size_t selection_pos);

	//! Whether we're cool with giving up focus.
	bool ok_to_lose_focus(ONLY IN_THREAD,
			      const callback_trigger_t &trigger) override;

 private:
	//! Whether the contents of the input field have been modified since they were validated.

	bool validation_required_thread_only=true;

 public:
	//! Invoke validation_callback if validation_required.

	bool validate_modified(ONLY IN_THREAD, const callback_trigger_t &);
 private:
	//! The validation callback.

	functionptr<input_field_validation_callback_t>
		validation_callback_thread_only;

 public:
	THREAD_DATA_ONLY(validation_required);
	THREAD_DATA_ONLY(validation_callback);

};

LIBCXXW_NAMESPACE_END

#endif
