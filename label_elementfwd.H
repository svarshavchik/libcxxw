/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_elementfwd_H
#define label_elementfwd_H

#include "textlabel.H"
#include "x/w/impl/element.H"
#include "x/w/text_paramfwd.H"
#include "x/w/rgb.H"

LIBCXXW_NAMESPACE_START

//! A mixin for inheriting from a textlabelObj::implObj.

//! The template parameter must be a child_elementObj, or one of its
//! subclasses.
//!
//! Multiply inherits from the template parameter, and textlabelObj::implObj.

template<typename element_impl>
class LIBCXX_HIDDEN label_elementObj : public element_impl,
				       public textlabelObj::implObj {

public:

	//! Constructor

	label_elementObj(const container_impl &parent_container,
			 const text_param &text,
			 halign alignment,
			 double initial_width,
			 bool allow_links);

	//! Constructor used by the editor subclass.

	label_elementObj(const container_impl &container,
			 halign alignment,
			 double initial_width,
			 richtextstring &&string,
			 const richtextmeta &default_meta,
			 bool allow_links,
			 const char *element_id,
			 const color_arg &initial_background_color);

	//! Destructor
	~label_elementObj();

	//! Implement get_label_element_impl()

	elementObj::implObj &get_label_element_impl() override;

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj::implObj

	//! Calculate the metrics of the initial contents of this label.
	void initialize(ONLY IN_THREAD) override;

	//! Invoke the underlying text object's theme_updated().

	//! When done, recalculate() our updated size.
	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;

	//! Invoke the underlying text object's do_draw().

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectangle_set &areas) override;

	//! Override process_updated_position()

	//! Call the superclass, then rewrap_due_to_updated_position().
	void process_updated_position(ONLY IN_THREAD)
		override;

	//! Override process_buton_event().

	//! Detect clicks over hotspots.

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Override process_key_event()

	//! Keyboard navigation over hotspots.
	bool process_key_event(ONLY IN_THREAD, const key_event &ke) override;

	//! Override report_motion-event

	//! Detect pointer movement over hotspots.
	void report_motion_event(ONLY IN_THREAD, const motion_event &)
		override;

	//! Override pointer_focus()

	//! If no longer have pointer focus, disable any active hotspots.
	void pointer_focus(ONLY IN_THREAD,
			   const callback_trigger_t &trigger) override;
};

LIBCXXW_NAMESPACE_END

#endif
