#include "libcxxw_config.h"
#include "xim/ximserver.H"
#include "xim/ximclient.H"
#include "xid_t.H"
#include "connection_thread.H"

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
		(RUN_AS,
		 [client_window=this->client_window, me=ximclient(this)]
		 (IN_THREAD_ONLY)
		 {
			 client_window->ximclient_ptr=me;
			 me->server->create_client(IN_THREAD, me);
		 });
}

void ximclientObj::xim_client_deregister() noexcept
{
	client_window->thread()->run_as
		(RUN_AS,
		 [client_window=this->client_window, me=ximclient(this)]
		 (IN_THREAD_ONLY)
		 {
			 client_window->ximclient_ptr=nullptr;
			 me->server->destroy_client(IN_THREAD, me);
		 });
}

LIBCXXW_NAMESPACE_END
