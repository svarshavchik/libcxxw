/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_attachedto_info.H"
#include "connection_thread.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

popup_attachedto_handlerObj
::popup_attachedto_handlerObj(const popup_attachedto_handler_args &args)
	: popupObj::handlerObj{args}
{
}

popup_attachedto_handlerObj::~popup_attachedto_handlerObj()=default;

LIBCXXW_NAMESPACE_END
