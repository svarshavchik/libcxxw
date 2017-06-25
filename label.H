/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_h
#define label_h

#include "x/w/labelobj.H"
#include "x/w/text_param.H"
#include "child_elementobj.H"
#include "richtext/richtextfwd.H"

LIBCXXW_NAMESPACE_START

//! Base class for a label implementation.

class LIBCXX_HIDDEN labelObj::implObj : public child_elementObj {

	//! Word wrapping width, in millimeters.

	//! A width of 0 means don't word-wrap the text.
	double word_wrap_widthmm_thread_only;

 protected:
	//! Preferred width.

	//! This is word_wrap_widthmm converted to pixels.
	dim_t preferred_width=0;

	//! Compute preferred_width

	virtual void compute_preferred_width(IN_THREAD_ONLY);
public:
	THREAD_DATA_ONLY(word_wrap_widthmm);

	//! The text in the label.

	const richtext text;

	//! Constructor for a label element.

	implObj(const ref<containerObj::implObj> &container,
		const text_param &text,
		halign alignment,
		double initial_width);

 private:
	//! Internal constructor

	implObj(const ref<containerObj::implObj> &container,
		const text_param &text,
		halign alignment,
		double initial_width,
		elementObj::implObj &container_element);

 protected:
	//! Constructor used by editor subclass.

	implObj(const ref<containerObj::implObj> &container,
		halign alignment,
		double initial_width,
		const richtextstring &string,
		const char *element_id);
 public:
	//! Destructor
	~implObj();

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

 private:

	//! Potentially re-wordwrap the label.

	//! After a label is resized we will need to re-wordwrap it, perhaps.
	//!
	//! Overriden in editor_impl, which uses fixed metrics, and does not
	//! do a resize.

	virtual void rewrap_due_to_updated_position(IN_THREAD_ONLY);

	//! Rewrap text to word_wrap_width

	//! Overridden in editor_impl to rewrap according to preferred_width.

	virtual bool rewrap(IN_THREAD_ONLY);

 protected:
	//! Recalculate and update our metrics.

	//! Invoke calculate_current_metrics(), then set_element_metrics().

	void recalculate(IN_THREAD_ONLY);

	//! Calculate current text metrics.

	//! Invokes text->get_metrics().
	//!
	//! Overridden in editor_impl to set the metrics strictly according
	//! to the preferred width.

	virtual std::pair<metrics::axis, metrics::axis>
		calculate_current_metrics(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
