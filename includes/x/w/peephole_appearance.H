/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peephole_appearance_h
#define x_w_peephole_appearance_h

#include <x/w/peephole_appearancefwd.H>
#include <x/w/peephole_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref peephole_appearance "peephole appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::peephole_appearance::base.

class peephole_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_peephole_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
