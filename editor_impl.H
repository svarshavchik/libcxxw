/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_impl_h
#define editor_impl_h

#include "editorobj.H"
#include "textlabel.H"
#include "label_elementfwd.H"
#include "cursor_pointer_elementfwd.H"
#include "icon_images_set_elementfwd.H"
#include "x/w/impl/theme_font_elementfwd.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/impl/fonts/current_fontcollectionfwd.H"
#include "x/w/impl/focus/focusable_elementfwd.H"
#include "editor_peephole_impl.H"
#include "x/w/richtext/richtextiteratorfwd.H"
#include "x/w/richtext/richtextstringfwd.H"
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

struct left_to_right_icon;
struct right_to_left_icon;

//! Compose the superclass for editor_implObj.

//! Pulls in various mixins:
//!
//! - background_color_element, for the regular and disabled background
//! colors.
//!
//! - icon_images_set_element, for text direction icon tooltips.
//!
//! - reference_font, used to compute the actual element metrics, based
//! on the nominal font size, and the requested number of rows and columns.
//!
//! - and the last, but not least, this is a focusable label element.

typedef background_color_elementObj<
	icon_images_set_elementObj<
		cursor_pointer_elementObj<drag_source_elementObj
					  <drag_destination_elementObj
					   <theme_font_elementObj
					    <focusable_elementObj
					     <label_elementObj<child_elementObj>
					      >>>>,
					  invisible_pointer,
					  dragging_pointer,
					  dragging_wontdrop_pointer>,
		left_to_right_icon,
		right_to_left_icon>,
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
//! editor_implObj takes care of handling keyboard (and button) events,
//! and drawing the blinking cursor by manipulating the richtext API.

class editor_implObj
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

	struct LIBCXX_HIDDEN modifying_text;

	struct LIBCXX_HIDDEN moving_cursor;

	//! The current dragging position

	//! dragging_location() updates this cursor. If we ever drop something,
	//! it'll go here.

	const richtextiterator dragged_pos;

	//! How many characters were deleted by the current operation.

	//! Cleared to 0 in modifying_text's constructor.
	//!
	//! Updated in update_content().
	size_t deleted_count;

	//! How many characters were inserted by the current operation.

	//! Cleared to 0 in modifying_text's constructor.
	//!
	//! Updated in update_content().
	size_t inserted_count;

	//! The current selection for update_content()

	//! Specifies the currently selected text.

	struct current_selection_info {

		//! Current selection's starting position.

		const richtextiterator starting_position;

		//! Current selection's ending position.

		//! Same as starting_position if there's no selection.

		const richtextiterator ending_position;
	};

	//! All changes to the contents of the input field go through here.

	//! Remove and/or inserts the contents of the input field.
	//!
	//! Removes the text given by current_selection_info, and replaces
	//! it with str.
	//!
	//! An insert is done by specifying starting_position=ending_position
	//! in current_selection_info.
	//!
	//! A delete is done by passing an empty str.
	//!
	//! A modifying_text object must be constructed first. This
	//! parameter enforces this contract.
	//!
	//! Immediately invokes update_filtered_content if no filter callback is
	//! installed, otherwise invokes the filter callback, instead.

	void update_content(ONLY IN_THREAD,
			    modifying_text &modifying,
			    const current_selection_info &info,
			    const std::u32string_view &str);

	struct LIBCXX_HIDDEN input_field_filter_info_impl;
	struct LIBCXX_HIDDEN input_field_filter_info_impl_change;
	struct LIBCXX_HIDDEN input_field_filter_info_impl_move;

	//! Apply filtered update.
	void update_filtered_content(ONLY IN_THREAD,
				     const richtextiterator &starting_cursor,
				     const richtextiterator &other,
				     const std::u32string_view &str);

	//! Apply filtered update.

	//! Computes the actual character count in the str and passes it as
	//! cleaned size.
	//!
	//! If password_char is set, all isolation and override markers in
	//! the unicode string get removed.
	void update_filtered_content(ONLY IN_THREAD,
				     richtextiterator starting_cursor,
				     richtextiterator other,
				     const std::u32string_view &str,
				     size_t cleaned_size);

	//! All access to the contents of the input field goes through here.

	//! Returns the characters between the two iterators.
	//! If this is a password field, the characters get
	//! retrieved from the real_string instead.

	richtextstring get_content(const richtextiterator &a,
				   const richtextiterator &b,
				   const std::optional<bidi_format> &embedding);

	//! Whether the last character inserted into a password field is shown

	//! This is the mcguffin for hiding the peek at the last password
	//! character.

	ptr<obj> password_peeking;

 public:

	void clear_password_peek(ONLY IN_THREAD);

 protected:
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

		protected:

			//! The lock on the underlying rich text object.

			internal_richtext_impl_t::lock internal_lock;

			//! The cursor reference.

			richtextiteratorptr &cursor;
		public:
			//! Constructor
			const_lock(editor_implObj &impl);

			//! Destructor
			~const_lock();

			richtextiteratorptr cursor_pos() const;

			richtext_draw_info get_richtext_draw_info(
				editor_implObj &me)
				const;
		};

		class lock : public const_lock {

		public:

			using const_lock::internal_lock;

			using const_lock::cursor;

			//! Constructor
			lock(ONLY IN_THREAD, editor_implObj &impl,
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

	//! Callback
	functionptr<input_field_filter_callback_t> on_filter_thread_only;

	//! My parent peephole
	const ref<editor_peephole_implObj> parent_peephole;

	//! Whether we just autoselect-ed the entire contents of the field

	//! Set when input focus is gained, and the whole thing is selected.
	//! Cleared by a button event or any processed key event.

	bool autoselected=false;

	//! Whether insert text direction has been overridden

	unicode_bidi_level_t bidi_override=UNICODE_BIDI_SKIP;

	//! Set the bidi_override and show the icon tooltip popup.

	void set_bidi_override(ONLY IN_THREAD,
			       unicode_bidi_level_t new_bidi_override,
			       const icon &tooltip_icon,
			       const input_mask &mask);

	//! Override hide_popups()

	//! Calls clear_bidi_override
	void hide_popups(ONLY IN_THREAD) override;

	//! Clear the bidi override and hide the icon tooltip popup
	void clear_bidi_override(ONLY IN_THREAD);

 private:
	popupptr direction_attached_popup_thread_only;
 public:
	THREAD_DATA_ONLY(direction_attached_popup);

	//! Override update_attachedto_info

	//! The default implementation updates the attached-to popup's
	//! position based on the element position.
	//!
	//! Bidi tooltips are attached to the cursor position. Here we
	//! check if the bidi tooltip is being shown, and update its
	//! position ourselves.

	void update_attachedto_info(ONLY IN_THREAD) override;

	//! Report the current cursor position is a pseudo-motion event.

	//! Tooltips are attached to the current cursor position, so we
	//! simulate a motion event at the current cursor position.

	void report_cursor_pos_as_motion_event(ONLY IN_THREAD);

 public:
	//! Override direction()

	//! Use the bidi override, if it's in place. Otherwise return the
	//! default one.

	bidi direction(ONLY IN_THREAD) override;

 private:
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
	THREAD_DATA_ONLY(on_filter);

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
		bidi_format directional_format;

		const_tooltip_appearance direction_tooltip_appearance;

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

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme) override;

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
		bool validated;
		const input_field_config &config;
		const input_field_validation_callbackptr &validation_callback;
		const icon &left_to_right_icon;
		const icon &right_to_left_icon;
		current_theme_t::lock theme_lock;

		// Forwarded to the internal label implementation constructor:
		label_config label_config_args;
		textlabel_config textlabel_config_args;

		//! init_args also constructs some additional objects.

		init_args(const ref<editor_peephole_implObj> &parent_peephole,
			  const text_param &text,
			  bool validated,
			  const input_field_config &config,
			  const input_field_validation_callbackptr
			  &validation_callback,
			  const icon &left_to_right_icon,
			  const icon &right_to_left_icon);


		//! Default metadata.
		richtextmeta default_meta;

		//! Default metadata for the input field hint
		richtextmeta hint_meta;

		~init_args();
	};

	//! Constructor

	//! Construct the init_args, then pass it here.

	editor_implObj(init_args &args);

	//! Destructor
	~editor_implObj();

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
			  size_t inserted,
			  const callback_trigger_t &trigger);

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

	struct LIBCXX_HIDDEN delete_selection_info;

	//! Delete the current selection.

	//! Constructed to begin the process of deleting the current selection.
	//! This is always constructed on the stack.
	//!
	//! If there is a current selection, the destructor clears it.

	struct delete_selection_info : current_selection_info {

		//! Constructed on the stack by the connection thread.
		ONLY IN_THREAD;

		//! Who created me

		editor_implObj &me;

		//! The lock on me's cursor.

		selection_cursor_t::lock &cursor_lock;

		//! Whether there was a selection.

		//! After the delete operation executes the starting and ending
		//! iterator positions are the same. This remembers if they
		//! were different when this was constructed.
		const bool had_selection;

		//! Constructor

		delete_selection_info(ONLY IN_THREAD,
				      editor_implObj &me,
				      modifying_text &modifying);

	private:

		//! Delegated constructor
		delete_selection_info(ONLY IN_THREAD,
				      editor_implObj &me,
				      modifying_text &modifying,
				      const richtextiterator &starting_position,
				      const richtextiterator &ending_position);
	public:
		//! Destructor deletes the current selection, if there is one.
		~delete_selection_info();
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
		    const std::u32string_view &str,
		    const callback_trigger_t &trigger);

	//! Move cursor to the beginning of the text.
	bool to_begin(ONLY IN_THREAD, const input_mask &mask,
		      const callback_trigger_t &trigger);

	//! Move cursor to the end of the text.
	bool to_end(ONLY IN_THREAD, const input_mask &mask,
		    const callback_trigger_t &trigger);

	//! Select the entire text.
	void select_all(ONLY IN_THREAD,
			const callback_trigger_t &trigger);

	//! What happens when DEL is pressed.
	void delete_char_or_selection(ONLY IN_THREAD,
				      const input_mask &mask,
				      const callback_trigger_t &trigger);

	//! Delete a character at cursor position.

	void delete_char(ONLY IN_THREAD, modifying_text &modifying);

	//! Common logic shared by del_char_or_selection and backspace

	void delete_char(ONLY IN_THREAD, modifying_text &modifying,
			 const richtextiterator &delete_cursor);
 public:

	//! Return the current contents  of the editor.

	std::u32string get(const std::optional<bidi_format> &embedding);

	//! Return the size of the input field.
	size_t size() const;

	//! Return the current cursor position.
	std::tuple<richtextiterator, richtextiterator> pos();

 private:
	//! Return the current cursor position.
	std::tuple<richtextiterator, richtextiterator>
		pos(selection_cursor_t::const_lock &);
 public:

	//! Set the current contents of the editor

	void set(const std::u32string_view &str, bool validated);

	//! Set the current contents of the editor

	void lock_and_set(ONLY IN_THREAD, const std::u32string_view &string,
			  bool validated);

	//! Set the current contents of the editor

	void set(ONLY IN_THREAD, const std::u32string &string, bool validated,
		 const callback_trigger_t &trigger);

	//! Set the current contents of the editor
	virtual void set(ONLY IN_THREAD, const std::u32string &string,
			 size_t cursor_pos, size_t selection_pos,
			 const callback_trigger_t &trigger);

	//! Whether we're cool with giving up focus.
	bool ok_to_lose_focus(ONLY IN_THREAD,
			      const callback_trigger_t &trigger) override;

 private:
	//! Whether the contents of the input field have been modified since they were validated.

	bool validation_required_thread_only;

 public:
	//! Invoke validation_callback if validation_required.

	bool validate_modified(ONLY IN_THREAD, const callback_trigger_t &);
 private:
	//! The validation callback.

	input_field_validation_callbackptr validation_callback_thread_only;

 public:
	THREAD_DATA_ONLY(validation_required);
	THREAD_DATA_ONLY(validation_callback);

	//! Install a new search callback

	//! The default implementation throws an exception.
	//!
	//! Overridden by editor_search_implObj.

	virtual void on_search(ONLY IN_THREAD,
			       const input_field_config::search_info &);
};

LIBCXXW_NAMESPACE_END

#endif
