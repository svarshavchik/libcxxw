/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_info_h
#define connection_info_h

#include "connection.H"
#include "connection_infofwd.H"
#include "builtin_atoms.H"
#include <x/threadmsgdispatcher.H>
#include <x/mpobj.H>
#include <queue>

LIBCXXW_NAMESPACE_START

// An opened xcb_connection_t handle.

class LIBCXX_HIDDEN connection_infoObj : virtual public obj {
 public:

	class connection_handle;

	xcb_connection_t * const conn;

	const int default_screen;

	//! Atoms

	const builtin_atoms atoms_info;

	connection_infoObj(const std::experimental::string_view &display);

	connection_infoObj(connection_handle &&handle);

	~connection_infoObj();

	// Allocate new XIDs, via xcb_generate_id().

	uint32_t alloc_xid();

	// An XID is no longer in use, make it available to alloc_xid()

	void release_xid(uint32_t) noexcept;
 private:
	typedef mpobj<std::queue<uint32_t>> available_xids_t;

	available_xids_t available_xids;
};

LIBCXXW_NAMESPACE_END

#endif
