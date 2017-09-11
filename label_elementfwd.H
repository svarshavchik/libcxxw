/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_elementfwd_H
#define label_elementfwd_H

#include "textlabel.H"
#include "element.H"
#include "x/w/text_paramfwd.H"

LIBCXXW_NAMESPACE_START

//! A mixin for inheriting from a textlabelObj::implObj.

//! The template parameter must be a child_elementObj, or one of its
//! subclasses.
//!
//! Multiply inherits from the template parameter, and textlabelObj::implObj.

template<typename element_impl>
class label_elementObj : public element_impl,
			 public textlabelObj::implObj {

public:

	//! Constructor

	label_elementObj(const ref<containerObj::implObj> &parent_container,
			 const text_param &text,
			 halign alignment,
			 double initial_width,
			 bool allow_links);

	//! Constructor used by the editor subclass.

	label_elementObj(const ref<containerObj::implObj> &container,
			 halign alignment,
			 double initial_width,
			 richtextstring &&string,
			 const richtextmeta &default_meta,
			 bool allow_links,
			 const char *element_id);

	//! Destructor
	~label_elementObj();

	//! Implement get_label_element_impl()

	elementObj::implObj &get_label_element_impl() override;

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj::implObj

	//! Calculate the metrics of the initial contents of this label.
	void initialize(IN_THREAD_ONLY) override;

	//! Invoke the underlying text object's theme_updated().

	//! When done, recalculate() our updated size.
	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;

	//! Invoke the underlying text object's do_draw().

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas) override;

	//! Override process_updated_position()

	//! Call the superclass, then rewrap_due_to_updated_position().
	void process_updated_position(IN_THREAD_ONLY)
		override;

	//! Override set_inherited_visibility()

	//! Call the superclass, then rewrap_due_to_updated_position()

	void set_inherited_visibility(IN_THREAD_ONLY,
				      inherited_visibility_info
				      &visibility_info) override;

	//! Override process_buton_event().

	//! Detect clicks over hotspots.

	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Override process_key_event()

	//! Keyboard navigation over hotspots.
	bool process_key_event(IN_THREAD_ONLY, const key_event &ke) override;

	//! Override report_motion-event

	//! Detect pointer movement over hotspots.
	void report_motion_event(IN_THREAD_ONLY, const motion_event &)
		override;

	//! Override pointer_focus()

	//! If no longer have pointer focus, disable any active hotspots.
	void pointer_focus(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
