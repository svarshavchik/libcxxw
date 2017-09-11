/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtexthorizinfo_H
#define richtexthorizinfo_H

#include "x/w/types.H"
#include <vector>

LIBCXXW_NAMESPACE_START

//! Internal cache of characters' widths and kernings.

//! This object is mostly for richtextfragment's internal use. It caches
//! the computed width and kerning of each character, obtained by looking
//! up the font glyph information.
//!
//! It implements the various operations that richtextfragmentObj does, with
//! this data. This is also used by a few other classes; namely
//! richtextcursorlocation needs to poke its nose in here.

class richtexthorizinfo_t {

	//! Width of each character in pixels.

	//! The constructor does not initialize widths either, this is also
	//! initialized by load_glyphs.

	std::vector<dim_t> widths;

	//! Kerning offset of each character in pixels, from the previous char.

	//! The constructor does not initialize kernings either, this is also
	//! initialized by load_glyphs.
	//!
	//! The horizontal position of character #n+1 is the horizontal
	//! position of character #n + widths[n]+kernings[n+1].
	//!
	//! kernings[0] is the kerning of the first character if it were
	//! drawn immediately after the last character of the previous fragment.

	std::vector<int16_t> kernings;

public:
	//! Default constructor
	richtexthorizinfo_t();

	//! Destructor
	~richtexthorizinfo_t();

	//! Copy constructor
	richtexthorizinfo_t(const richtexthorizinfo_t &);

	//! Assignment operator
	richtexthorizinfo_t &operator=(const richtexthorizinfo_t &);

	//! Substring constructor

	//! Construct from a portion of another richtexthorizinfo_t
	richtexthorizinfo_t(const richtexthorizinfo_t &cur,
			    size_t pos,
			    size_t len);

	//! Return width of character #n
	inline const auto &width(size_t n) const { return widths.at(n); }
	//! Return kerning value of character #n
	inline const auto &kerning(size_t n) const { return kernings.at(n); }

	//! Return width of character #n
	inline auto &width(size_t n) { return widths.at(n); }

	//! Return kerning value of character #n
	inline auto &kerning(size_t n) { return kernings.at(n); }

	//! Whether we are empty.
	inline bool empty() const { return widths.empty(); }

	//! What is our size.
	size_t size() const { return widths.size(); }

	//! Comparison operator, used by unit tests.
	bool operator==(const richtexthorizinfo_t &o) const
	{
		return widths == o.widths && kernings == o.kernings;
	}

	//! Comparison operator, used by unit tests.
	bool operator!=(const richtexthorizinfo_t &o) const
	{
		return !operator==(o);
	}

	//! Update horizontal width data.

	//! Invokes a lambda, passing to it the widths and kernings
	//! vectors. updated() gets called afterwards.
	template<typename lambda>
	void update(lambda &&l)
	{
		l(widths, kernings);
		updated();
	}

	//! Insert widths+kernings values.

	//! A call to update() will follow, to initialize them.
	void insert(size_t pos, size_t n_size);

	//! Remove widths+kernings values.
	void erase(size_t pos, size_t n_size);

	//! Concatenate another richtexthorizinfo_t to us.
	void append(const richtexthorizinfo_t &other);

	//! Sanity check after an update.
	void updated();

private:

	//! Whether the offset vector is valid.

	bool offsets_valid=false;

	//! Helper vector for find_x_pos() and x_pos()

	std::vector<std::pair<dim_t, dim_t>> offsets;

	//! Compute the offsets() array.

	void compute_offsets();

	//! Largest x value, calculated by compute_offsets().
	dim_t largest_x;

public:

	//! Find the character that's at horizontal position x_pos

	//! Returns size()-1 if x_pos is to the right of the last character

	size_t find_x_pos(dim_t xpos);

	//! Returns the starting position for character #x.

	dim_t x_pos(size_t n);

	//! Return the sum total of widths and kernings

	//! ... except for kerning 0

	dim_t width();
};

LIBCXXW_NAMESPACE_END

#endif
