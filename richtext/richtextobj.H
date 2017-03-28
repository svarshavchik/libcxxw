/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextobj_H
#define richtextobj_H

#include "connection_threadfwd.H"
#include "element_draw.H"
#include "richtext/richtextfwd.H"
#include "richtext/richtextstring.H"
#include "x/w/types.H"
#include "x/w/metrics/axis.H"
#include <x/obj.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! A \ref richtext "rich text" object.

class LIBCXX_HIDDEN richtextObj : virtual public obj {


 public:

	class implObj;

 private:
	//! The implementation object is mutex-protected.

	class impl_t : public mpobj<ref<implObj>> {

	public:

		using mpobj::mpobj;

		class lock : public mpobj<ref<implObj>>::lock {

		public:
			lock(IN_THREAD_ONLY, impl_t &);
		};
	};

	//! The implementation object.

	impl_t impl;

	//! Whether this rich text object is word-wrapped.

	//! If word_wrap_width is 0, this richtext object is not wrapped
	//! (each paragraph consists of a single line); otherwise this is the
	//! targeted word wrap width, in pixels.

	dim_t word_wrap_width_thread_only;

 public:

	THREAD_DATA_ONLY(word_wrap_width);

	//! Constructor
	richtextObj(const ref<implObj> &impl,
		    dim_t word_wrap_width);

	//! Destructor
	~richtextObj();

	//! Rewrap the text to a new width.

	//! Updated word_wrap_width. A width of 0 un-wordwraps the label.

	bool rewrap(IN_THREAD_ONLY, dim_t width);

	//! Return the text object's current metrics

	//! Return the text object's current horizontal and vertical sizes.

	std::pair<metrics::axis, metrics::axis>
		get_metrics(IN_THREAD_ONLY, dim_t preferred_width);

	//! Theme has been updated

	//! Forwarded to the implementation object.

	void theme_updated(IN_THREAD_ONLY);

	//! Set the entire contents of the richtext.

	void set(IN_THREAD_ONLY, richtextstring &string);

	//! Draw the text

	void do_draw(IN_THREAD_ONLY,
		     //! The element being drawn
		     element_drawObj &element,

		     //! The drawn element's draw_info
		     const draw_info &di,

		     //! Whether to forcibly redraw everything.

		     //! If \c false, only \c redraw_needed fragments get
		     //! redrawn.

		     bool force);

	//! For testing purpose.

	ref<implObj> debug_get_impl(IN_THREAD_ONLY);
};


LIBCXXW_NAMESPACE_END;
#endif
