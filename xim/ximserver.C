#include "libcxxw_config.h"
#include "xim/ximclient.H"
#include "xim/ximserver.H"
#include "xim/ximrequest.H"
#include "xim/ximencoding.H"
#include "catch_exceptions.H"
#include "messages.H"
#include <x/locale.H>
#include <x/chrcasecmp.H>
#include <x/sentry.H>
#include <algorithm>
#include <X11/Xlib.h>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::ximserverObj);

LIBCXXW_NAMESPACE_START

ximserverObj::ximserverObj()
	: encoding_thread_only(ximencoding::create("ISO-8859-1"))
	  // Default encoding
{
}

ximserverObj::~ximserverObj()=default;

void ximserverObj::stop(IN_THREAD_ONLY)
{
	if (!stop_flag)
	{
		LOG_DEBUG("Stop, flushing request_queue");
		stop_flag=true;
		request_queue.clear();
		input_contexts.clear();
	}
}

void ximserverObj
::add_client_request(IN_THREAD_ONLY,
		     const ximrequest &req,
		     const ximclient &client,
		     const std::function<bool (IN_THREAD_ONLY,
					       const ximserver &,
					       const ximclient &)>
		     &callback)
{
	add_request(IN_THREAD, req,
		    [=]
		    (IN_THREAD_ONLY, const ximserver &server)
		    {
			    if (client->input_context_id(IN_THREAD) == 0)
			    {
				    LOG_DEBUG(req->request_type <<
					      ": ignoring, client context has been destroyed.");
				    return false;
			    }

			    return callback(IN_THREAD, server, client);
		    });
}


// Queue up a synchronous request.

void ximserverObj::add_request(IN_THREAD_ONLY,
			       const ximrequest &req,
			       const send_sync_request_callback_t &action)
{
	LOG_DEBUG("Adding request: " << req->request_type);

	if (stop_flag)
	{
		LOG_DEBUG("Discarding new request after disconnection");
		return; // The factory is closed.
	}

	bool was_empty=request_queue.empty();

	request_queue.push_back({action, req});

	// If no requests are outstanding, send the next one. Until we are
	// fully connected, don't send anything. After we announce that we
	// are xim_fully_connected(), we will
	if (was_empty && fully_connected)
		send_next_request(IN_THREAD);
}

void ximserverObj::send_next_request(IN_THREAD_ONLY)
{
	auto me=ximserver(this);

	while (!request_queue.empty())
	{
		LOG_DEBUG("Sending: "
			  << request_queue.front().second->request_type);

		try
		{
			if (request_queue.front().first(IN_THREAD, me))
			{
				// Must immediately return. If this is a
				// shutdown(), the request_queue is now
				// completely empty!
				LOG_DEBUG("Request sent.");
				return; // Request sent, await reply.
			}
		} CATCH_EXCEPTIONS;

		LOG_DEBUG("Request discarded/unused.");
		request_queue.pop_front();
	}
}

// Received a reply to a synchronous request. This should be the request
// at the top of our request queue. Remove it, and invoke the callback method
// with it, then send the next synchronous request to the X input method
// server, if there is one.

void ximserverObj::do_sync_reply_received(IN_THREAD_ONLY, const function<void
					  (const ximrequest &)> &callback)
{
	if (request_queue.empty())
	{
		LOG_ERROR("Unexpected message from the XIM server.");
		return;
	}

	auto req=request_queue.front().second;

	request_queue.pop_front();
	try {
		callback(req);
	} CATCH_EXCEPTIONS;
	send_next_request(IN_THREAD);
}

// XIM protocol parsing.
//
// The _received() function get a pointer to the remaining data received and
// its size, and extract the next value from the data.

typedef uint32_t CARD32;
typedef int16_t INT16;
typedef uint16_t CARD16;
typedef uint8_t CARD8;

static inline bool CARD32_received(IN_THREAD_ONLY,
				   const uint8_t *&data, size_t &data_size,
				   CARD32 &v)
{
	if (data_size < 4)
		return false;
	v=(((uint32_t)data[0]) << 24) | (((uint32_t)data[1]) << 16)
		| (((uint32_t)data[2]) << 8) | data[3];

	data += 4;
	data_size -= 4;

	return true;
}

static inline bool INT16_received(IN_THREAD_ONLY,
				  const uint8_t *&data, size_t &data_size,
				  INT16 &v)
{
	if (data_size < 2)
		return false;
	v=(((uint16_t)data[0]) << 8) | data[1];

	data += 2;
	data_size -= 2;

	return true;
}

static inline bool CARD16_received(IN_THREAD_ONLY,
				  const uint8_t *&data, size_t &data_size,
				   CARD16 &v)
{
	if (data_size < 2)
		return false;
	v=(((uint16_t)data[0]) << 8) | data[1];

	data += 2;
	data_size -= 2;

	return true;
}

static inline bool CARD8_received(IN_THREAD_ONLY,
				  const uint8_t *&data, size_t &data_size,
				  CARD8 &v)
{
	if (!data_size)
		return false;
	v=*data++;
	--data_size;

	return true;
}

// Receive a forwarded X event as a 32 byte blob, we'll parse it later.

static inline bool eventbuf_t_received(IN_THREAD_ONLY,
				       const uint8_t * &data, size_t &data_size,
				       ximserverObj::eventbuf_t &a)
{
	if (data_size < 32)
		return false;

	for (size_t i=0; i<32; ++i)
	{
		a[i]=*data++;
		--data_size;
	}
	return true;
}

// The generate() functions create an XIM protocol message.

static inline void CARD8_generate(uint8_t * &p, uint8_t n)
{
	*p++ = n;
}

static constexpr size_t CARD8_sizeof(uint8_t n)
{
	return 1;
}

static inline void CARD16_generate(uint8_t * &p, uint16_t n)
{
	*p++ = n >> 8;
	*p++ = n;
}

static constexpr size_t CARD16_sizeof(uint16_t n)
{
	return 2;
}

static inline void INT16_generate(uint8_t * &p, int16_t n)
{
	*p++ = n >> 8;
	*p++ = n;
}

static constexpr size_t INT16_sizeof(int16_t n)
{
	return 2;
}

static inline void CARD32_generate(uint8_t * &p, uint32_t n)
{
	*p++ = n >> 24;
	*p++ = n >> 16;
	*p++ = n >> 8;
	*p++ = n;
}

static constexpr size_t CARD32_sizeof(uint16_t n)
{
	return 4;
}

static constexpr size_t eventbuf_t_sizeof(const ximserverObj::eventbuf_t &a)
{
	return 32;
}

static inline void eventbuf_t_generate(uint8_t * &p,
				       const ximserverObj::eventbuf_t &a)
{
	for (size_t i=0; i<32; ++i)
		*p++=a[i];
}

static void set_preedit_position(const auto &ic_attributes_by_name,
				 const rectangle &pos,
				 std::vector<ximattrvalue> &attrs)
{
	auto preedit_iter=ic_attributes_by_name.find("preeditattributes");
	if (preedit_iter == ic_attributes_by_name.end())
		return; // Shouldn't happen.

	auto iter=ic_attributes_by_name.find("spotlocation");
	if (iter == ic_attributes_by_name.end())
		return;

#if 0
	auto iter2=ic_attributes_by_name.find("area");
	if (iter2 == ic_attributes_by_name.end())
		return;
#endif

	auto x=pos.x;
	auto y=pos.y+pos.height;

	ximattrvalue v;

	v.id=preedit_iter->second.id;
	v.value.resize(8);

	auto p=&v.value[0];

	CARD16_generate(p, iter->second.id);
	CARD16_generate(p, 4);
	CARD16_generate(p, coord_t::value_type(x));
	CARD16_generate(p, coord_squared_t::value_type(y));

	attrs.push_back(v);

#if 0
	ximattrvalue v2;

	v2.id=iter2->second.id;
	v2.value.resize(8);

	p=&v2.value[0];

	INT16_generate(p, coord_t::value_type(pos.x));
	INT16_generate(p, coord_t::value_type(pos.y));
	CARD16_generate(p, dim_t::value_type(pos.width));
	CARD16_generate(p, dim_t::value_type(pos.height));
	attrs.push_back(v2);
#endif
}


void ximserverObj::badmessage(IN_THREAD_ONLY, const char *message)
{
	xim_disconnected(IN_THREAD);

	throw EXCEPTION(gettextmsg(_("Received truncated %1% message"),
				   message));
}

#include "xim/ximclient.inc.C"

ximattrvalue::ximattrvalue(uint16_t idArg,
			   uint32_t valueArg) : id(idArg)
{
	value.resize(4);
	auto p=&value[0];
	CARD32_generate(p, valueArg);
}

void ximserverObj::received(IN_THREAD_ONLY, const uint8_t *data, size_t p)
{
	if (p < 4)
		badmessage(IN_THREAD, "(unknown)");

	size_t data_size=(((uint16_t)data[2]) << 8) | data[3];

	data_size *= 4;

	if (data_size > p+4)
	{
		std::ostringstream o;

		o << "#" << data[0];

		badmessage(IN_THREAD, o.str().c_str());
	}
	auto message_major=data[0];
	auto message_minor=data[1];
	data += 4;

	switch (message_major) {
#include "xim/ximclient2.inc.C"
	default:
		LOG_INFO("Received unknown message, major=" << message_major
			 << ", minor=" << message_minor);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// Startup
//
// When ximxtransport found_server(), it sends a XIM_XCONNECT message.
// Then XCONNECT response is checked in client_message_event() doing
// a xim_connect_send(). Now, we have the xim_connect reply:

void ximserverObj::received_xim_connect_reply(IN_THREAD_ONLY,
					      uint16_t major,
					      uint16_t minor)
{
	LOG_DEBUG("XIM_CONNECT reply: protocol "  << major << "." << minor);

	if (major != 1 || minor != 0)
	{
		LOG_ERROR("Unsupported X Input Method protocol: "
			  << major << "." << minor);
		xim_disconnected(IN_THREAD);
		return;
	}

	// Next step is to a send a XIM_OPEN message.

	xim_open_send(IN_THREAD, locale::base::environment()->name());
}

// Received a reply to the XIM_OPEN message.

void ximserverObj
::received_xim_open_reply(IN_THREAD_ONLY,
			  xim_im_t input_method_idArg,
			  const std::vector<attr> &im_attributes,
			  const std::vector<attr> &ic_attributes)
{
	LOG_DEBUG( ({
				std::ostringstream o;

				o << "XIM_OPEN reply: input_method_id="
				  << input_method_idArg << ", IM:";

				const char *p="";

				for (const auto &a:im_attributes)
				{
					o << p << "id=" << a.id
					  << ", type=" << a.type
					  << ", name=" << a.name;

					p="; ";
				}
				o << p << "IC: ";
				p="";
				for (const auto &a:ic_attributes)
				{
					o << p << "id=" << a.id
					  << ", type=" << a.type
					  << ", name=" << a.name;
					p="; ";
				}
				o.str();
			}));

	// Save our input_method_id, IM attributes, and IC attributes.

	input_method_id(IN_THREAD)=input_method_idArg;

	for (const auto &a:im_attributes)
	{
		auto s=a.name;

		for (auto &c:s)
			c=chrcasecmp::tolower(c);

		im_attributes_by_name(IN_THREAD).insert({s, a});
	}

	for (const auto &a:ic_attributes)
	{
		auto s=a.name;

		for (auto &c:s)
			c=chrcasecmp::tolower(c);

		ic_attributes_by_name(IN_THREAD).insert({s, a});
	}

	// Now, negotiate the encoding with the server. Some day, someone
	// will wise up and implement a sane Input Method server that's
	// aware of the 21st century, and UTF-8. Until that day happens,
	// we'll still have to support the COMPOUND_TEXT ugliness.

	std::vector<std::string> encodings, ignore;

	// See received_xim_encoding_negotiation_reply
	encodings.push_back("UTF-8");
	encodings.push_back("COMPOUND_TEXT");

	xim_encoding_negotiation_send(IN_THREAD,
				      input_method_id(IN_THREAD), encodings,
				      ignore);
}

void ximserverObj
::received_xim_encoding_negotiation_reply(IN_THREAD_ONLY,
					  xim_im_t input_method_id,
					  uint16_t category,
					  int16_t index)
{
	LOG_DEBUG("XIM_ENCODING_NEGOTIATION reply: category "
		  << category
		  << ", index "
		  << index);

	switch (index) {
	case 0:
		encoding(IN_THREAD)=ximencoding::create("UTF-8");
		break;
	case 1:
		encoding(IN_THREAD)=ximencoding::create("COMPOUND_TEXT");
		break;
	}

	// Send a XIM_GET_IM_VALUES, requesting the values for
	// queryInputStyle IM attribute.

	auto iter=im_attributes_by_name(IN_THREAD).find("queryinputstyle");

	if (iter == im_attributes_by_name(IN_THREAD).end())
	{
		LOG_ERROR("XIM server does not have the queryInputStyle attribute");
		xim_disconnected(IN_THREAD);
		return;
	}

	std::vector<uint16_t> values;

	values.push_back(iter->second.id);

	xim_get_im_values_send(IN_THREAD, input_method_id, values);
}

// Process the reply to XIM_GET_IM_VALUES.

void ximserverObj::received_xim_get_im_values_reply(IN_THREAD_ONLY,
						    xim_im_t input_method_id,
						    const std::vector<attrvalue>
						    &im_attributes)
{
	auto query_input_style_iter=im_attributes_by_name(IN_THREAD)
		.find("queryinputstyle");

	for (const auto &a:im_attributes)
	{
		LOG_DEBUG("XIM_GET_IM_VALUES reply: attribute "
			  << a.id << ": "
			  << ({
					  std::ostringstream o;

					  for (const auto &v:a.value)
					  {
						  o << std::hex << std::setw(2)
						    << std::setfill('0')
						    << (int)v;
					  }
					  o.str();
				  }));

		auto p=&a.value[0];
		auto s=a.value.size();

		if (query_input_style_iter !=
		    im_attributes_by_name(IN_THREAD).end() &&
		    query_input_style_iter->second.id == a.id)
		{
			input_styles(IN_THREAD).clear();

			XIMStyles_received(IN_THREAD,
					   p, s, input_styles(IN_THREAD));

			LOG_DEBUG("Input styles:"
				  << ({
						  std::ostringstream o;

						  for (const auto &s
							       :input_styles
							       (IN_THREAD))
						  {
							  o << " " << std::hex
							    << std::setw(8)
							    << std::setfill('0')
							    << s;
						  }
						  o.str();
					  }));
		}
	}

	// Connection negotiation complete.

	fully_connected=true;
	if (shutdown_requested)
		shutdown(IN_THREAD);

	xim_fully_connected(IN_THREAD);

	// If requests have pent up, we can start sending them.
	send_next_request(IN_THREAD);
}

////////////////////////////////////////////////////////////////////////////
//
// Orderly shutdown.
//
// Orderly shutdown begins with wait_until_disconnected() invoking
// shutdown().
//
// shutdown() schedules a request. If we're shutting down even before we are
// fully_connected, the request stays in the request_queue until we're
// fully_connected, then it gets processed. If we're already fully_connected
// this request will get queued up and processed after all other requests
// get handled in an ordinary fashion.
//
// If there's a connection error, and we're not even connected, the request
// gets quietly tossed on the floor. wait_until_disconnected() waits until
// the ximserver is disconnected; whether it already is disconncted, or
// after everything winds down.
//
// But, if things are on the up an go, the queued request sends an XIM_CLOSE
// message. Immediately after sending XIM_CLOSE, we stop() al further
// requests.

void ximserverObj::shutdown(IN_THREAD_ONLY)
{
	shutdown_requested=true;

	if (!fully_connected)
		return;

	add_request(IN_THREAD,
		    ximrequest::create("shut down"),
		    []
		    (IN_THREAD_ONLY,
		     const ximserver &me)
		    {
			    LOG_DEBUG("Sending XIM_CLOSE");

			    // input_method_id is set only once
			    // fully_connected=true, immediately after that
			    // send_next_request() gets called to start
			    // processing the request_queue.
			    //
			    // Anything that results in stop() getting
			    // called sets stop_flag, which purges the
			    // request_queue(), removing the
			    // request that called shutdown(), here.

			    me->xim_close_send(IN_THREAD,
					       me->input_method_id(IN_THREAD));
			    me->stop(IN_THREAD);

			    // The call to stop() just wiped out
			    // the request_queue. Must return true
			    // so that send_next_request() immediately
			    // returns.
			    return true;
		    });
}

// Received the reply to the XIM_CLOSE message. We send a XIM_DISCONNECT
// message.

void ximserverObj::received_xim_close_reply(IN_THREAD_ONLY,
					    xim_im_t input_method_id)
{
	LOG_DEBUG("XIM_CLOSE reply");

	xim_disconnect_send(IN_THREAD);
}

// Received the reply to the XIM_DISCONNECT message. Everything is fully
// stopped.

void ximserverObj::received_xim_disconnect_reply(IN_THREAD_ONLY)
{
	LOG_DEBUG("XIM_DISCONNECT reply");

	xim_disconnected(IN_THREAD);
}

//////////////////////////////////////////////////////////////////////////
//
// Registering a new client.

class LIBCXX_HIDDEN ximserverObj::create_ic_requestObj : public ximrequestObj {

 public:

	const ximclient client;

	create_ic_requestObj(const ximclient &client)
		: ximrequestObj("XIM_CREATE_IC"), client(client)
	{
	}

	~create_ic_requestObj()=default;

	void xim_create_ic_reply(IN_THREAD_ONLY,
				 const ximserver &server,
				 xim_ic_t input_context_id) override
	{
		LOG_DEBUG("Input context created");

		// This client can now be registered in input_contexts

		server->input_contexts.insert({input_context_id, client});
		client->input_context_id(IN_THREAD)=input_context_id;
		client->forward_event_mask(IN_THREAD)=
			server->default_forward_event_mask;
		client->synchronous_event_mask(IN_THREAD)=
			server->default_synchronous_event_mask;
	}
};

void ximserverObj
::create_client(IN_THREAD_ONLY, const ximclient &client)
{
	LOG_DEBUG("xim_create_ic scheduled");

	auto req=ref<create_ic_requestObj>::create(client);

	add_request(IN_THREAD,
		    req,
		    [client]
		    (IN_THREAD_ONLY,
		     const ximserver &server)
		    {
			    return server->attempt_to_create_client(IN_THREAD,
								    client);
		    });
}

// Too much indentation...

bool ximserverObj::attempt_to_create_client(IN_THREAD_ONLY,
					    const ximclient &client)
{
	std::vector<attrvalue> attributes;

	auto iter=ic_attributes_by_name(IN_THREAD).find("inputstyle");

	if (iter == ic_attributes_by_name(IN_THREAD).end())
	{
		LOG_ERROR("X input method does not have inputStyle IC attribute");
		return false;
	}

	auto style=find_best_input_style
		(IN_THREAD,
		 []
		 (uint32_t s)
		 {
			 /*
			   See XLIB Programming Manual, Rel 5,
			   Page 368.

			   XIMStatusNothing: display status info in the root
			   window.
			   XIMStatusNone: do not display any status

			   XIMPreeditNothing: display preedit in the root
			   window
			   XIMPreeditNone: do not do any pre-editing

			   XIMPreeditPosition: we'll provide the location
			   of the insertion cursor

			 */


			 return ((s & XIMStatusNothing) ? 0x0200:
				 (s & XIMStatusNone) ? 0x0100:0) |
			 ((s & XIMPreeditNothing) ? 0x0002:
			  (s & XIMPreeditNone) ? 0x0001:0) |
			 (s & XIMPreeditPosition ? 0x0400:0);
		 });

	attributes.emplace_back(iter->second.id, style);

	LOG_DEBUG(({

				std::ostringstream o;

				o << "Using input style "
				  << std::hex << std::setw(8)
				  << std::setfill('0')
				  << (int)style;

				o.str();
			}));

	auto window_id=client->client_window->id();

	iter=ic_attributes_by_name(IN_THREAD).find("clientwindow");

	if (iter == ic_attributes_by_name(IN_THREAD).end())
	{
		LOG_ERROR("X input method does not have clientwindow attribute");
		return false;
	}

	attributes.emplace_back(iter->second.id, window_id);
	iter=ic_attributes_by_name(IN_THREAD).find("focuswindow");

	if (iter == ic_attributes_by_name(IN_THREAD).end())
	{
		LOG_ERROR("X input method does not have focuswindow attribute");
		return false;
	}
	attributes.emplace_back(iter->second.id, window_id);

	set_preedit_position(ic_attributes_by_name(IN_THREAD),
			     {},
			     attributes);

	LOG_DEBUG("There are " << attributes.size() << " attributes");

	xim_create_ic_send(IN_THREAD,
			   input_method_id(IN_THREAD),
			   attributes);
	return true;
}

class LIBCXX_HIDDEN ximserverObj::destroy_ic_requestObj : public ximrequestObj {

 public:
	destroy_ic_requestObj() : ximrequestObj("XIM_DESTROY_IC")
	{
	}

	~destroy_ic_requestObj()=default;

	void xim_destroy_ic_reply(IN_THREAD_ONLY,
				  const ximserver &server,
				  xim_ic_t input_context_id) override
	{
		LOG_DEBUG("Destroyed input_context_id " << input_context_id);
		// Noop, nothing needs to be done here, for now.
	}
};

void ximserverObj::destroy_client(IN_THREAD_ONLY, const ximclient &client)
{
	LOG_DEBUG("xim_destroy_ic scheduled");

	auto req=ref<destroy_ic_requestObj>::create();

	add_request(IN_THREAD,
		    req,
		    [client]
		    (IN_THREAD_ONLY,
		     const ximserver &server)
		    {
			    // Deregister this one from input_contexts, first.

			    auto ic=client->input_context_id(IN_THREAD);

			    auto iter=server->input_contexts.find(ic);

			    if (iter == server->input_contexts.end())
			    {
				    LOG_ERROR("Cannot find XIM client");
				    return false;
			    }

			    client->input_context_id(IN_THREAD)=0;
			    server->input_contexts.erase(iter);

			    server->xim_destroy_ic_send(IN_THREAD,
							server->input_method_id
							(IN_THREAD),
							ic);
			    return true;
		    });
}

//////////////////////////////////////////////////////////////////////////

void ximserverObj
::received_xim_set_event_mask(IN_THREAD_ONLY,
			      xim_im_t input_method_id,
			      xim_ic_t input_context_id,
			      uint32_t forward_event_mask,
			      uint32_t synchronous_event_mask)
{
	LOG_DEBUG("XIM_SET_EVENT_MASK reply: context_id="
		  << input_context_id
		  << ", forward_event_mask=" << forward_event_mask
		  << ", synchronous_event_mask="
		  << synchronous_event_mask);

	if (input_context_id == 0)
	{
		// Defaults

		default_forward_event_mask=forward_event_mask;
		default_synchronous_event_mask=synchronous_event_mask;
		return;
	}

	auto c=input_contexts.find(input_context_id);

	if (c == input_contexts.end())
		return;

	c->second->forward_event_mask(IN_THREAD)=forward_event_mask;
	c->second->synchronous_event_mask(IN_THREAD)=synchronous_event_mask;
}

void ximserverObj
::received_xim_register_triggerkeys(IN_THREAD_ONLY,
				    xim_im_t input_method_id,
				    const std::vector<triggerkey> &on_keys,
				    const std::vector<triggerkey> &off_keys)
{
	LOG_DEBUG( ({
				std::ostringstream o;

				o << "XIM_REGISTER_TRIGGERKEYS reply: "
					"ON: ";

				auto dump=[&o]
					(const std::vector<triggerkey> &v)
					{
						const char *sep="";

						for (const auto &k:v)
						{
							o << sep
							  << "key="
							  << k.keysym
							  << ", modified="
							  << k.modifier
							  << ", mask="
							  << k.mask
							  << std::endl;
							sep="; ";
						}
					};
				dump(on_keys);
				o << "; OFF: ";
				dump(off_keys);

				o.str();
			}));
}

void ximserverObj::received_xim_sync(IN_THREAD_ONLY,
				     xim_im_t input_method_id,
				     xim_ic_t input_context_id)
{
	xim_sync_reply_send(IN_THREAD, input_method_id, input_context_id);
}

class LIBCXX_HIDDEN ximserverObj::sync_requestObj : public ximrequestObj {

 public:

	sync_requestObj(const char *request_type)
		: ximrequestObj(request_type)
	{
	}

	~sync_requestObj() noexcept
	{
	}

	void xim_sync_reply(IN_THREAD_ONLY,
			    const ximserver &server,
			    uint16_t input_context_id) override
	{
		// No-op
	}
};

void ximserverObj::received_xim_sync_reply(IN_THREAD_ONLY,
					   xim_im_t input_method_id,
					   xim_ic_t input_context_id)
{
	sync_reply_received(IN_THREAD,
			    [&, this]
			    (const ximrequest &req)
			    {
				    req->xim_sync_reply(IN_THREAD,
							ximserver(this),
							input_context_id);
			    });
}

void ximserverObj::received_xim_create_ic_reply(IN_THREAD_ONLY,
						xim_im_t input_method_id,
						xim_ic_t input_context_id)
{
	sync_reply_received(IN_THREAD,
			    [&, this]
			    (const ximrequest &req)
			    {
				    req->xim_create_ic_reply(IN_THREAD,
							     ximserver(this),
							     input_context_id);
			    });
}

void ximserverObj::received_xim_destroy_ic_reply(IN_THREAD_ONLY,
						 xim_im_t input_method_id,
						 xim_ic_t input_context_id)
{
	sync_reply_received(IN_THREAD,
			    [&, this]
			    (const ximrequest &req)
			    {
				    req->xim_destroy_ic_reply(IN_THREAD,
							      ximserver(this),
							      input_context_id);
			    });
}


void ximserverObj::set_spot_location(IN_THREAD_ONLY,
				     const ximclient &client)
{
	add_client_request
		(IN_THREAD,
		 ximrequest::create("XIM_SET_IC_VALUES(spotLocation)"),
		 client,
		 []
		 (IN_THREAD_ONLY,
		  const ximserver &me, const ximclient &client)
		 {
			 auto &pos=client->sent_cursor_position(IN_THREAD);
			 if (client->reported_cursor_position(IN_THREAD) ==
			     pos)
				 return false;

			 pos=client->reported_cursor_position(IN_THREAD);
			 std::vector<attrvalue> attrs;

			 set_preedit_position
				 (me->ic_attributes_by_name(IN_THREAD),
				  pos,
				  attrs);

			 LOG_DEBUG("Sending cursor position based on "
				   << pos
				   << "(" << attrs.size() << ")");

			 me->xim_set_ic_values_send(IN_THREAD,
						    me->input_method_id
						    (IN_THREAD),
						    client->input_context_id
						    (IN_THREAD),
						    attrs);
#if 0
			 me->get_ic_values(IN_THREAD, client,
					   {"spotlocation","area"});
#endif
			 return true;
		 });
}

void ximserverObj::focus_state(IN_THREAD_ONLY, const ximclient &client,
			       bool flag)
{
	add_client_request
		(IN_THREAD,
		 ximrequest::create(flag ?
				    "XIM_SET_IC_FOCUS":"XIM_UNSET_IC_FOCUS"),
		 client,
		 [flag]
		 (IN_THREAD_ONLY,
		  const ximserver &me, const ximclient &client)
		 {
			 if (client->sent_focus(IN_THREAD) == flag)
				 return false;

			 client->sent_focus(IN_THREAD)=flag;

			 if (flag)
				 me->xim_set_ic_focus_send
					 (IN_THREAD,
					  me->input_method_id(IN_THREAD),
					  client->input_context_id(IN_THREAD));
			 else
				 me->xim_unset_ic_focus_send
					 (IN_THREAD,
					  me->input_method_id(IN_THREAD),
					  client->input_context_id(IN_THREAD));

			 // This is not really a synchronous request, it's
			 // just more convenient to implement everything using
			 // add_request().

			 return false;
		 });

}

bool ximserverObj
::forward_key_press_release_event(IN_THREAD_ONLY,
				  const ximclient &client,
				  const xcb_key_release_event_t &e,
				  uint16_t sequencehi,
				  uint32_t mask)
{
	if (client->input_context_id(IN_THREAD) == 0
	    || !(client->forward_event_mask(IN_THREAD) & mask))
		return false;

	// If the X input method wants events to be forwarded
	// synchronously, queue up a synchronous request.
	if (client->synchronous_event_mask(IN_THREAD) & mask)
	{
		LOG_DEBUG("Forwarding key event synchronously");

		auto req=ref<sync_requestObj>
			::create("xim_forward_keypress_event");

		add_client_request
			(IN_THREAD, req, client,
			 [e, sequencehi]
			 (IN_THREAD_ONLY, const ximserver &server,
			  const ximclient &client)
			 {
				 server->xim_forward_keypress_event_send
					 (IN_THREAD,
					  server->input_method_id(IN_THREAD),
					  client->input_context_id(IN_THREAD),
					  3, // sync+request_filtering
					  sequencehi,
					  e);
				 return true;
			 });
	}
	else
	{
		// Otherwise, send asynchronously.
		LOG_DEBUG("Forwarding key event asynchronously");

		xim_forward_keypress_event_send
			(IN_THREAD,
			 input_method_id(IN_THREAD),
			 client->input_context_id(IN_THREAD),
			 2,
			 sequencehi, e);
	}

	return true;
}

void ximserverObj::received_xim_forwarded_event(IN_THREAD_ONLY,
						xim_im_t input_method_id,
						xim_ic_t input_context_id,
						uint16_t flag,
						uint16_t sequencehi,
						const eventbuf_t &eventbuf)
{
	LOG_DEBUG("Received forwarded event: flag=" << flag);

	auto sentry=make_sentry
		([&]
		 {
			 if (flag & 1)
				 // XIM server wants a sync reply.
				 xim_sync_reply_send(IN_THREAD,
						     input_method_id,
						     input_context_id);
		 });

	sentry.guard();

	switch (eventbuf[0]) {
	case KeyPress:
	case KeyRelease:

		// Filtered event, can now be forwarded back to the input
		// context's window, for processing.
		{
			xcb_key_press_event_t e{};

			const uint8_t *p=eventbuf;
			size_t size=sizeof(eventbuf);

			key_press_release_event_received(IN_THREAD, p, size, e);

			auto c=input_contexts.find(input_context_id);

			if (c == input_contexts.end())
			{
				LOG_WARNING("Unable to find the input context for a forwarded key event");
				return;
			}

			try {
				c->second->client_window->handle_key_event
					(IN_THREAD,
					 &e,
					 eventbuf[0] == KeyPress);
			} CATCH_EXCEPTIONS;
		}
		break;
	default:
		LOG_WARNING("Received unknown forwarded event "
			    << (int)eventbuf[0]);
	}
}

void ximserverObj::received_xim_commit(IN_THREAD_ONLY,
				       xim_im_t input_method_id,
				       xim_ic_t input_context_id,
				       uint16_t flag,
				       uint32_t keysym,
				       const std::string &string)
{
	LOG_DEBUG("Received commit string: flag=" << flag
		  << ", keysym=" << keysym << ", string:" << ({
				  std::ostringstream o;

				  o << std::hex << std::setw(2)
				    << std::setfill('0');

				  for (unsigned char c:string)
					  o << (int)c;

				  o.str();
			  }));

	if (flag & 1) // XIM server wants a sync reply.
		xim_sync_reply_send(IN_THREAD, input_method_id,
				    input_context_id);

	auto c=input_contexts.find(input_context_id);

	if (c == input_contexts.end())
	{
		LOG_WARNING("Unable to find the input context for a committed string");
		return;
	}

	try {
		c->second->client_window->pasted_string
			(IN_THREAD,
			 encoding(IN_THREAD)->to_ustring(string));
	} CATCH_EXCEPTIONS;
}

void ximserverObj::received_xim_set_ic_values_reply(IN_THREAD_ONLY,
						    xim_im_t input_method_id,
						    xim_ic_t input_context_id)
{
	sync_reply_received(IN_THREAD,
			    [&, this]
			    (const ximrequest &req)
			    {
				    // Nothing needs to be done.
			    });

}
#if 0
class LIBCXX_HIDDEN ximserverObj::get_ic_valuesObj : public ximrequestObj {

public:
	get_ic_valuesObj() : ximrequestObj("XIM_GET_IC_VALUES")
	{
	}

	~get_ic_valuesObj()=default;

	void xim_get_ic_values_reply(IN_THREAD_ONLY,
				     const ximserver &server,
				     uint16_t input_context_id,
				     const std::vector<ximattrvalue>
				     &ic_attributes) override;
};

void ximserverObj::get_ic_valuesObj
::xim_get_ic_values_reply(IN_THREAD_ONLY,
			  const ximserver &server,
			  uint16_t input_context_id,
			  const std::vector<attrvalue> &ic_attributes)
{
	LOG_DEBUG(({
				std::ostringstream o;

				o << "Received IC attributes:" << std::endl;

				std::map<uint16_t, std::string> lookup;

				for (const auto &i:server
					     ->ic_attributes_by_name(IN_THREAD))
					lookup.insert({i.second.id,
								i.first});

				for (const auto &a:ic_attributes)
				{
					auto p=lookup.find(a.id);

					o << "    " << (p == lookup.end()
					      ? "(unknown)":p->second)
					  << ": ("
					  << a.value.size() << "): ";

					for (auto byte:a.value)
					{
						o << ' '
						  << std::hex
						  << std::setw(2)
						  << std::setfill('0')
						  << (int)byte
						  << std::dec;
					}
					o << std::endl;
				}

				o.str();
			}));
}

void ximserverObj::get_ic_values(IN_THREAD_ONLY,
				 const ximclient &client,
				 const std::vector<std::string> &values)
{
	add_client_request
		(IN_THREAD,
		 ref<get_ic_valuesObj>::create(),
		 client,
		 [values]
		 (IN_THREAD_ONLY,
		  const ximserver &me, const ximclient &client)
		 {
			 std::vector<uint16_t> ids;

			 ids.reserve(values.size());

			 for (const auto &name:values)
			 {
				 auto iter=me->ic_attributes_by_name(IN_THREAD)
					 .find(name);

				 if (iter==me
				     ->ic_attributes_by_name(IN_THREAD).end())
				 {
					 LOG_ERROR("No such attribute: "
						   << name);
					 continue;
				 }
				 ids.push_back(iter->second.id);
			 }
			 if (ids.empty())
				 return false;

			 me->xim_get_ic_values_send(IN_THREAD,
						    me->input_method_id
						    (IN_THREAD),
						    client->input_context_id
						    (IN_THREAD),
						    ids);
			 return true;
		 });
}

void ximserverObj
::received_xim_get_ic_values_reply(IN_THREAD_ONLY,
				   xim_im_t input_method_id,
				   xim_ic_t input_context_id,
				   const std::vector<attrvalue> &ic_attributes)
{
	sync_reply_received(IN_THREAD,
			    [&, this]
			    (const ximrequest &req)
			    {
				    req->xim_get_ic_values_reply
					    (IN_THREAD,
					     ximserver(this),
					     input_context_id,
					     ic_attributes);
			    });


}
#endif

void ximserverObj::received_xim_error(IN_THREAD_ONLY,
				      xim_im_t input_method_id,
				      xim_ic_t input_context_id,
				      uint16_t flag,
				      uint16_t error_code,
				      uint16_t error_type,
				      const std::string &error_detail)
{
	std::ostringstream o;

	switch (error_code) {
	case 1: o << "BadAlloc"; break;
	case 2: o << "BadStyle"; break;
	case 3: o << "BadClientWindow"; break;
	case 4: o << "BadFocusWindow"; break;
	case 5: o << "BadArea"; break;
	case 6: o << "BadSpotLocation"; break;
	case 7: o << "BadColormap"; break;
	case 8: o << "BadAtom"; break;
	case 9: o << "BadPixel"; break;
	case 10: o << "BadPixmap"; break;
	case 11: o << "BadName"; break;
	case 12: o << "BadCursor"; break;
	case 13: o << "BadProtocol"; break;
	case 14: o << "BadForeground"; break;
	case 15: o << "BadBackground"; break;
	case 16: o << "LocaleNotSupported"; break;
	case 999: o << "BadSomething"; break;
	default:
		o << "#" << error_code;
	}

	if (error_detail.size())
		o << ", " << error_detail;

	LOG_ERROR("Error: " << o.str());


	if (!fully_connected)
	{
		// The connection has not completed, so it must be
		// broken.

		xim_disconnected(IN_THREAD);
		return;
	}

	// If there's a synchronous request pending,
	// this error is for this request. Pop it off
	// and invoke its error handler.
	if (!request_queue.empty())
	{
		auto r=request_queue.front();
		request_queue.pop_front();

		try {
			r.second->error(o.str());
		} CATCH_EXCEPTIONS;

		send_next_request(IN_THREAD);
	}
}

LIBCXXW_NAMESPACE_END
