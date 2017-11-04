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
#include "richtext/richtext_draw_boundariesfwd.H"
#include "x/w/types.H"
#include "x/w/metrics/axis.H"
#include "x/w/rectanglefwd.H"
#include "x/w/drawablefwd.H"
#include "x/w/input_field_lock.H"
#include <x/obj.H>
#include <x/mpobj.H>
#include <x/functional.H>
#include <utility>

LIBCXXW_NAMESPACE_START

//! A \ref richtext "rich text" object.

class LIBCXX_HIDDEN richtextObj : virtual public obj {


 public:

	//! The implementation object is mutex-protected.

	class impl_t : public internal_richtext_impl_t {

	public:

		using internal_richtext_impl_t::internal_richtext_impl_t;

		class lock;
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
	richtextObj(const richtextstring &string,
		    halign alignment,
		    dim_t word_wrap_width);

	//! Constructor
	richtextObj(const ref<richtext_implObj> &impl,
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

	void set(IN_THREAD_ONLY, const richtextstring &string);

	//! Redraw this element fully.

	//! Invoked from do_draw(), which handles exposure events.

	void full_redraw(IN_THREAD_ONLY,
			 element_drawObj &,
			 const richtext_draw_info &,
			 const draw_info &,
			 const rectangle_set &);

	//! Redraw this element fully.

	//! This is used by the rich text layout manager that positions
	//! multiple text elements inside its element. It supplies a custom
	//! richtext_draw_boundaries.

	void full_redraw(IN_THREAD_ONLY,
			 element_drawObj &,
			 const richtext_draw_info &,
			 const draw_info &,
			 const richtext_draw_boundaries &);

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

	//! This is used by the editor element, as such this takes care of
	//! constructing richtext_draw_boundaries that assumes that the
	//! text label is the entire display element (it is).
	void redraw_between(IN_THREAD_ONLY,
			    element_drawObj &,
			    const richtextiterator &,
			    const richtextiterator &,
			    const richtext_draw_info &,
			    const draw_info &);

	//! Set logical text width

	//! If unset, the logical text width is always the same as the actual
	//! text width. The logical text width is used by the
	//! listlayoutmanager to position a text cell, so that if it is
	//! right-aligned, it gets rendered flush to the right margin of its
	//! column, since right-alignment based on the actual text width is
	//! not going to accomplish anything useful.

	void text_width(const std::optional<dim_t> &s);

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
		  const richtext_draw_boundaries &draw_bounds);
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
		internal_richtext_impl_t::lock lock{impl};

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

	void insert_at_location(IN_THREAD_ONLY,
				impl_t::lock &lock,
				const richtext_insert_base &new_text);


	//! Remove text.

	//! Invoked from richtextiterator->insert() after a thread_lock().

	void remove_at_location(IN_THREAD_ONLY,
				impl_t::lock &lock,
				const richtextcursorlocation &a,
				const richtextcursorlocation &b);

	//! Combine remove with insert.

	void replace_at_location(IN_THREAD_ONLY,
				 impl_t::lock &lock,
				 const richtext_insert_base &new_text,
				 const richtextcursorlocation &remove_from,
				 const richtextcursorlocation &remove_to);

	//! Calculate a location's position'

	static size_t pos(const internal_richtext_impl_t::lock &,
			  const richtextcursorlocation &);

	//! Get text between two locations.

	//! Invoked from richtextiterator->get() after a read_only_lock().

	static void get(const internal_richtext_impl_t::lock &,
			richtextstring &,
			const richtextcursorlocation &,
			const richtextcursorlocation &);

 private:
	//! Return an iterator to the given character # in the rich text.

	richtextiterator at(internal_richtext_impl_t::lock &lock, size_t npos);
 public:
	//! For testing purpose.

	ref<richtext_implObj> debug_get_impl(IN_THREAD_ONLY);
};


//! IN_THREAD has full rights on the underlying object.
class richtextObj::impl_t::lock : public internal_richtext_impl_t::lock {


public:
	lock(IN_THREAD_ONLY, impl_t &);
};

LIBCXXW_NAMESPACE_END;
#endif
