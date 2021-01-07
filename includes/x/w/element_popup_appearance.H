/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_popup_appearance_h
#define x_w_element_popup_appearance_h

#include <x/w/element_popup_appearancefwd.H>
#include <x/w/element_popup_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref element_popup_appearance "element_popup appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::element_popup_appearance::base.

class element_popup_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_element_popup_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
