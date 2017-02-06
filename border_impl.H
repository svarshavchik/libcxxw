/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef border_impl_h
#define border_impl_h

#include "border_implfwd.H"
#include "border_implobj.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for \ref border_impl "border implementation objects".

//! Refer to this class as \c LIBCXX_NAMESPACE::w::border_impl_base.

class border_impl_base : public ptrrefBase {

public:
	//! typedef alias

	typedef border_implObj::draw_info draw_info;

	//! Drawing the top/left corner of a cell.

	static constexpr int cornertl() { return 1; }

	//! Drawing the top/right corner of a cell.

	static constexpr int cornertr() { return 2; }

	//! Drawing the bottom-left corner of a cell.

	static constexpr int cornerbl() { return 4; }

	//! Drawing the bottom-right corner of a cell.

	static constexpr int cornerbr() { return 8; }
};

LIBCXXW_NAMESPACE_END

#endif
