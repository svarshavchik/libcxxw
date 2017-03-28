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

	//! Preferred width.

	//! This is word_wrap_widthmm converted to pixels.
	dim_t preferred_width=0;

	//! Compute preferred_width

	void compute_preferred_width(IN_THREAD_ONLY);
public:
	THREAD_DATA_ONLY(word_wrap_widthmm);

	//! The text in the label.

	const richtext text;

	//! Constructor

	implObj(const ref<containerObj::implObj> &container,
		const text_param &text,
		halign alignment,
		double initial_width);

	//! Destructor
	~implObj();

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj::implObj

	//! Calculate the metrics of the initial contents of this label.
	void initialize(IN_THREAD_ONLY) override;

	//! Invoke the underlying text object's theme_updated().

	//! When done, recalculate() our updated size.
	void theme_updated(IN_THREAD_ONLY) override;

	//! Invoke the underlying text object's do_draw().

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas) override;

	//! Override process_updated_position()

	//! Once this label is resized we will need to re-wordwrap it, perhaps.
	void process_updated_position(IN_THREAD_ONLY)
		override;

 private:

	//! Rewrap text to word_wrap_width

	bool rewrap(IN_THREAD_ONLY);

	//! Recalculate and update our metrics.

	void recalculate(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
