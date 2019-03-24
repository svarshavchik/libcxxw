/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_print_dialog_appearance_h
#define x_w_print_dialog_appearance_h

#include <x/w/print_dialog_appearancefwd.H>
#include <x/w/print_dialog_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref print_dialog_appearance "print_dialog appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::print_dialog_appearance::base.

class print_dialog_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const const_print_dialog_appearance &theme();

};

LIBCXXW_NAMESPACE_END

#endif
