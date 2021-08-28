/*
** Copyright 2017-2021 Double Precision, Inc.
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
::window_handlerObj(ONLY IN_THREAD,
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

void window_handlerObj::installed(ONLY IN_THREAD)
{
}

void window_handlerObj::disconnected(ONLY IN_THREAD)
{
}

bool window_handlerObj::keep_passive_grab(ONLY IN_THREAD)
{
	if (grabbed_timestamp(IN_THREAD) != XCB_CURRENT_TIME &&
	    !grab_locked(IN_THREAD))
	{
		grab_locked(IN_THREAD)=true;
		xcb_allow_events(IN_THREAD->info->conn,
				 XCB_ALLOW_ASYNC_BOTH,
				 grabbed_timestamp(IN_THREAD));
		return true;
	}
	return false;
}

void window_handlerObj::ungrab(ONLY IN_THREAD)
{
	grab_locked(IN_THREAD)=false;
	release_grabs(IN_THREAD);
}

bool window_handlerObj::is_pointer_actively_grabbed(ONLY IN_THREAD)
{
	return false;
}

void window_handlerObj::release_grabs(ONLY IN_THREAD)
{
	auto timestamp=grabbed_timestamp(IN_THREAD);

	if (timestamp == XCB_CURRENT_TIME)
		return;

	if (grab_locked(IN_THREAD))
		return;

	grabbed_timestamp(IN_THREAD)=XCB_CURRENT_TIME;

	// We only want to unwind a passive grab. Don't attempt to unwind
	// an active grab.
	if (!is_pointer_actively_grabbed(IN_THREAD))
		xcb_ungrab_pointer(IN_THREAD->info->conn, timestamp);
	xcb_ungrab_keyboard(IN_THREAD->info->conn, timestamp);
}

void window_handlerObj::idle(ONLY IN_THREAD)
{
}

void window_handlerObj::change_property(ONLY IN_THREAD,
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

void window_handlerObj::configure_notify_received(ONLY IN_THREAD,
						  const rectangle &)
{
}

void window_handlerObj::process_configure_notify(ONLY IN_THREAD)
{
}

void window_handlerObj::process_map_notify_event(ONLY IN_THREAD)
{
}

void window_handlerObj::process_unmap_notify_event(ONLY IN_THREAD)
{
}

void window_handlerObj::client_message_event(ONLY IN_THREAD,
					     const xcb_client_message_event_t *)
{
}

void window_handlerObj::process_collected_exposures(ONLY IN_THREAD)
{
}

void window_handlerObj::process_collected_graphics_exposures(ONLY IN_THREAD)
{
}

void window_handlerObj::theme_updated_event(ONLY IN_THREAD)
{
}

void window_handlerObj::key_press_event(ONLY IN_THREAD,
					const xcb_key_press_event_t *event,
					uint16_t sequencehi)
{
}

bool window_handlerObj::handle_key_event(ONLY IN_THREAD,
					 const xcb_key_release_event_t *event,
					 bool keypress)
{
	return false;
}

void window_handlerObj::key_release_event(ONLY IN_THREAD,
					  const xcb_key_release_event_t *event,
					  uint16_t sequencehi)
{
}

void window_handlerObj::button_press_event(ONLY IN_THREAD,
					   const xcb_button_press_event_t *event)
{
}

void window_handlerObj::button_release_event(ONLY IN_THREAD,
					     const xcb_button_release_event_t *event)
{
}

void window_handlerObj::pointer_motion_event(ONLY IN_THREAD,
					     const xcb_motion_notify_event_t *)
{
}

void window_handlerObj::enter_notify_event(ONLY IN_THREAD,
					     const xcb_enter_notify_event_t *)
{
}

void window_handlerObj::leave_notify_event(ONLY IN_THREAD,
					   const xcb_leave_notify_event_t *)
{
}

void window_handlerObj::focus_change_event(ONLY IN_THREAD, bool)
{
}


LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::selection, selection_debug_log);

void window_handlerObj
::selection_request_event(ONLY IN_THREAD,
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

	LOG_DEBUG("Converting to "
		  << IN_THREAD->info->get_atom_name(request.target));

	if (request.target == IN_THREAD->info->atoms_info.multiple &&
	    reply.property != XCB_NONE)
	{
		selection_request_multiple(IN_THREAD, request, reply,
					   iter->second);
		return;
	}

	auto v=request.target != IN_THREAD->info->atoms_info.targets
		? iter->second->convert(IN_THREAD, request.target)
		: ({
				// TARGETS

				auto targets=iter->second->supported(IN_THREAD);
				LOG_DEBUG("TARGETS requested, we support: "
					  << ({
							  std::ostringstream o;
							  const char *sep="";

							  for (auto a:targets)
							  {
								  o << sep <<
									  IN_THREAD->info->get_atom_name(a);
								  sep=", ";
							  }
							  o.str();
						  }));

				auto b=&*targets.begin();
				auto e=b+targets.size();

				vector<uint8_t> data=
					vector<uint8_t>::create
					(reinterpret_cast<uint8_t *>(b),
					 reinterpret_cast<uint8_t *>(e));

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
pasted_string(ONLY IN_THREAD,
	      const std::u32string_view &)
{
}

void window_handlerObj
::selection_request_multiple(ONLY IN_THREAD,
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
			LOG_DEBUG("Cannot convert to "
				  << IN_THREAD->info->get_atom_name(atoms[i]));

			atoms[i]=XCB_NONE;
			conversion_failed=true;
			continue;
		}

		LOG_DEBUG("Converting to "
			  << IN_THREAD->info->get_atom_name(atoms[i]));

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

bool window_handlerObj
::transfer_focus_to_next_window(ONLY IN_THREAD)
{
	if (!will_accept_transferred_focus(IN_THREAD))
		// A popup that doesn't accept it, won't transfer it either.
		return false;

	auto &handlers=*IN_THREAD->window_handlers(IN_THREAD);

	auto iter=handlers.find(id());

	auto e=handlers.end();

	if (iter == e)
		return false; // Can't find myself?

	while (++iter != e)
	{
		if (iter->second->will_accept_transferred_focus(IN_THREAD))
		{
			xcb_set_input_focus(conn()->conn,
					    XCB_INPUT_FOCUS_PARENT,
					    iter->first,
					    IN_THREAD->timestamp(IN_THREAD));
			return true;
		}
	}

	// Take it from the top

	for (const auto &wh:handlers)
		if (wh.first != id() &&
		    wh.second->will_accept_transferred_focus(IN_THREAD))
		{
			xcb_set_input_focus(conn()->conn,
					    XCB_INPUT_FOCUS_PARENT,
					    wh.first,
					    IN_THREAD->timestamp(IN_THREAD));
			return true;
		}

	return false;
}

bool window_handlerObj::will_accept_transferred_focus(ONLY IN_THREAD)
{
	return false;
}

void window_handlerObj::set_default_focus(ONLY IN_THREAD)
{
}

void window_handlerObj::flush_redrawn_areas(ONLY IN_THREAD)
{
}

bool window_handlerObj::process_focus_updates(ONLY IN_THREAD)
{
	return false;
}

LIBCXXW_NAMESPACE_END
