#include "libcxxw_config.h"
#include "xim/ximserver.H"
#include "xim/ximclient.H"
#include "xid_t.H"
#include "connection_thread.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

ximclientObj::ximclientObj(const ximserver &server,
			   const ref<window_handlerObj> &client_window)
	: server(server), client_window(client_window)
{
}

ximclientObj::~ximclientObj()=default;

void ximclientObj::xim_client_register() noexcept
{
	client_window->thread()->run_as
		([client_window=this->client_window, me=ximclient(this)]
		 (IN_THREAD_ONLY)
		 {
			 client_window->ximclient_ptr=me;
			 me->server->create_client(IN_THREAD, me);
		 });
}

void ximclientObj::xim_client_deregister() noexcept
{
	client_window->thread()->run_as
		([client_window=this->client_window, me=ximclient(this)]
		 (IN_THREAD_ONLY)
		 {
			 client_window->ximclient_ptr=nullptr;
			 me->server->destroy_client(IN_THREAD, me);
		 });
}

void ximclientObj::current_cursor_position(IN_THREAD_ONLY,
					   const rectangle &r)
{
	if (r == reported_cursor_position(IN_THREAD))
		return;
	reported_cursor_position(IN_THREAD)=r;

	server->set_spot_location(IN_THREAD, ximclient(this));
}

void ximclientObj::focus_state(IN_THREAD_ONLY, bool flag)
{
	if (flag == reported_focus(IN_THREAD))
		return;

	reported_focus(IN_THREAD)=flag;
	server->focus_state(IN_THREAD, ximclient(this), flag);
}

bool ximclientObj::forward_key_press_event(IN_THREAD_ONLY,
					   const xcb_key_press_event_t &e,
					   uint16_t sequencehi)
{
	return server->forward_key_press_release_event
		(IN_THREAD, ximclient(this), e, sequencehi,
		 XCB_EVENT_MASK_KEY_PRESS);
}

bool ximclientObj::forward_key_release_event(IN_THREAD_ONLY,
					     const xcb_key_release_event_t &e,
					     uint16_t sequencehi)
{
	return server->forward_key_press_release_event
		(IN_THREAD, ximclient(this), e, sequencehi,
		 XCB_EVENT_MASK_KEY_RELEASE);
}

LIBCXXW_NAMESPACE_END
