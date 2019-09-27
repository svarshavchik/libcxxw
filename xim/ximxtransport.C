/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "xim/ximxtransport.H"
#include "xim/ximxtransport_impl.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "connection.H"
#include "connection_thread.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

ximxtransportObj::ximxtransportObj(const screen &s)
	: ximxtransportObj
	  (ref<implObj>::create(s->get_connection()->impl->thread,
				ximxtransportObj::implObj::constructor_params{
					s,
					s->impl->xcb_screen->root, // parent
					0, // depth
					{0, 0, 1, 1}, // initial position,
					XCB_WINDOW_CLASS_INPUT_ONLY,
					s->impl->toplevelwindow_visual->impl
						->visual_id, // visual
					{
						XCB_CW_EVENT_MASK,
							(uint32_t)
							generic_windowObj
							::handlerObj
							::initial_event_mask(),
					}, // events_and_mask
				 }))
{
}

ximxtransportObj::ximxtransportObj(const ref<implObj> &impl)
	: impl(impl), window_owner(ref<windowObj>::create(impl))
{
	impl->thread()->run_as([impl]
			       (ONLY IN_THREAD)
			       {
				       impl->connect(IN_THREAD);
			       });
}

ximxtransportObj::~ximxtransportObj()
{
	// There are two possible race conditions here:
	//
	// 1) the connection thread ended up owning the last reference to
	// the main window, or
	//
	// 2) the main thread still has some references to secondary objects
	// that hold locks, such as layout manager objects, in automatic
	// scope which are going to be destoryed, but not after the top level
	// main window objects gets destroyed, and we're called as part of that
	// operation. This can happen if, for example, a uielements gets
	// constructed in auto scope first, then the mainwindow object, then
	// the main window object's creator generates the main window's
	// contents, which populates the uielements' contents with the
	// constructed layout manager objects, and then an exception gets
	// thrown, which unwinds everything.
	//
	// What we're trying to do here is an orderly shutdown of the X
	// transport.
	//
	// The Ibus XIM server is buggy, and fails to clean up
	// after itself unless the client does an orderly
	// disconnection. We're going out of our way to make sure
	// we do an orderly disconnect. But to do this we need
	// to wait until for things to naturally run its course,
	// while the connection thread is running.
	//
	// We'll kick off a separate thread that will wait for the
	// XIM transport to disconnect. The separate thread captures
	// by value the refs to both impl and owner, which is also
	// going to keep everything alive, until this is done.

	run_lambda([impl=this->impl, window_owner=this->window_owner]
		   {
			   impl->wait_until_disconnected();
		   });
}

LIBCXXW_NAMESPACE_END
