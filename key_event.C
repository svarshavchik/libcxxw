/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/key_event.H"

LIBCXXW_NAMESPACE_START

key_event::key_event(uint16_t state, const keysyms &k) : input_mask{state, k}
{
}

key_event::~key_event()=default;

LIBCXXW_NAMESPACE_END
