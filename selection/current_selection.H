/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef current_selection_H
#define current_selection_H

#include <x/obj.H>
#include <x/vector.H>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "x/w/impl/connection_threadfwd.H"
#include "selection/current_selectionfwd.H"

LIBCXXW_NAMESPACE_START

/*! An object representing an X selection.

A window that wishes to publish a selection sends this object to the
connection thread, the selection_announce() message.

If the connection thread already has an announced selection, the
clear() method gets invoked before the connection thread replaces the
selection.

The connection thread stores this object on its execution stack.
A window that wishes to withdraw its selection makes sure that stillvalid()
is going to return false, then sends a selection_withdraw() message.

*/

class LIBCXX_HIDDEN current_selectionObj : virtual public obj {

	//! Whether this selection is still valid.

	//! Indicates whether the original display element still considers
	//! this a valid selection.
	//!
	//! A display element discards a selection by setting stillvalid to
	//! false, then calling selection_discard().
	bool stillvalid_thread_only=true;

 public:

	THREAD_DATA_ONLY(stillvalid);

	//! Selection's timestamp.
	const xcb_timestamp_t timestamp;

	//! Constructor
	current_selectionObj(xcb_timestamp_t timestamp);

	//! Destructor
	~current_selectionObj();

	//! This selection will not be used any more.

	//! Either we received a SelectionClear() message, or another
	//! display element announced its own selection.
	virtual void clear(ONLY IN_THREAD)=0;

	class convertedValueObj;
	class convertedIncrementalValueObj;

	// Convert the current selection to the requested format. Returns
	// a null ptr if the current selection value cannot be converted.
	virtual ptr<convertedValueObj> convert(ONLY IN_THREAD, xcb_atom_t)=0;

	// For which formats convert() is supported.

	virtual std::vector<xcb_atom_t> supported(ONLY IN_THREAD)=0;
};

//! A converted selection value.

class LIBCXX_HIDDEN current_selectionObj::convertedValueObj
	: virtual public obj {

 public:
	//! Type
	const xcb_atom_t type;

	//! Format
	const uint8_t format;

	//! The data
	const vector<uint8_t> data;

	//! Whether this is an incremental value.
	bool incremental;

	//! Converted data sequence.

	//! Defaults to data.begin() and data.end(). This is the effective
	//! range of the selected value, to send.

	const std::vector<uint8_t>::iterator data_begin, data_end;

	//! Simple constructor.
	convertedValueObj(xcb_atom_t typeArg, uint8_t formatArg,
			  const vector<uint8_t> &dataArg);

	//! Constructor used by convertedIncrementalValueObj only:

	convertedValueObj(xcb_atom_t typeArg, uint8_t formatArg,
			  const vector<uint8_t> &dataArg,
			  std::vector<uint8_t>::iterator data_beginArg,
			  std::vector<uint8_t>::iterator data_endArg);

	//! Constructor for an INCR-formatted selection.

	convertedValueObj(// Should be the INCR atom
			  xcb_atom_t typeArg,
			  uint32_t estimated_size);

	//! Destructor
	~convertedValueObj();

	//! Return the next chunk.

	//! Provide the next/first incremental contents. For an incrementally-
	//! provided selection, the constructor with the estimated size should
	//! be used, with the INCR atom.
	//!
	//! Each call to next_chunk() to /this/ instance should return the next
	//! chunk of the incrementally provided value, returning a zero-length
	//! value vector after it's done.

	virtual ref<convertedValueObj> next_chunk(ONLY IN_THREAD);
};

// Taking a value, and chopping it up into incremental chunks.

class LIBCXX_HIDDEN current_selectionObj::convertedIncrementalValueObj
	: public convertedValueObj {

	//! The original, entire value
	ref<convertedValueObj> original_value;

	//! What chunk size we're using.
	size_t chunk_size;

	//! Next chunk to send.
	std::vector<uint8_t>::iterator next_chunk_start; // original_value->data

 public:
	//! Constructor
	convertedIncrementalValueObj(const ref<convertedValueObj> &valueArg,
				     size_t chunk_size,
				     xcb_atom_t incr_atom);

	//! Destructor
	~convertedIncrementalValueObj();

	//! Implement next_chunk().
	ref<convertedValueObj> next_chunk(ONLY IN_THREAD) override;
};

LIBCXXW_NAMESPACE_END

#endif
