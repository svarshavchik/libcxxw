/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_progressbar_appearance_h
#define x_w_progressbar_appearance_h

#include <x/w/progressbar_appearancefwd.H>
#include <x/w/progressbar_appearanceobj.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <type_traits>
#include <utility>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref progressbar_appearance "progressbar appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::progressbar_appearance::base.

class progressbar_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_progressbar_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
