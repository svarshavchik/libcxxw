/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "xim/ximrequest.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::ximrequestObj);

LIBCXXW_NAMESPACE_START

ximrequestObj::ximrequestObj(const char *request_typeArg)
	: request_type(request_typeArg)
{
}

ximrequestObj::~ximrequestObj()=default;

void ximrequestObj::error(const std::string &error_code)
{
	LOG_ERROR(request_type << ": received " << error_code
		  << " error from the XIM server");
}

void ximrequestObj::wrong_request_type(const char *received_request_type)
{
	LOG_ERROR(request_type << ": expected " << request_type
		  << " response, received " << received_request_type);
}

void ximrequestObj::xim_create_ic_reply(ONLY IN_THREAD,
					const ximserver &server,
					xim_ic_t input_context_id)
{
	wrong_request_type("xim_create_ic_reply");
}

void ximrequestObj::xim_destroy_ic_reply(ONLY IN_THREAD,
					 const ximserver &server,
					 xim_ic_t input_context_id)
{
	wrong_request_type("xim_destroy_ic_reply");
}

void ximrequestObj::xim_sync_reply(ONLY IN_THREAD,
				   const ximserver &server,
				   xim_ic_t input_context_id)
{
	wrong_request_type("xim_sync_reply");
}
#if 0
void ximrequestObj::xim_get_ic_values_reply(ONLY IN_THREAD,
					    const ximserver &server,
					    uint16_t input_context_id,
					    const std::vector<ximattrvalue>
					    &ic_attributes)
{
	wrong_request_type("xim_get_ic_values_reply");
}
#endif

LIBCXXW_NAMESPACE_END
