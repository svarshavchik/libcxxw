/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_delayed_input_focusfwd_h
#define x_w_impl_delayed_input_focusfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN delayed_input_focusObj;

/*! Widget that requested input focus that has been delayed.

This gets placed into scheduled_input_focus weak pointer.
process_focus_updates() checks if the widget can now receive input focus.

*/

typedef ref<delayed_input_focusObj> delayed_input_focus;

//! A constant \ref delayed_input_focus "WHATITIS".

//! \see delayed_input_focus

typedef const_ref<delayed_input_focusObj> const_delayed_input_focus;

//! A nullable pointer reference to a \ref delayed_input_focus "WHATITIS".

//! \see delayed_input_focus

typedef ptr<delayed_input_focusObj> delayed_input_focusptr;

//! A nullable pointer reference to a const \ref delayed_input_focus "WHATITIS".

//! \see delayed_input_focus

typedef const_ptr<delayed_input_focusObj> const_delayed_input_focusptr;

LIBCXXW_NAMESPACE_END

#endif
