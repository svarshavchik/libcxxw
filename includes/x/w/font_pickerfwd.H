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

	template<typename iter_type>
	void serialize(iter_type &iter)
	{
		iter(family);
		iter(foundry);
	}
};

/*! Font selection picker


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
