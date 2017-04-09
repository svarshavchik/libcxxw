/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_impl_h
#define editorimpl_h

#include "editorobj.H"
#include "label.H"
#include "fonts/current_fontcollectionfwd.H"
#include "focus/focusable_elementfwd.H"
#include "richtext/richtextiteratorfwd.H"
#include "x/w/input_field_config.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN editorObj::implObj
	: public focusable_elementObj<labelObj::implObj> {

	//! My superclass alias.
	typedef focusable_elementObj<labelObj::implObj> superclass_t;

	//! Mcguffin for the scheduled cursor blink callback.
	ptr<obj> blinking;

	//! The current cursor position.
	const richtextiterator cursor;

 public:

	//! Editing font.
	const current_fontcollection font;

	//! This editor's configuration.

	const input_field_config config;

	//! Multiply columns by font's nominal width.

	dim_t nominal_width(IN_THREAD_ONLY) const;

	//! Contsructor
	implObj(const ref<containerObj::implObj> &container,
		const text_param &text,
		const input_field_config &config);

 private:
	//! Internal constructor

	implObj(const ref<containerObj::implObj> &container,
		richtextstring &&string,
		const input_field_config &config);

 public:
	//! Destructor
	~implObj();

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

	//! Override theme_updated
	void theme_updated(IN_THREAD_ONLY) override;

	//! Inherited from elementObj::implObj

	//! Starts or stops the cursor blink.

	void keyboard_focus(IN_THREAD_ONLY,
			    focus_change event,
			    const ref<elementObj::implObj> &ptr) override;

	//! Inherited from elementObj::implObj

	//! Processes a key event.
	bool process_key_event(IN_THREAD_ONLY, const key_event &ke);
 private:

	//! Set a callback for the next cursor blink on/off.
	void schedule_blink(IN_THREAD_ONLY);

	//! Blink the cursor, on or off.
	void blink(IN_THREAD_ONLY);

	//! Whether the cursor is blinked on.
	bool blinkon;

	//! If the cursor is currently blinked on, blink it off.
	void unblink(IN_THREAD_ONLY);

	//! Draw changed text fragments.
	void draw_changes(IN_THREAD_ONLY);

	//! Process key press.
	bool process_keypress(IN_THREAD_ONLY, const key_event &);

	//! ensure_visibility() of the cursor.
	void scroll_cursor_into_view(IN_THREAD_ONLY);

};

LIBCXXW_NAMESPACE_END

#endif
