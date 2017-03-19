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

public:

	//! The text in the label.

	const richtext text;

	implObj(const ref<containerObj::implObj> &container,
		const text_param &text,
		halign alignment);

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
 private:

	//! Recalculate and update our metrics.

	void recalculate(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
