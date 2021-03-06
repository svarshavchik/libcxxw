/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/button_event.H"

LIBCXXW_NAMESPACE_START

button_event::button_event(uint16_t SETofKEYBUTMASK,
			   const keysyms &k,
			   int button,
			   bool press,
			   unsigned click_count,
			   button_event_redirect_info &redirect_info)
	: input_mask{SETofKEYBUTMASK, k},
	  button{button},
	  press{press},
	  click_count{click_count},
	  redirect_info{redirect_info}
{
}

button_event::~button_event()=default;

LIBCXXW_NAMESPACE_END
