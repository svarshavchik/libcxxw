/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_radio_groupfwd_h
#define x_w_radio_groupfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC radio_groupObj;

//! A group of radio buttons.

//! Construct this object, and pass it to a \ref factory "factory"'s
//! create_radio() to link related radio buttons together.

typedef ref<radio_groupObj> radio_group;

//! A nullable pointer reference to an \ref radio_group "radio button group"..

typedef ptr<radio_groupObj> radio_groupptr;

//! A reference to a constant \ref radio_group "radio button group"..

//! \see radio_group

typedef const_ref<radio_groupObj> const_radio_group;

//! A nullable pointer reference to a constant \ref radio_group "radio button group"..

//! \see radio_group

typedef const_ptr<radio_groupObj> const_radio_groupptr;

LIBCXXW_NAMESPACE_END

#endif
