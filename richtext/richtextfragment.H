/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextfragment_H
#define richtextfragment_H

#include <vector>
#include <unordered_map>

#include <x/ref.H>
#include <x/obj.H>
#include <x/logger.H>
#include <x/functional.H>
#include <x/vector.H>
#include "x/w/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/impl/richtext/richtextfragmentfwd.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/richtext/richtextcursorlocationfwd.H"
#include "richtext/richtexthorizinfo.H"
#include "x/w/impl/richtext/richtext_insertfwd.H"
#include "richtext/richtext_linebreak_infofwd.H"
#include "x/w/picturefwd.H"
#include "x/w/pictureobj.H"
#include "x/w/rgb_hash.H"
#include "x/w/screenfwd.H"
#include "x/w/metrics/axis.H"
#include "x/w/alignment.H"
#include "assert_or_throw.H"

#include <list>

LIBCXXW_NAMESPACE_START

//! Compute size of the underline.

dim_t underline_size(dim_squared_t font_height) LIBCXX_HIDDEN;

//! Adjust for underline.

//! Even though this text range may not be underlined
//! always take into account the underline's
//! requirements. See underline().

dim_t adjust_descender_for_underline(dim_t ascender,
				     dim_t descender) LIBCXX_HIDDEN;

//! A text fragment, with some metadata (fonts)

//! A list of these form a text paragraph. These objects are managed by
//! a \ref textparagraph object.

class LIBCXX_HIDDEN richtextfragmentObj : virtual public obj {

	//! Logger
	LOG_CLASS_SCOPE;

public:

	//! Internal exception.

	static void get_fragment_out_of_bounds();

	//! A vector of these fragments.

	//! This vector is actually stored as part of the paragraph object.

	class fragments_t : private std::vector<richtextfragment> {

		typedef std::vector<richtextfragment> v_t;
	public:

		using v_t::iterator;
		using v_t::const_iterator;

		using v_t::size;
		using v_t::empty;

		// Return an iterator for the nth fragment.
		inline auto get_iter(size_t n) const
		{
			if (n >= size())
				get_fragment_out_of_bounds();

			return begin()+n;
		}

		// Return an iterator for the nth fragment.
		inline auto get_iter(size_t n)
		{
			if (n >= size())
				get_fragment_out_of_bounds();

			return begin()+n;
		}

		//! The paragraph with these fragments is about to be destroyed.

		//! Invoked from the paragraph object's constructor, sets
		//! every fragment's my_paragraph to a nullptr.

		void paragraph_destroyed();

		//! Find the fragment for character #pos

		//! Also update pos to reflect the character offset within
		//! the found fragment.

		richtextfragment find_fragment_for_pos(size_t &pos);

		//! Find the fragment for the given y coordinate.

		//! Returns null ptr if the y coordinate is below the last
		//! fragment.
		richtextfragmentptr find_fragment_for_y_position(size_t);

		//! Find the fragment for the given position.

		//! Used by richtextparagraphObj::find_fragment_for_pos()
		richtextfragment find_fragment_for_pos(size_t &) const;

		friend class fragment_list;
		friend class const_fragment_list;

		//! Helper for invoking the given lambda for each fragment.

		template<typename lambda_t>
		void for_fragments(lambda_t &&lambda)
		{
			do_for_fragments
				(make_function<void (const richtextfragment &)>
				 (std::forward<lambda_t>(lambda)));
		}

		//! Invoke a callback for each fragment in this paragraph.
		void do_for_fragments(const function<void(const richtextfragment
							  &)> &lambda)
		{
			for (const auto &f:*this)
				lambda(f);
		}
	};

	//! The paragraph this fragment is in.

	//! Paragraphs hold refs on their fragments. The entire implObj
	//! object is protected by a mutex lock. When the paragraph gets
	//! destroyed, all of its fragments get destroyed. This native pointer,
	//! therefore, is always valid (so far).
	//!
	//! my_paragraph and my_fragment_number are initialized and
	//! maintained by fragment_list's methods.

	richtextparagraphObj *my_paragraph=nullptr;


#define USING_MY_PARAGRAPH() \
	assert_or_throw(my_paragraph && my_paragraph->my_richtext &&			\
			my_paragraph->fragments.size() > my_fragment_number,\
			"Internal error: paragraph or text linkage sanity check failed.")

#define RESOLVE_FONTS()				\
	(string.resolve_fonts())


	//! \see my_paragraph
	size_t my_fragment_number=0;

	//! First character in this fragment is character #first_char_n in this paragraph
	size_t first_char_n=0;

	//! Fragment's y-position, respective to its paragraph's y_position.
	size_t y_pos=0;

	//! A list of cursor locations in this fragment.

	typedef std::list<richtextcursorlocation> locations_t;

	//! Cursor locations in this fragment
	locations_t locations;

	//! The text fragment
	richtextstring string;

	//! Line breaking status before the corresponding character in text.

	//! This is UNICODE_LB_NONE, UNICODE_LB_ALLOWED, or
	//! UNICODE_LB_MANDATORY. The first richtextfragment in a textparagraph
	//! will be 0 for the first textparagraph, and UNICODE_LB_MANDATORY for
	//! all subsequent text paragraphs. Any second, or subsequent,
	//! richtextfragment in a textparagraph is expected to show
	//! UNICODE_LB_ALLOWED for its first character.

	std::vector<unicode_lb> breaks;

	//! Horizontal dimensions cache.
	richtexthorizinfo_t horiz_info;

	//! Current theme was updated. Fonts have been changed.
	void theme_updated_called_by_fragment_list(ONLY IN_THREAD,
						   const const_defaulttheme
						   &new_theme);

 private:

	//! Make sure that all glyphs are loaded. Calculate widths and kernings.

	//! This invokes richtextstring's compute_width(), after some minor
	//! prep work.

	void load_glyphs_widths_kernings();

	//! Internal implementation.

	void load_glyphs_widths_kernings(richtextfragmentObj *prev_fragment,
					 richtextfragmentObj *next_fragment);

	//! Compute glyphs, widths, and kernings for a subset of characters.

	//! A shortcut, as a result of a change, we ohly need to update the
	//! glyphs, widths, and kernings for a subset of characters, instead
	//! of the whole thing.
	//!
	//! Invokes richtextstring's compute_width().
	void update_glyphs_widths_kernings(size_t pos,
					   size_t count);

 public:
	//! Default constructor
	richtextfragmentObj();

	//! Internal constructor

	//! This gets invoked from set(), when constructing the initial
	//! single fragment in the paragraph, to kick things off.

	richtextfragmentObj(richtextstring &&string,
			    std::vector<unicode_lb> &&breaks);

	//! After the internal constructor, above, is done, set() calls this.

	//! set() must add the new fragment to its paragraph, before we can
	//! wrap things up and load_glyphs_widths_kernings().

	void finish_setting();

	//! Private constructor

	//! This constructor is used by split() to peel off a portion of
	//! an existing fragment into a new fragment. As such, the peeled off
	//! portion's metadata (line breaks, widths, kernings) can be simply
	//! lifted off the existing fragment, avoiding the need to recompute
	//! it from scratch.

	richtextfragmentObj(const richtextfragmentObj &current_fragment,
			    size_t substr_pos,
			    size_t substr_len);

	//! What kind of split() to do.

	//! The onus is on the caller to pick the right value.
	enum split_t {
	      split_lr, //!< Splitting of normal left-to-right text
	      split_rl, //!< Special logic for splitting right-to-left text
	};
	//! Split this text fragment at character #n.

	//! Returns a new text fragment, containing character #n and
	//! onwards. This text fragment is truncated to character #n.

	void split(//! Fragment's paragraph's fragment list
		   fragment_list &my_fragments,

		   //! Split position
		   size_t pos,

		   //! Split type

		   //! The onus is on the caller to do the right thing.
		   enum split_t type,

		   //! Force the split

		   //! Turns off sanity checking. Used to split nl
		   //! at the end of the paragraph, before splitting the
		   //! last line.
		   bool force
		   );

	//! Take the next fragment in this paragraph and merge it into this one.

	void merge(//! Fragment's paragraph's fragment list
		   fragment_list &my_fragments);

 private:

	//! Merge a richtextdir::lr line with the next line in rl paragraph

	//! Paragraph embedding level is rl, and this is a richtext_dir::lr
	//! fragment.
	//!
	//! If the next line is richtextdir::lr too, we can just use
	//! merge_lr_lr.
	//!
	//! Otherwise if there are any lr text at the end of the next
	//! fragment, they get appended to this line, with the rest prepended.

	void merge_rl_lr(fragment_list &my_fragments,
			 const richtextfragment &other);

	//! Left to right paragraph, merging richtext_dir:rl with next fragment

	//! This fragment is richtext_dir::rl. Check if the next fragment
	//! begins with rl text, if so it needs to be prepended to this
	//! fragment.

	void merge_lr_rl(fragment_list &my_fragments,
			 const richtextfragment &other);

 public:

	//! Left-to-right paragraph, left-to-right text merge.

	//! Basically appends the next fragment to this one.
	//!
	//! Invoked by merge(), to merge left-to-right text. This is also
	//! called from merge_lr_rl, to handle merging of the next fragment
	//! when this fragment is right-to-left text, merge_lr_rl() does some
	//! prep work, then invokes this too.
	void merge_lr_lr(fragment_list &my_fragments,
			 const richtextfragment &other);


	//! Calculate the fragment index from start of rich text.

	//! Each fragment is essentially a line, and this is the line number
	//! of this fragment in the rich text.
	size_t index() const;

	//! Destructor
	~richtextfragmentObj();

	//! Recalculate linebreaks for the line being inserted or removed.

	//! Must be called before recalculate_size()
	//!

	void recalculate_linebreaks();

	//! Delete text.

	void remove(size_t pos,
		    size_t nchars,
		    fragment_list &my_fragments);

	//! Move attached cursor locations to another fragment.

	//! Called when removing entire fragments, to move their attached
	//! cursor locations to the next fragment.

	void move_locations_to_another_fragment(richtextfragmentObj *n);

	//! Insert text.

	//! Returns 0 if the text is empty.
	//!
	//! Returns 1, unless new_text has line breaks, which inserts a new
	//! paragraph. Each inserted paragraph increments the return value
	//! by 1.

	size_t insert(ONLY IN_THREAD,
		      paragraph_list &my_paragraphs,
		      const richtext_insert_base &new_string);

	//! Computed width of this text fragment.

	//! This does not include the first character's kerning value.
	dim_t width=0;

	//! Computed width up to the first breakable character, for lr text.

	//! This includes the kerning value of the first character.
	//!
	//! If this fragment starts with rl text, this is the size of the
	//! last non-breakable rl sequence.
	dim_t initial_width_lr=0;

	//! Trailing lr segment's initial width

	//! If this fragment ends with lr text this is the width of the
	//! initial sequence of lr text, up to the first breakable character.
	//!
	//! If this fragment ends with rl text, this is the width of the rl
	//! after after the last break.

	dim_t initial_width_rl_trailing_lr=0;

	//! Trailing rl segment's width.

	//! If this fragment ends with lr text, this is the entire width of the
	//! rl text. Otherwise it is the same as initial_width_rl_trailing_lr

	dim_t initial_width_rl=0;

	//! Maximum minimum width

	//! Basically, if this fragment is split at every breakable position,
	//! this would be the maximum width of all the split fragments.
	dim_t minimum_width=0;

	//! Computed height of this text fragment above the baseline
	dim_t above_baseline=0;

	//! Computed height of this text fragment below the baseline
	dim_t below_baseline=0;

	//! above+below=height

	dim_t height() const { return dim_t::truncate(above_baseline+below_baseline); }

	//! Compute the real y_position of this fragment
	size_t y_position() const;

	//! Locate a fragment for a given y position.

	//! Using this fragment as a starting point of reference, locate
	//! a fragment for a given y_position_requested.
	//!
	//! If y_position is below this fragment's y_position()+height(),
	//! find the following fragment that covers the y_position.
	//! If y_position is above this fragment's y_position()
	//! find the previous fragment that covers this y_position.
	//! Returns the fragment for the given y_position, and a boolean flag,
	//! false if this fragment covers y_position_requested.

	std::pair<richtextfragmentObj *, bool>
		find_y_position(size_t y_position_requested);

	//! Set by fragment_list in order to recalculate this fragment's size

	bool recalculate_size_needed=false;

	//! Invoked by fragment_list to recalculate this fragment.

	void recalculate_size_called_by_fragment_list();

	struct render_info;

	//! Render fragment

	void render(ONLY IN_THREAD,

		    //! Rendering coordinate information
		    const render_info &info);

	//! Compute starting X position of the rendered fragment.
	coord_t first_xpos(ONLY IN_THREAD) const;

	//! Return the width of the fragment in pixels.
	dim_t x_width(ONLY IN_THREAD);

	//! Compute the "initial width" of this fragment

	//! rewrap_fragment() is considering merging the next fragment into
	//! this one. This determines what the next fragment "brings
	//! to the table". For left-to-right text, this would be the width
	//! of the initial word. For right-to-left text, this would be the
	//! width of the last word.
	//!
	//! NOTE: rewrap_fragment() makes sure that this is not the last
	//! fragment in the paragraph.
	dim_t compute_initial_width_for_bidi() const;

 private:

	//! We may sometimes need to overlay richtextmeta with something

	enum class meta_overlay {
		normal, //!< Nothing
		inverse, //!< Inverse video (text cursor)
	};

	//! A list of temporary metadata overlays for rendering.
	typedef std::map<size_t, meta_overlay> overlay_map_t;

	//! Merge something into an overlay

	static void overlay_merge(overlay_map_t &overlay,
				  size_t start,
				  size_t end,
				  meta_overlay what);

	struct render_range_info;

	//! Apply overlays to range_info, call render_range()
	void render_range_with_overlay(ONLY IN_THREAD,
				       render_range_info &range_info,
				       const overlay_map_t &overlay);

	//! Render a range of text, same metadata for the text.
	void render_range(render_range_info &range_info,
			  const richtextmeta &markup,
			  bool has_background_color,
			  const const_picture &foreground_color,
			  coord_t color_x,
			  coord_t color_y,
			  const const_picture &background_color,
			  coord_t background_x,
			  coord_t background_y);

 public:
	//! Return previous fragment in this paragraph, or nullptr
	richtextfragmentObj *prev_fragment() const;

	//! Return next fragment in this paragraph, or nullptr
	richtextfragmentObj *next_fragment() const;

	//! Rich text editor window: this fragment needs to be redrawn

	//! This is used by richtexteditorwindow. It defaults to true,
	//! richtexteditorwindow resets it to false. This is reset back to
	//! true by: insert(), split(), merge().
	bool redraw_needed=true;

	//! At what row this fragment was redrawn (if redraw_needed is false).
	coord_t redrawn_y_position=0;


	size_t compute_split_pos_lr(dim_t width) const;

	size_t compute_split_pos_rl(dim_t width) const;
};

LIBCXXW_NAMESPACE_END

#endif
