/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/button_event.H"

LIBCXXW_NAMESPACE_START

button_event::button_event(uint16_t SETofKEYBUTMASK,
			   const keysyms &k,
			   int button,
			   bool press)
	: input_mask{SETofKEYBUTMASK, k},
	  button{button},
	  press{press}
{
}

button_event::~button_event()=default;

LIBCXXW_NAMESPACE_END
