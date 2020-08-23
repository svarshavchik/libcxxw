/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_richtext_richtextmeta_H
#define x_w_impl_richtext_richtextmeta_H

#include <x/w/rgb.H>
#include <x/w/text_hotspotfwd.H>
#include <x/w/impl/richtext/richtextmetafwd.H>
#include <x/w/impl/fonts/current_fontcollection.H>
#include <x/w/impl/background_colorfwd.H>
#include <compare>

LIBCXXW_NAMESPACE_START

//! Metadata for each consecutive range of characters in some text, somewhere.

//! Text color and other text rendering attributes for a
//! \ref richtextstring "rich text string".

class richtextmeta {

public:

	//! Constructor
	richtextmeta(const background_color &textcolor,
		     const current_fontcollection &textfont);

	//! Constructor
	richtextmeta(const background_color &textcolor,
		     const current_fontcollection &textfont,
		     const background_colorptr &bg_color);

	//! Destructor
	~richtextmeta();

	//! Copy constructor
	richtextmeta(const richtextmeta &);

	//! Assignment operator
	richtextmeta &operator=(const richtextmeta &);

	//! Some kind of a link associated with this metadata
	text_hotspotptr link;

	//! Color

	//! Note that in most cases the color's alpha channel value is ignored.
	//! The alpha channel comes from font's glyphs, used to anti-alias
	//! it during rendering.

	background_color textcolor;

private:
	//! The underlying font is private.

	//! Once a richtextmeta is constructed, this gets etched in stone and
	//! cannot be modified, because the font properties get cached in
	//! several places.

	current_fontcollection textfont;
public:

	//! The font used to draw characters.

	const current_fontcollection &getfont() const { return textfont; }

	//! Replace textfont.

	//! We have to return a new object.

	richtextmeta replace_font(const current_fontcollection &font) const;

	//! Background color

	background_colorptr bg_color;

	//! Whether to artificially underline the text
	bool underline=false;

	//! Right to left text

	//! The rich text is stored in the left-to-right rendering order.
	//! This affects the algorithms for inserting, deleting, and wrapping
	//! text.
	bool rl=false;

	//! Comparison operator, two metadata are equivalent
	std::strong_ordering operator<=>(const richtextmeta &) const noexcept;

	//! Comparison operator, two metadata are equivalent
	bool operator==(const richtextmeta &o) const noexcept
	{
		return operator<=>(o) == std::strong_ordering::equal;
	}
};

#if 0
{
	{
#endif
	}
}

#endif
