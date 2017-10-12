/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_impl_H
#define richtext_impl_H

#include "richtext/richtextparagraphfwd.H"
#include "richtext/richtext.H"
#include "richtext/richtextstring.H"
#include "richtext/richtextmeta.H"
#include "x/w/metrics/axis.H"
#include "x/w/screenfwd.H"
#include <x/functional.H>
#include <x/vectorfwd.H>

#include <unordered_map>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

class paragraph_list;
class richtextfragmentObj;
class richtext_insert_base;

//! A list of text paragraphs that ends with a mandatory line break.

//! This is not a thread-safe object. Although some methods are marked
//! IN_THREAD_ONLY, this implementation object is protected by a mutex, in
//! the public object. The IN_THREAD_ONLY is in addition to the
//! general thread-safe requirement.

class LIBCXX_HIDDEN richtext_implObj : virtual public obj {

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

	//! If non-0, show this instead of every character
	char32_t password_char=0;

	//! Alignment of text, for rendering paragraphs

	//! We store it here, but richtextObj doesn't use it. The rendering
	//! code picks it up, and forwards it to richtextparagraphObj::render().
	halign alignment;

	//! Constructor
	richtext_implObj(//! Initial contents
			 const richtextstring &str,

			 //! Alignment
			 halign alignmentArg);

	//! Replace existing contents by something else.

	void set(IN_THREAD_ONLY,
		 //! Text string
		 const richtextstring &string);

 private:
	//! Invoked from set(), after all existing locations are copied.
	void do_set(//! Text string
		    const richtextstring &string);

 public:

	bool initialized=false;

	//! Finish initialization of new text.

	//! If initialized is not set, calculate the new text size, and load
	//! all glyphs.
	//!
	//! Invoked from set(), after initialized is set to false.
	//!
	//! Also invoked from the parent rich text object, after acquiring
	//! the lock IN_THREAD_ONLY, to finish the construction of this object.

	void finish_initialization(IN_THREAD_ONLY);

	//! Destructor
	~richtext_implObj();

	//! Minimum width of this text
	dim_t minimum_width=0;

	//! Computed width of this text
	dim_t real_width=0;

	//! Computed maximum width of this text if each paragraph was one line.
	dim_t real_maximum_width=0;

	//! Optional logical text width. Used in first_xpos().
	std::optional<dim_t> text_width;

	//! Computed height of tallest fragment

	dim_t height_of_tallest_fragment=0;

	//! Actual width

	dim_t width() const
	{
		return minimum_width > real_width ? minimum_width:real_width;
	}

	//! Return real_maximum_width or minimum_width, if not wrapped.
	dim_t maximum_width_if_not_wrapped() const
	{
		return minimum_width > real_maximum_width
			? minimum_width:real_maximum_width;
	}

	//! Computed height of this text  above the baseline of the last text paragraph
	size_t above_baseline=0;

	//! Computed height of this text  below the baseline of the last text paragraph
	dim_t below_baseline=0;

	//! Shortcut
	size_t height() const { return above_baseline+dim_t::value_type(below_baseline); }

	//! Rewrap this rich text to requested width, in pixels.

	//! Returns true if the rich text was reformatted.
	bool rewrap(IN_THREAD_ONLY,
		    dim_t width);

	// Text was removed and inserted here. Rewrap only from here.

	void rewrap_at_fragment(IN_THREAD_ONLY,
				dim_t width,
				richtextfragmentObj *fragment,
				fragment_list &fragment_list_arg);

	//! Unwrap all text

	//! Returns true if the rich text was reformatted.
	bool unwrap(IN_THREAD_ONLY);

	/////////////////////////////////////////////////////////////////////

	//! Insert content at the given position.

	//! Invoked from the public object's insert_at_location(), after
	//! locking everything down tightly, and supplying us with the
	//! word_wrap_width.

	void insert_at_location(IN_THREAD_ONLY,
				dim_t word_wrap_width,
				const richtext_insert_base &new_text);

	//! Remove content between two cursors.

	//! Invoked from the public object's remove_at_location(), after
	//! locking everything down tightly, and supplying us with the
	//! word_wrap_width.
	void remove_at_location(IN_THREAD_ONLY,
				dim_t word_wrap_width,
				const richtextcursorlocation &a,
				const richtextcursorlocation &b);

	//! Combine replace and insert at location, together.

	void replace_at_location(IN_THREAD_ONLY,
				 dim_t word_wrap_width,
				 const richtext_insert_base &new_text,
				 const richtextcursorlocation &remove_from,
				 const richtextcursorlocation &remove_to);

 private:

	struct remove_info;

	//! Common code shared by remove_at_location() and replace_at_location().

	void remove_at_location(IN_THREAD_ONLY,
				const remove_info &,
				paragraph_list &,
				dim_t);
	//! Common code shared by remove_at_location() and replace_at_location().

	void insert_at_location(IN_THREAD_ONLY,
				paragraph_list &my_paragraphs,
				dim_t word_wrap_width,
				const richtext_insert_base &new_text,
				const function<void ()> &after_insert);

 public:
	// Locate the paragraph containing character #pos

	// Updates pos to reflect the position of the character in the
	// returned paragraph.
	//
	// Updates pos if it exceeds the number of characters in the
	// text. An exception gets thrown if text is empty, no paragraphs.
	size_t find_paragraph_for_pos(size_t &pos);

	// Locate the fragment containing the given y position.

	// Returns null if the y position is below the last fragment

	ptr<richtextfragmentObj> find_fragment_for_y_position(size_t y_pos);

	// A null ptr gets returned if the
	// Extract text string.

	richtextstring get_as_richtext() const;

	// Theme was updated.
	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme);
};

LIBCXXW_NAMESPACE_END

#endif
