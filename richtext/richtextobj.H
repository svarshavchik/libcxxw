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
#include "richtext/richtextiteratorfwd.H"
#include "richtext/richtextcursorlocation.H"
#include "richtext/richtext_insertfwd.H"
#include "richtext/richtext_draw_infofwd.H"
#include "x/w/types.H"
#include "x/w/metrics/axis.H"
#include "x/w/rectanglefwd.H"
#include "x/w/drawablefwd.H"
#include <x/obj.H>
#include <x/mpobj.H>
#include <x/functional.H>

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

		//! Read lock, on an honor system.
		typedef mpobj<ref<implObj>>::lock read_only_lock;

		//! IN_THREAD has full rights on the underlying object.
		class lock : public read_only_lock {

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

	//! Return the text object's current width
	dim_t get_width(IN_THREAD_ONLY);

	//! Return the text object's current metrics

	//! Return the text object's current horizontal and vertical sizes.

	std::pair<metrics::axis, metrics::axis>
		get_metrics(IN_THREAD_ONLY, dim_t preferred_width,
			    bool visible);

	//! Theme has been updated

	//! Forwarded to the implementation object.

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme);

	//! Set the entire contents of the richtext.

	void set(IN_THREAD_ONLY, richtextstring &string);

	//! Redraw this element fully.

	//! Invoked from do_draw(), which handles exposure events.

	void full_redraw(IN_THREAD_ONLY,
			 element_drawObj &,
			 const richtext_draw_info &,
			 const draw_info &,
			 const rectangle_set &);

	//! Redraw only the fragments that need redrawing.

	//! Invoked after the text is modified.

	void redraw_whatsneeded(IN_THREAD_ONLY,
				element_drawObj &,
				const richtext_draw_info &,
				const draw_info &);

	//! Create a picture containing this text

	//! This constructs a pixmap and a picture, then renders the text
	//! into the picture.
	std::tuple<pixmap, picture> create(IN_THREAD_ONLY,
					   const drawable &for_drawable);

	//! Redraw only the fragments that need redrawing.

	//! Invoked after the text is modified. The rectangleset further
	//! narrows down what needs to be redrawn.

	void redraw_whatsneeded(IN_THREAD_ONLY,
				element_drawObj &,
				const richtext_draw_info &,
				const draw_info &,
				const rectangle_set &);

	//! Redraw a fragment range.

	void redraw_between(IN_THREAD_ONLY,
			    element_drawObj &,
			    const richtextiterator &,
			    const richtextiterator &,
			    const richtext_draw_info &,
			    const draw_info &);
 private:
	//! Draw the text

	void draw(IN_THREAD_ONLY,
		  //! The element being drawn
		  element_drawObj &element,

		  //! Additional drawing information

		  //! Such as whether any part of the text is "selected".
		  const richtext_draw_info &rdi,

		  //! The drawn element's draw_info
		  const draw_info &di,

		  //! Whether to redraw the given fragment

		  //! full_redraw() always returns true. redraw_whatsneeded
		  //! checks the fragment's redraw_needed.
		  const function<bool (richtextfragmentObj *)> &redraw_fragment,

		  //! Whether to clear the area below the last fragment.

		  //! full_redraw() sets this to true. redraw_whatsneeded
		  //! sets it to false.

		  bool clear_padding,

		  //! Which areas to redraw
		  const rectangle_set &areas
		  );
 public:
	//! Return an iterator to the first character of the rich text.

	richtextiterator begin();

	//! Return an iterator to the last character of the rich text.

	//! Note that this ending iterator is not "one-past". Iterators are
	//! used with richtext objects used for input fields, which append
	//! an extra space character at the end, and this is what this returns.

	richtextiterator end();

	//! Return an iterator to the given character # in the rich text.

	richtextiterator at(size_t npos);

	//! Run something while protected by a read only lock.

	//! This is used by richtextiterator to gain read-only access (on an
	//! honor system) to the underlying implementation object.
	//!
	//! Here, "read-only" means: no access to the horiz_info structure of
	//! any text fragment. It is accessible IN_THREAD_ONLY.

	template<typename lambda>
		auto read_only_lock(lambda &&l)
	{
		impl_t::read_only_lock lock{impl};

		return l(lock);
	}

	//! Run something while protected by the big thread lock.

	//! This must be used by any IN_THREAD-scoped execution path.

	template<typename lambda>
		auto thread_lock(IN_THREAD_ONLY, lambda &&l)
	{
		impl_t::lock lock{IN_THREAD, impl};

		return l(IN_THREAD, lock);
	}

	//! Insert text.

	//! Invoked from richtextiterator->insert() after a thread_lock().
	//! Passes a richtextcursorlocation that must have the
	//! do_not_adjust_in_insert flag set.
	//!
	//! See richtextfragmentObj::insert() for the description of the
	//! return value.

	size_t insert_at_location(IN_THREAD_ONLY,
				  impl_t::lock &lock,
				  const richtext_insert_base &new_text);


	//! Remove text.

	//! Invoked from richtextiterator->insert() after a thread_lock().

	void remove_at_location(IN_THREAD_ONLY,
				impl_t::lock &lock,
				const richtextcursorlocation &a,
				const richtextcursorlocation &b);

	//! Calculate a location's position'

	static size_t pos(const impl_t::read_only_lock &,
			  const richtextcursorlocation &);

	//! Get text between two locations.

	//! Invoked from richtextiterator->get() after a read_only_lock().

	static void get(const impl_t::read_only_lock &,
			richtextstring &,
			const richtextcursorlocation &,
			const richtextcursorlocation &);

 private:
	//! Return an iterator to the given character # in the rich text.

	richtextiterator at(impl_t::read_only_lock &lock, size_t npos);
 public:
	//! For testing purpose.

	ref<implObj> debug_get_impl(IN_THREAD_ONLY);
};


LIBCXXW_NAMESPACE_END;
#endif
