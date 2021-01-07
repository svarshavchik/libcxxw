/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_frame_appearance_h
#define x_w_frame_appearance_h

#include <x/w/frame_appearancefwd.H>
#include <x/w/frame_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref frame_appearance "frame appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::frame_appearance::base.

class frame_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_frame_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
