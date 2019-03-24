/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peephole_appearanceobj_h
#define x_w_peephole_appearanceobj_h

#include <x/w/peephole_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/dim_arg.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref peephole_appearance "INSERT_LIBX_NAMESPACE::w::peephole_appearance".

struct peephole_appearance_properties {

	//! Constructor
	peephole_appearance_properties();

	//! Destructor
	~peephole_appearance_properties();

	//! Optional border
	std::optional<border_arg> peephole_border;

	//! Extra padding for the element in the peephole.

	dim_arg left_padding=0;

	//! Extra padding for the element in the peephole.

	dim_arg right_padding=0;

	//! Extra padding for the element in the peephole.

	dim_arg top_padding=0;

	//! Extra padding for the element in the peephole.

	dim_arg bottom_padding=0;

	//! Horizontal scrollbar appearance
	const_scrollbar_appearance horizontal_scrollbar;

	//! Vertical scrollbar appearance
	const_scrollbar_appearance vertical_scrollbar;
};

//! Implementation object.

//! \see peephole_appearance

class peephole_appearanceObj: public peephole_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	peephole_appearanceObj();

	//! Destructor
	~peephole_appearanceObj();

	//! Copy constructor
	peephole_appearanceObj(const peephole_appearanceObj &);

	//! Create a copy of this object.
	peephole_appearance clone() const;
};

LIBCXXW_NAMESPACE_END

#endif
