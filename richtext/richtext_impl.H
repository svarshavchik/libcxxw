/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_impl_H
#define richtext_impl_H

#include "x/w/impl/richtext/richtextparagraphfwd.H"
#include "x/w/impl/richtext/richtextfragmentfwd.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/impl/richtext/richtextstringfwd.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/metrics/axis.H"
#include "x/w/screenfwd.H"
#include "x/w/input_field_lock.H"
#include <x/functional.H>
#include <x/vectorfwd.H>

#include <unordered_map>
#include <algorithm>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN richtext_insert_base;

//! A list of text paragraphs that ends with a mandatory line break.

//! This is not a thread-safe object. Although some methods are marked
//! ONLY IN_THREAD, this implementation object is protected by a mutex, in
//! the public object. The ONLY IN_THREAD is in addition to the
//! general thread-safe requirement.

class richtext_implObj : virtual public obj {

	//! Targeted word wrap width.

	dim_t word_wrap_width;

public:

	static void rich_text_paragraph_out_of_bounds();

	class paragraphs_t : private std::vector<richtextparagraph> {

		typedef std::vector<richtextparagraph> v_t;
	public:

		friend class paragraph_list;

		using v_t::iterator;
		using v_t::const_iterator;
		using v_t::size;
		using v_t::empty;
		using v_t::clear;

	        inline iterator get_paragraph(size_t n)
		{
			if (n >= size())
				rich_text_paragraph_out_of_bounds();
			return begin()+n;
		}

		inline const_iterator get_paragraph(size_t n) const
		{
			if (n >= size())
				rich_text_paragraph_out_of_bounds();
			return begin()+n;
		}

		// Iterate a functor callback, that receives all paragraphs
		// starting with paragraph #first_paragraph.
		//
		// callback returns true to continue iteration, false to
		// abort.

		template<typename functor>
		inline void for_paragraphs(size_t first_paragraph,
					   functor &&f) const
		{
			size_t s=size();

			if (s < first_paragraph)
				s=first_paragraph;

			for_paragraphs(first_paragraph, s-first_paragraph,
				       std::forward<functor>(f));
		}

		// Iterate a functor callback, that receives #num_paragraphs
		// paragraphs, starting with paragraph #first_paragraph.
		//
		// callback returns true to continue iteration, false to
		// abort.

		template<typename functor>
		inline void for_paragraphs(size_t first_paragraph,
					   size_t num_paragraphs,
					   functor &&f) const
		{
			do_for_paragraphs(make_function
					  <bool (const richtextparagraph &)>
					  (std::forward<functor>(f)),
					  first_paragraph,
					  num_paragraphs);
		}

		inline void do_for_paragraphs(const function<bool
					      (const richtextparagraph &)>
					      &callback,
					      size_t first_paragraph,
					      size_t num_paragraphs) const
		{
			if (first_paragraph > size() ||
			    size()-first_paragraph < num_paragraphs)
				rich_text_paragraph_out_of_bounds();

			auto b=begin()+first_paragraph;

			while (num_paragraphs)
			{
				if (!callback(*b))
					return;
				++b;
				--num_paragraphs;
			}
		}

		richtextparagraph find_paragraph_for_pos(size_t);
		ptr<richtextfragmentObj> find_fragment_for_y_position(size_t);
	};

	//! All paragraphs

	paragraphs_t paragraphs;

	//! How many characters there are in all paragraphs.

	size_t num_chars=0;

	//! Replacement character to show instead of unprintable characters.

	//! \ref editor "editor" overwrites this with a space, so that a
	//! cursor on top of a newline character is visible.
	char32_t unprintable_char=0;

	//! Alignment of text, for rendering paragraphs

	//! We store it here, but richtextObj doesn't use it. The rendering
	//! code picks it up, and forwards it to richtextparagraphObj::render().
	halign alignment;

	//! Paragraph bi-direction embedding level.

	unicode_bidi_level_t paragraph_embedding_level;

	//! Constructor

	//! The richtextstring gets passed in by reference, and gets converted
	//! to rendering order, but not otherwise moved from.
	richtext_implObj(//! Initial contents
			 richtextstring &&str,

			 //! Alignment
			 halign alignmentArg,

			 //! Paragraph bi-direction embedding level.
			 unicode_bidi_level_t paragraph_embedding_level);

	//! Replace existing contents by something else.

	//! The richtextstring gets passed in by reference, and gets converted
	//! to rendering order, but not otherwise moved from.
	void set(ONLY IN_THREAD,
		 //! Text string
		 richtextstring &&string);

 private:
	//! Invoked from set(), after all existing locations are copied.
	void do_set(//! Text string
		    richtextstring &&string);

 public:

	//! Finish initialization of new text.

	//! If initialized is not set, calculate the new text size, and load
	//! all glyphs.
	//!
	//! Invoked from set(), after initialized is set to false.
	//!
	//! Also invoked from the parent rich text object, after acquiring
	//! the lock ONLY IN_THREAD, to finish the construction of this object.

	void finish_initialization();

	//! Destructor
	~richtext_implObj();

	//! Used by the editor to stretch width() to the width of the input window

	dim_t minimum_width_override=0;

	//! Minimum width of this text
	dim_t minimum_width=0;

	//! Computed width of this text
	dim_t real_width=0;

	//! Computed maximum width of this text if each paragraph was one line.
	dim_t real_maximum_width=0;

	//! Optional logical text width. Used in first_xpos().

	//! This is used in list_celltext, for table list labels, to override
	//! the label's logical width.
	std::optional<dim_t> text_width;

	//! Computed height of tallest fragment

	dim_t height_of_tallest_fragment=0;

	//! Actual width

	dim_t width() const
	{
		// Our opening bid is the computed minimum width.
		// Or the minimum width override.
		//
		// If the real width is larger, that is of course our width.
		//
		// But if word_wrap_width is non-0, then this can be slightly
		// larger than the current real width, but if we are being
		// rendered with a center alignment, we should use
		// word_wrap_width as our official width.

		return std::max({minimum_width_override,
				 minimum_width,
				 word_wrap_width,
				 real_width});
	}

	//! Return real_maximum_width or minimum_width, if not wrapped.
	dim_t maximum_width_if_not_wrapped() const
	{
		return std::max({minimum_width_override,
				 minimum_width,
				 real_maximum_width});
	}

	//! Computed height of this text  above the baseline of the last text paragraph
	size_t above_baseline=0;

	//! Computed height of this text  below the baseline of the last text paragraph
	dim_t below_baseline=0;

	//! Shortcut
	size_t height() const { return above_baseline+dim_t::value_type(below_baseline); }

	//! Rewrap this rich text to requested width, in pixels.

	//! Returns true if the rich text was reformatted.
	bool rewrap(dim_t width);

	// Text was removed and inserted here. Rewrap only from here.

	void rewrap_at_fragment(dim_t width,
				richtextfragmentObj *fragment,
				fragment_list &fragment_list_arg);

	//! Unwrap all text

	//! Returns true if the rich text was reformatted.
	bool unwrap();

	/////////////////////////////////////////////////////////////////////

	//! Insert content at the given position.

	//! Invoked from the public object's insert_at_location(), after
	//! locking everything down tightly, and supplying us with the
	//! word_wrap_width.

	void insert_at_location(ONLY IN_THREAD,
				const richtext_insert_base &new_text);

	//! Remove content between two cursors.

	//! Invoked from the public object's remove_at_location(), after
	//! locking everything down tightly, and supplying us with the
	//! word_wrap_width.
	void remove_at_location(const richtextcursorlocation &a,
				const richtextcursorlocation &b);

	//! Combine replace and insert at location, together.

	void replace_at_location(ONLY IN_THREAD,
				 const richtext_insert_base &new_text,
				 const richtextcursorlocation &remove_from,
				 const richtextcursorlocation &remove_to);

 private:

	struct remove_info;

	//! Common code shared by remove_at_location() and replace_at_location().

	void remove_at_location(const remove_info &,
				paragraph_list &);

	//! Remove content between two cursors, without rewrapping it.

	//! remove_at_location() calls remove_at_location_no_rewrap(),
	//! then rewraps the removed text.
	//!
	//! replace_at_location() calls this in order to complete
	//! avoid rewrapping, since insert_at_location(), which
	//! replace_at_location() uses, will do this anyway.

	void remove_at_location_no_rewrap(const remove_info &,
					  paragraph_list &);

	//! Common code shared by remove_at_location() and replace_at_location().

	void insert_at_location(ONLY IN_THREAD,
				paragraph_list &my_paragraphs,
				const richtext_insert_base &new_text,
				const function<void ()> &after_insert);

 public:
	//! Locate the paragraph containing character #pos

	//! Updates pos to reflect the position of the character in the
	//! returned paragraph.
	//!
	//! Updates pos if it exceeds the number of characters in the
	//! text. An exception gets thrown if text is empty, no paragraphs.
	size_t find_paragraph_for_pos(size_t &pos);

	//! Locate the fragment containing the given y position.

	//! Returns null if the y position is below the last fragment

	ptr<richtextfragmentObj> find_fragment_for_y_position(size_t y_pos);

	//! A null ptr gets returned if the
	//! Extract text string.

	richtextstring get_as_richtext() const;

	//! Theme was updated.
	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme);

	//! Return the rich text string's metrics.
	std::pair<metrics::axis, metrics::axis>
		get_metrics(dim_t preferred_width);
};

LIBCXXW_NAMESPACE_END

#endif
