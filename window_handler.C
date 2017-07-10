/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "window_handler.H"
#include "connection_thread.H"
#include "assert_or_throw.H"
#include "selection/current_selection.H"
#include "selection/incremental_selection_updates.H"
#include "xim/ximclient.H"
#include <x/vector.H>
#include <X11/X.h>
#include <X11/Xatom.h>

LIBCXXW_NAMESPACE_START

window_handlerObj
::window_handlerObj(IN_THREAD_ONLY,
		    const constructor_params &params)
	: xid_t<xcb_window_t>(IN_THREAD),
	screenref(params.screenref)
{
	auto width=params.initial_position.width;
	auto height=params.initial_position.height;

	// We can logically attempt to create a window with zero width or
	// height (empty container, for example). X will complain about
	// BadValue, so turn this into a tiny 1x1 window.

	if (width == 0 || height == 0)
		width=height=1;

	if (width == width.infinite() ||
	    height == height.infinite())
		throw EXCEPTION("Internal error, invalid initial display element size");

	xcb_create_window(conn()->conn,
			  (depth_t::value_type)params.depth,
			  id(),
			  params.parent,
			  (coord_t::value_type)params.initial_position.x,
			  (coord_t::value_type)params.initial_position.y,
			  (dim_t::value_type)width,
			  (dim_t::value_type)height,

			  0,
			  // Border width. For non-WM controlled popups, this
			  // seems to adjust visible popup window location!

			  params.window_class,
			  params.visual,
			  params.events_and_mask.mask(),
			  params.events_and_mask.values().data());
}

window_handlerObj::~window_handlerObj()
{
	xcb_destroy_window(conn()->conn, id());
}

void window_handlerObj::installed(IN_THREAD_ONLY)
{
}

void window_handlerObj::disconnected(IN_THREAD_ONLY)
{
}


void window_handlerObj::ungrab(IN_THREAD_ONLY)
{
	grab_locked(IN_THREAD)=false;
	release_grabs(IN_THREAD);
}

void window_handlerObj::release_grabs(IN_THREAD_ONLY)
{
	auto timestamp=grabbed_timestamp(IN_THREAD);

	if (timestamp == XCB_CURRENT_TIME)
		return;

	if (grab_locked(IN_THREAD))
		return;

	grabbed_timestamp(IN_THREAD)=XCB_CURRENT_TIME;

	xcb_ungrab_pointer(IN_THREAD->info->conn, timestamp);
	xcb_ungrab_keyboard(IN_THREAD->info->conn, timestamp);
}

void window_handlerObj::change_property(IN_THREAD_ONLY,
					uint8_t mode,
					xcb_atom_t property,
					xcb_atom_t type,
					uint8_t format,
					uint32_t data_len,
					void *data)
{
	xcb_change_property(IN_THREAD->info->conn, mode, id(), property,
			    type, format, data_len, data);
}

void window_handlerObj::configure_notify(IN_THREAD_ONLY,
					 const rectangle &)
{
}

void window_handlerObj::client_message_event(IN_THREAD_ONLY,
					     const xcb_client_message_event_t *)
{
}

void window_handlerObj::exposure_event(IN_THREAD_ONLY,
				       const rectangle_set &)
{
}

void window_handlerObj::theme_updated_event(IN_THREAD_ONLY)
{
}

void window_handlerObj::key_press_event(IN_THREAD_ONLY,
					const xcb_key_press_event_t *event,
					uint16_t sequencehi)
{
}

void window_handlerObj::handle_key_event(IN_THREAD_ONLY,
					 const xcb_key_release_event_t *event,
					 bool keypress)
{
}

void window_handlerObj::key_release_event(IN_THREAD_ONLY,
					  const xcb_key_release_event_t *event,
					  uint16_t sequencehi)
{
}

void window_handlerObj::button_press_event(IN_THREAD_ONLY,
					   const xcb_button_press_event_t *event)
{
}

void window_handlerObj::button_release_event(IN_THREAD_ONLY,
					     const xcb_button_release_event_t *event)
{
}

void window_handlerObj::pointer_motion_event(IN_THREAD_ONLY,
					     const xcb_motion_notify_event_t *)
{
}

void window_handlerObj::enter_notify_event(IN_THREAD_ONLY,
					     const xcb_enter_notify_event_t *)
{
}

void window_handlerObj::leave_notify_event(IN_THREAD_ONLY,
					   const xcb_leave_notify_event_t *)
{
}

void window_handlerObj::focus_change_event(IN_THREAD_ONLY, bool)
{
}


LOG_FUNC_SCOPE_DECL(INSERT_LIBX_NAMESPACE::w::selection, selection_debug_log);

void window_handlerObj
::selection_request_event(IN_THREAD_ONLY,
			  const xcb_selection_request_event_t &request,
			  xcb_selection_notify_event_t &reply)
{
	LOG_FUNC_SCOPE(selection_debug_log);

	reply.response_type=XCB_SELECTION_NOTIFY;
	reply.time=request.time;
	reply.requestor=request.requestor;
	reply.selection=request.selection;
	reply.property=request.property;

	auto iter=selections(IN_THREAD).find(request.selection);

	if (iter == selections(IN_THREAD).end() ||
	    iter->second->timestamp > request.time)
	{
		LOG_DEBUG("Selection not found");
		reply.property=XCB_NONE;
		return;
	}

	if (reply.target == IN_THREAD->info->atoms_info.multiple &&
	    reply.property != XCB_NONE)
	{
		selection_request_multiple(IN_THREAD, request, reply,
					   iter->second);
		return;
	}

	if (reply.property == XCB_NONE)
		reply.property=reply.target;

	auto v=reply.target != IN_THREAD->info->atoms_info.targets
		? iter->second->convert(IN_THREAD, request.target)
		: ({
				// TARGETS

				LOG_DEBUG("TARGETS requested");
				auto targets=iter->second->supported(IN_THREAD);

				vector<uint8_t> data=
					vector<uint8_t>::create
					(reinterpret_cast<uint8_t *>
					 (&targets[0]),
					 reinterpret_cast<uint8_t *>
					 (&targets[targets.size()]));

				ptr<current_selectionObj::convertedValueObj>
					::create(XA_ATOM, 32,
						 data,
						 data->begin(),
						 data->end());
			});


	if (v.null())
	{
		LOG_DEBUG("Cannot convert selection to requested format");
		reply.property=XCB_NONE;
		reply.target=request.target;
		return;
	}

	// If the supplied value exceeds maximum chunk size, replace 'v'
	// with ConvertedIncrementalValueObj, which will deal it out piecemeal.

	auto setup=xcb_get_setup(IN_THREAD->info->conn);

	assert_or_throw(setup->maximum_request_length >= 1024,
			"WTF? maximum request length < 1024 bytes???");

	size_t max_chunk_size=setup->maximum_request_length-1024;

	max_chunk_size = max_chunk_size/4*4;

	if (!v->incremental && v->data->size() > max_chunk_size)
		v=ref<current_selectionObj::convertedIncrementalValueObj>
			::create(v, max_chunk_size, IN_THREAD->info
				 ->atoms_info.incr);

	reply.target=v->type;

	if (v->incremental)
	{
		LOG_DEBUG("Beginning incremental selection update of property "
			  << request.property
			  << " of window " << request.requestor);

		auto &existing_updates=
			IN_THREAD->pending_incremental_updates(IN_THREAD)
			->get_updates_for_window(IN_THREAD, request.requestor);

		auto ret=existing_updates.updates
			.insert({request.property,v});

		if (!ret.second)
		{
			LOG_ERROR("Multiple incremental updates of the same "
				  "property to the same window");
			ret.first->second=v;
			// It's possible that the first one was lost somewhere?
		}
	}

	LOG_DEBUG("Updated property "
		  << IN_THREAD->info->get_atom_name(reply.property)
		  << " of window " << request.requestor
		  << " using format " << IN_THREAD->info->get_atom_name(v->type)
		  << " (" << (v->data_end-v->data_begin)
		  << " bytes, format="
		  << (int)v->format << ")");

	xcb_change_property(IN_THREAD->info->conn,
			    XCB_PROP_MODE_REPLACE,
			    request.requestor,
			    reply.property,
			    v->type,
			    v->format,
			    (v->data_end-v->data_begin)/(v->format/8),
			    &*v->data_begin);
}

void window_handlerObj::
pasted_string(IN_THREAD_ONLY,
	      const std::experimental::u32string_view &)
{
}

void window_handlerObj
::selection_request_multiple(IN_THREAD_ONLY,
			     const xcb_selection_request_event_t &request,
			     xcb_selection_notify_event_t &reply,
			     const current_selection &selection)
{
	LOG_FUNC_SCOPE(selection_debug_log);

	LOG_DEBUG("Converting MULTIPLE selection");

	std::vector<xcb_atom_t> atoms;
	bool error=false;

	IN_THREAD->info->get_entire_property_with
		(request.requestor, reply.target,
		 XCB_GET_PROPERTY_TYPE_ANY, false,
		 [&]
		 (xcb_atom_t type,
		  uint8_t format,
		  void *data,
		  size_t data_size)
		 {
			 if (format != 32)
			 {
				 error=true;
				 return;
			 }
			 auto p=reinterpret_cast<xcb_atom_t *>(data);

			 atoms.insert(atoms.end(),
				      p,
				      p+data_size/sizeof(*p));
		 });

	if (error || (atoms.size() & 1))
	{
		LOG_ERROR("Received bad MULTIPLE conversion request.");
		return;
	}

	bool conversion_failed=false;

	for (size_t i=0; i<atoms.size(); i += 2)
	{
		auto v=selection->convert(IN_THREAD, atoms[i]);

		if (v.null())
		{
			atoms[i]=XCB_NONE;
			conversion_failed=true;
			continue;
		}

		if (v->incremental)
		{
			LOG_ERROR("Cannot send incremental update in a MULTIPLE conversion request");
			atoms[i]=XCB_NONE;
			conversion_failed=true;
			continue;
		}

		xcb_change_property(IN_THREAD->info->conn,
				    XCB_PROP_MODE_REPLACE,
				    request.requestor,
				    atoms[i+1],
				    v->type,
				    v->format,
				    (v->data_end-v->data_begin)/(v->format/8),
				    &*v->data_begin);
	}

	if (conversion_failed)
	{
		xcb_change_property(IN_THREAD->info->conn,
				    XCB_PROP_MODE_REPLACE,
				    request.requestor,
				    reply.target,
				    XA_ATOM,
				    32,
				    atoms.size(),
				    &atoms[0]);

	}
}

LIBCXXW_NAMESPACE_END
