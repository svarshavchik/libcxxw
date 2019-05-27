/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_richtext_richtextobj_H
#define x_w_impl_richtext_richtextobj_H

#include <x/w/connection_threadfwd.H>
#include <x/w/impl/element_draw.H>
#include <x/w/impl/richtext/richtextstringfwd.H>
#include <x/w/impl/richtext/richtextfwd.H>
#include <x/w/impl/richtext/richtextfragmentfwd.H>
#include <x/w/impl/richtext/richtextiteratorfwd.H>
#include <x/w/impl/richtext/richtextcursorlocationfwd.H>
#include <x/w/impl/richtext/richtext_insertfwd.H>
#include <x/w/impl/richtext/richtext_draw_infofwd.H>
#include <x/w/impl/richtext/richtext_draw_boundariesfwd.H>
#include <x/w/types.H>
#include <x/w/alignment.H>
#include <x/w/metrics/axis.H>
#include <x/w/rectanglefwd.H>
#include <x/w/drawablefwd.H>
#include <x/w/input_field_lock.H>
#include <x/w/defaultthemefwd.H>
#include <x/obj.H>
#include <x/mpobj.H>
#include <x/functional.H>
#include <utility>

LIBCXXW_NAMESPACE_START

//! A \ref richtext "rich text" object.

class richtextObj : virtual public obj {


 public:

	class LIBCXX_PUBLIC impl_t;

	//! The implementation object is mutex-protected.

	class impl_t : public internal_richtext_impl_t {

	public:

		using internal_richtext_impl_t::internal_richtext_impl_t;

		class lock;
	};

	//! The implementation object.

	impl_t impl;

 public:

	//! Constructor
	richtextObj(const richtextstring &string,
		    halign alignment,
		    dim_t word_wrap_width);

 private:
	//! Internal constructor
	richtextObj(const ref<richtext_implObj> &impl,
		    dim_t word_wrap_width);
 public:
	//! Destructor
	~richtextObj();

	//! Rewrap the text to a new width.

	//! Updated word_wrap_width. A width of 0 un-wordwraps the label.

	bool rewrap(dim_t width);

	//! Return the text object's current width
	dim_t get_width();

	//! Return the text object's current metrics

	//! Return the text object's current horizontal and vertical sizes.

	std::pair<metrics::axis, metrics::axis>
		get_metrics(dim_t preferred_width);

	//! Theme has been updated

	//! Forwarded to the implementation object.

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme);

	//! Set the entire contents of the richtext.

	void set(ONLY IN_THREAD, const richtextstring &string);

	//! Redraw this widget fully.

	//! Invoked from do_draw(), which handles exposure events.

	void full_redraw(ONLY IN_THREAD,
			 element_drawObj &,
			 const richtext_draw_info &,
			 const draw_info &,
			 const rectarea &);

	//! Redraw this widget fully.

	//! This is used by the rich text layout manager that positions
	//! multiple text elements inside its widget. It supplies a custom
	//! richtext_draw_boundaries.
	//!
	//! The richtext_draw_boundaries parameter is mutable. full_redraw()
	//! calls draw(), which may temporarily modify the boundaries in
	//! order to attach an ellipsis to a truncated label.
	void full_redraw(ONLY IN_THREAD,
			 element_drawObj &,
			 const richtext_draw_info &,
			 const draw_info &,
			 clip_region_set &,
			 richtext_draw_boundaries &);

	//! Redraw only the fragments that need redrawing.

	//! Invoked after the text is modified.

	void redraw_whatsneeded(ONLY IN_THREAD,
				element_drawObj &,
				const richtext_draw_info &,
				const draw_info &);

	//! Create a picture containing this text

	//! This constructs a pixmap and a picture, then renders the text
	//! into the picture.
	std::tuple<pixmap, picture> create(ONLY IN_THREAD,
					   const drawable &for_drawable);

	//! Redraw only the fragments that need redrawing.

	//! Invoked after the text is modified. The rectangleset further
	//! narrows down what needs to be redrawn.

	void redraw_whatsneeded(ONLY IN_THREAD,
				element_drawObj &,
				const richtext_draw_info &,
				const draw_info &,
				const rectarea &);

	//! Redraw a fragment range.

	//! This is used by the editor element, as such this takes care of
	//! constructing richtext_draw_boundaries that assumes that the
	//! text label is the entire widget (it is).
	void redraw_between(ONLY IN_THREAD,
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

	//! The minimum width override is slightly different semantics

	//! The minimum width override sets the minimum width of the computed
	//! text.
	void minimum_width_override(ONLY IN_THREAD, dim_t);
 private:

	//! Most recently rendered height.
	size_t rendered_height=0;

	//! Draw the text

	void draw(//! Invoked only in the connection thread.

		  ONLY IN_THREAD,
		  //! The widget being drawn
		  element_drawObj &element,

		  //! Additional drawing information

		  //! Such as whether any part of the text is "selected".
		  const richtext_draw_info &rdi,

		  //! The drawn element's draw_info
		  const draw_info &di,

		  //! Element clipping region must be established already.
		  clip_region_set &clipped,

		  //! Whether to redraw the given fragment

		  //! full_redraw() always returns true. redraw_whatsneeded
		  //! checks the fragment's redraw_needed.
		  const function<bool (richtextfragmentObj *)> &redraw_fragment,

		  //! Whether to clear the area below the last fragment.

		  //! full_redraw() sets this to true. redraw_whatsneeded
		  //! sets it to false.

		  bool clear_padding,

		  //! Which areas to redraw

		  //! This parameter is mutable. We may temporarily modify the
		  //! boundaries in order to attach an ellipsis to a truncated
		  //! label.
		  richtext_draw_boundaries &draw_bounds);

	//! Draw the text

	//! An additional richtextfragmentObj * pointer points to the
	//! text fragment from the ellipsis string, if the drawing area is
	//! to short. The drawn text is cut off even further, to make room
	//! for the ellipsis which is drawn immediately after it.

	void draw_with_ellipsis(ONLY IN_THREAD,
				element_drawObj &element,
				const richtext_draw_info &rdi,
				const draw_info &di,
				clip_region_set &clipped,
				const function<bool (richtextfragmentObj *)>
				&redraw_fragment,
				bool clear_padding,
				richtext_draw_boundaries &draw_bounds,
				richtextfragmentObj *ellipsis_fragment);

	struct draw_fragment_info;
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
	//! any text fragment. It is accessible ONLY IN_THREAD.

	template<typename lambda>
		auto read_only_lock(lambda &&l)
	{
		internal_richtext_impl_t::lock lock{impl};

		return l(lock);
	}

	//! How many characters there are.

	size_t size(ONLY IN_THREAD);

	//! Run something while protected by the big thread lock.

	//! This must be used by any IN_THREAD-scoped execution path.

	template<typename lambda>
		auto thread_lock(ONLY IN_THREAD, lambda &&l)
	{
		impl_t::lock lock{impl};

		return l(IN_THREAD, lock);
	}

	//! Insert text.

	//! Invoked from richtextiterator->insert() after a thread_lock().
	//! Passes a richtextcursorlocation that must have the
	//! do_not_adjust_in_insert flag set.

	void insert_at_location(ONLY IN_THREAD,
				impl_t::lock &lock,
				const richtext_insert_base &new_text);


	//! Remove text.

	//! Invoked from richtextiterator->insert() after a thread_lock().

	void remove_at_location(ONLY IN_THREAD,
				impl_t::lock &lock,
				const richtextcursorlocation &a,
				const richtextcursorlocation &b);

	//! Combine remove with insert.

	void replace_at_location(ONLY IN_THREAD,
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

	ref<richtext_implObj> debug_get_impl(ONLY IN_THREAD);
};


//! IN_THREAD has full rights on the underlying object.
class richtextObj::impl_t::lock : public internal_richtext_impl_t::lock {


public:
	lock(impl_t &);
};

LIBCXXW_NAMESPACE_END;
#endif
