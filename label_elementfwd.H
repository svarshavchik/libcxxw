/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_elementfwd_H
#define label_elementfwd_H

#include "textlabel.H"
#include "x/w/impl/element.H"
#include "x/w/text_paramfwd.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/rgb.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
class LIBCXX_HIDDEN label_elementObj;

//! A mixin for inheriting from a textlabelObj::implObj.

//! The template parameter must be a child_elementObj, or one of its
//! subclasses.
//!
//! Multiply inherits from the template parameter, and textlabelObj::implObj.

template<typename element_impl>
class label_elementObj : public textlabelObj::implObj,
			 public element_impl {

public:

	//! Constructor

	//! Note that the textlabel_config gets passed by reference.
	label_elementObj(const container_impl &parent_container,
			 const text_param &text,
			 textlabel_config &config);

	//! Constructor used by the editor subclass.

	//! Note that the textlabel_config gets passed by reference.
	label_elementObj(const container_impl &container,
			 textlabel_config &config,
			 bidi_format directional_format,
			 const const_defaulttheme &theme,
			 richtextstring &&string);

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
	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme) override;

	//! Invoke the underlying text object's do_draw().

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas) override;

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


	//! Override set_minimum_override()

	//! Forwarded to textlabelObj::implObj, so it is aware of it for
	//! the purposes of calculate_current_metrics().

	void set_minimum_override(ONLY IN_THREAD,
				  dim_t horiz_override,
				  dim_t vert_override) override;
};

LIBCXXW_NAMESPACE_END

#endif
