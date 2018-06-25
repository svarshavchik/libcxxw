/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_pickerfwd_h
#define x_w_font_pickerfwd_h

#include <x/w/font.H>
#include <x/w/callback_triggerfwd.H>
#include <x/w/busyfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class font_pickerObj;

//! \ref font_picker "Font picker" grouping.

//! The font picker's font family combo-box shows a list of
//! available font families. Each foundry that provides the same font
//! family is listed individually.
//!
//! The fonts are identified by their families, and foundries.

struct font_picker_group_id {

	//! Font family
	std::string family;

	//! Font foundry
	std::string foundry;

	//! Default constructor

	font_picker_group_id()=default;

	//! Construct from a font object.
	font_picker_group_id(const font &f)
		: family{f.family},
		  foundry{f.foundry}
	{
	}

	//! Construct a font object

	void update(font &f) const
	{
		f.family=family;
		f.foundry=foundry;
	}

	//! Comparison operator.
	bool operator<(const font_picker_group_id &) const;

	//! Comparison operator.
	inline bool operator<=(const font_picker_group_id &o) const
	{
		return !operator>(o);
	}

	//! Comparison operator.
	inline bool operator>(const font_picker_group_id &o) const
	{
		return o < *this;
	}

	//! Comparison operator.

	inline bool operator>=(const font_picker_group_id &o) const
	{
		return !operator<(o);
	}

	//! Comparison operator.
	bool operator==(const font_picker_group_id &) const;

	//! Comparison operator.
	inline bool operator!=(const font_picker_group_id &o) const
	{
		return !operator==(o);
	}

	//! Serialization function.

	//! Implement base LibCXX library's serialization API. The serialization
	//! API is designed for reference-counted objects. This isn't one,
	//! but provide a compatible serialize().

	template<typename iter_type>
	void serialize(iter_type &iter)
	{
		iter(family);
		iter(foundry);
	}
};

/*! Font picker display element.

\code
INSERT_LIBX_NAMESPACE::w::font_picker fp=factory->create_font_picker();
\endcode

The font picker display element provides the means of specifying most
values of a \ref font "INSERT_LIBX_NAMESPACE::w::font" specification.

The display element itself shows only the name of the font, with a button
next to it. Clicking the popup opens an interactive popup for modifying
the font's family, size, and style (bold, italicized, etc...)

\par The "Ok" and "Cancel" buttons

The "Cancel" button closes the font picker popup and restores the original
font if it was changed. "Ok" closes the popup and saves the current
font if it was changed. Closing the font picker popup in any other way
(the "Esc" key, or clicking outside of it with the pointer) is equivalent to
a "Cancel".

\code
INSERT_LIBX_NAMESPACE::w::font f=fp->current_font();

fp->current_font(f);

fp->current_font(IN_THREAD, f);
\endcode

current_font() returns the font picker's most recently saved font, or
sets the font picker's current and saved font. current_font() checks the
new font for validity. Unknown font families are quietly ignored. The
underlying freetype library handles font with fractional font sizes, but
the popup accepts only whole point sizes, so the passed in point size
gets rounded off. The font popup ignores \ref font "INSERT_LIBX_NAMESPACE::w::font"
style, scaled size, pixel size, and spacing values.

\code
std::vector<INSERT_LIBX_NAMESPACE::w::font_picker_group_id> mru=fp->most_recently_used();

fp->most_recently_used(mru);

fp->most_recently_used(IN_THREAD, mru);
\endcode

The font picker popup lists all available system fonts in alphabetical
order, except for the "most recently used" fonts, which appear first.
most_recently_used() returns the most recently used font list, or sets the
new list.

\ref font_picker_group_id "INSERT_LIBX_NAMESPACE::w::font_picker_group_id"
is an identifier for a system font; just its name and maybe its foundry.

\code
fp->on_font_update([]
                   (ONLY IN_THREAD,
                    const INSERT_LIBX_NAMESPACE::w::font &new_font,
                    const INSERT_LIBX_NAMESPACE::w::font_picker_group_id *new_font_group,

                    const INSERT_LIBX_NAMESPACE::w::font_picker &myself,
                    const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger,
                    const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
	           {
                   });
\endcode

on_font_update() installs a callback that gets executed by the font picker
popup's "Ok" button. A newly-installed callback gets initially executed
by the library's internal connection thread passing a
\c INSERT_LIBX_NAMESPACE::w::initial{} value for
the \ref callback_trigger_t "trigger parameter". This specifies the picker's
current font at the time the callback gets installed.
The callback then gets invoked by the "Ok" button, or by current_font()
setting a new font.

Only one font picker callback is active at any time.
Installing another font picker callback replaces the previous one.

The font picker callback's parameter include:

- the font picker's new saved \ref font "font".

- the identifier for the system font that the new font is based on. A
custom \ref font_picker_config "font picker configuration" makes the selecting
a font optional, in that case the passed in font value is the default system
font and the identifier is a \c nullptr.

- the font picker display element itself. Font picker callbacks often need
to reference their display element; usually to have the font picker's
most recently used list include the newly picked font. This saved the
callbacks the trouble of properly weakly-capturing their display element and
recovering the weak reference before using it. That detail is already taken
care of.

*/

typedef ref<font_pickerObj> font_picker;

//! A constant \ref font_picker "font selection picker".

//! \see font_picker

typedef const_ref<font_pickerObj> const_font_picker;

//! A nullable pointer reference to a \ref font_picker "font selection picker".

//! \see font_picker

typedef ptr<font_pickerObj> font_pickerptr;

//! A nullable pointer reference to a const \ref font_picker "font selection pickery".

//! \see font_picker

typedef const_ptr<font_pickerObj> const_font_pickerptr;

//! Parameters for the \ref font_picker "font picker"'s callback.

//! Signature for the \ref font_picker "font picker"'s callback.

typedef void font_picker_callback_t(THREAD_CALLBACK,
				    const font &new_font,
				    const font_picker_group_id *new_font_group,
				    const font_picker &myself,
				    const callback_trigger_t &trigger,
				    const busy &mcguffin);

LIBCXXW_NAMESPACE_END

#endif
