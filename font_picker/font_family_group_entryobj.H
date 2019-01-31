/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_family_group_entryobj_h
#define x_w_font_family_group_entryobj_h

#include "x/w/font_pickerfwd.H"
#include "font_picker/font_family_group_entryfwd.H"
#include "fonts/fontpatternfwd.H"
#include "fonts/fontpatternobj.H"
#include <x/obj.H>
#include <vector>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see font_family_group_entry

class LIBCXX_HIDDEN font_family_group_entryObj : virtual public obj {

public:
	//! Font ID
	font_picker_group_id id;

	//! Initially set to id.family

	//! If we end up with the same family from different foundries, the
	//! foundry name get added to name, in parenthesis. This is used to
	//! present all font families in the list.

	std::string name;

	//! Whether there are other groups with the same family (different foundries)

	bool multiple_foundries=false;

	//! All the matching font patterns for the font_picker_group_id
	std::vector<fontconfig::const_pattern> patterns;

	//! Constructor
	font_family_group_entryObj(const font_picker_group_id &id);

	//! Destructor
	~font_family_group_entryObj();
};

LIBCXXW_NAMESPACE_END

#endif
