/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_border_impl_h
#define x_w_impl_border_impl_h

#include <x/w/impl/border_implfwd.H>
#include <x/w/impl/border_implobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for \ref border_impl "border implementation objects".

//! Refer to this class as \c LIBCXX_NAMESPACE::w::border_impl_base.

class border_impl_base : public ptrref_base {

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

	//! Drawing the top stub of the corner border

	static constexpr int top_stub() { return 1; }

	//! Drawing the bottom stub of the corner border

	static constexpr int bottom_stub() { return 2; }

	//! Drawing the left stub of the corner border

	static constexpr int left_stub() { return 4; }

	//! Drawing the right stub of the corner border

	static constexpr int right_stub() { return 8; }
};

LIBCXXW_NAMESPACE_END

#endif
