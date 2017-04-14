/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "selection/current_selection.H"
#include "window_handler.H"
#include "connection_thread.H"
#include "catch_exceptions.H"
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::window_handlerObj::selection,
		    selection_log);

current_selectionObj::current_selectionObj(xcb_timestamp_t timestamp)
	: timestamp(timestamp)
{
}

current_selectionObj::~current_selectionObj()=default;

///////////////////////////////////////////////////////////////////////////////

current_selectionObj::convertedValueObj
::convertedValueObj(xcb_atom_t typeArg, uint8_t formatArg,
		    const vector<uint8_t> &dataArg)
	: convertedValueObj(typeArg, formatArg, dataArg,
			    dataArg->begin(), dataArg->end())
{
}

current_selectionObj::convertedValueObj
::convertedValueObj(xcb_atom_t typeArg, uint8_t formatArg,
		    const vector<uint8_t> &dataArg,
		    std::vector<uint8_t>::iterator data_beginArg,
		    std::vector<uint8_t>::iterator data_endArg)
	: type(typeArg), format(formatArg), data(dataArg), incremental(false),
	  data_begin(data_beginArg), data_end(data_endArg)
{
}

current_selectionObj::convertedValueObj::~convertedValueObj() noexcept
{
}

// This is going to be an incrementally sent value

current_selectionObj::convertedValueObj
::convertedValueObj(xcb_atom_t typeArg, uint32_t estimated_size)
	: type(typeArg), format(32),
	  data(vector<uint8_t>
	       ::create(reinterpret_cast<uint8_t *>(&estimated_size),
			reinterpret_cast<uint8_t *>(&estimated_size+1))),
	  incremental(true), data_begin(data->begin()), data_end(data->end())
{
}

ref<current_selectionObj::convertedValueObj>
current_selectionObj::convertedValueObj::next_chunk(IN_THREAD_ONLY)
{
	throw EXCEPTION("Internal error: no incremental selection available");
}

///////////////////////////////////////////////////////////////////////////////

current_selectionObj::convertedIncrementalValueObj
::convertedIncrementalValueObj(const ref<convertedValueObj> &valueArg,
			       size_t chunk_sizeArg,
			       xcb_atom_t incr_atom)
	: convertedValueObj(incr_atom,
			    (uint32_t)valueArg->data->size() ==
			    valueArg->data->size() ?
			    (uint32_t)valueArg->data->size():(uint32_t)~0),
	  original_value(valueArg),
	  chunk_size(chunk_sizeArg),
	  next_chunk_start(original_value->data->begin())
{
	chunk_size -= (chunk_size % (valueArg->format/8));
}

current_selectionObj::convertedIncrementalValueObj
::~convertedIncrementalValueObj()=default;

ref<current_selectionObj::convertedValueObj>
current_selectionObj::convertedIncrementalValueObj::next_chunk(IN_THREAD_ONLY)
{
	auto e=original_value->data->end();

	// Compute end of next chunk
	auto p=(size_t)(e-next_chunk_start) < chunk_size
					      ? e:next_chunk_start+chunk_size;

	auto c=ref<convertedValueObj>
			      ::create(original_value->type,
				       original_value->format,
				       original_value->data,
				       next_chunk_start, p);
	next_chunk_start=p;
	return c;
}

////////////////////////////////////////////////////////////////////////////

void window_handlerObj::selection_clear_event(IN_THREAD_ONLY,
					      xcb_atom_t selection_atom,
					      xcb_timestamp_t timestamp)
{
	LOG_FUNC_SCOPE(selection_log);

	LOG_DEBUG("Selection clear: "
		  << IN_THREAD->info->get_atom_name(selection_atom));

	auto iter=selections(IN_THREAD).find(selection_atom);

	if (iter == selections(IN_THREAD).end())
		return;

	auto old_selection=iter->second;
	if (old_selection->timestamp > timestamp)
		return;

	selections(IN_THREAD).erase(iter);

	try {
		old_selection->clear(IN_THREAD);
	} CATCH_EXCEPTIONS;
}

void window_handlerObj::selection_announce(IN_THREAD_ONLY,
					   xcb_atom_t selection_atom,
					   const current_selection &selection)
{
	LOG_FUNC_SCOPE(selection_log);

	LOG_DEBUG("Selection announce: "
		  << IN_THREAD->info->get_atom_name(selection_atom));

	current_selectionptr old_selection;

	auto iter=selections(IN_THREAD).find(selection_atom);

	if (iter != selections(IN_THREAD).end())
	{
		old_selection=iter->second;
		selections(IN_THREAD).erase(iter);
	}

	selections(IN_THREAD).insert({selection_atom, selection});

	xcb_set_selection_owner(conn()->conn, id(),
				selection_atom, selection->timestamp);

	if (old_selection)
		try {
			old_selection->clear(IN_THREAD);
		} CATCH_EXCEPTIONS;
}

void window_handlerObj::selection_discard(IN_THREAD_ONLY,
					  xcb_atom_t selection_atom)
{
	LOG_FUNC_SCOPE(selection_log);

	LOG_DEBUG("Selection discard: "
		  << IN_THREAD->info->get_atom_name(selection_atom));

	auto iter=selections(IN_THREAD).find(selection_atom);

	if (iter==selections(IN_THREAD).end())
		return;

	try {
		if ( iter->second->stillvalid(IN_THREAD))
			return; // Race condition, already been replaced
	} CATCH_EXCEPTIONS;

	xcb_set_selection_owner(conn()->conn, XCB_NONE, selection_atom,
				iter->second->timestamp);
	selections(IN_THREAD).erase(iter);
}

LIBCXXW_NAMESPACE_END
