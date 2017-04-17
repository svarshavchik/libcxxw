/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximrequest_H
#define ximrequest_H

#include "xim/ximrequestfwd.H"
#include "xim/ximserverfwd.H"
#include "xim/ximclientfwd.H"
#include "connection_threadfwd.H"
#include <x/logger.H>
#include <x/functional.H>

#include <vector>
#include <cstdint>

LIBCXXW_NAMESPACE_START

//! XIM synchronoous request object.

class LIBCXX_HIDDEN ximrequestObj : virtual public obj {

 protected:
	LOG_CLASS_SCOPE;

 public:
	//! Identifies the request
	const char * const request_type;

	//! Constructor
	ximrequestObj(const char *request_typeArg);

	//! Destructor
	~ximrequestObj();

	//! Log an error.

	void error(const std::string &error_code);

	//! Received the wrong request type.

	//! The connection is considered to be broken.
	void wrong_request_type(const char *received_request_type);

	//! Create a reply.
	virtual void xim_create_ic_reply(IN_THREAD_ONLY,
					 const ximserver &server,
					 xim_ic_t input_context_id);
	//! Create a reply.
	virtual void xim_destroy_ic_reply(IN_THREAD_ONLY,
					  const ximserver &server,
					  xim_ic_t input_context_id);
	//! Create a reply.
	virtual void xim_sync_reply(IN_THREAD_ONLY,
				    const ximserver &server,
				    uint16_t input_context_id);
};

LIBCXXW_NAMESPACE_END
#endif
