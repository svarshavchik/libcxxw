/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_table_appearance_h
#define x_w_table_appearance_h

#include <x/w/table_appearancefwd.H>
#include <x/w/table_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref table_appearance "table appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::table_appearance::base.

class table_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_table_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
