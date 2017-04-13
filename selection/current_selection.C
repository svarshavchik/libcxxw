/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "selection/current_selection.H"

LIBCXXW_NAMESPACE_START

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

LIBCXXW_NAMESPACE_END
