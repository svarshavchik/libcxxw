/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_info_h
#define connection_info_h

#include "connection.H"
#include "connection_infofwd.H"
#include "builtin_atoms.H"
#include <x/threadmsgdispatcher.H>
#include <x/mpobj.H>
#include <x/functional.H>
#include <queue>
#include <vector>
#include <string>
#include <string_view>

LIBCXXW_NAMESPACE_START

// An opened xcb_connection_t handle.

class LIBCXX_HIDDEN connection_infoObj : virtual public obj {
 public:

	class connection_handle;

	xcb_connection_t * const conn;

	const int default_screen;

	//! Atoms

	const builtin_atoms atoms_info;

	connection_infoObj(const std::string_view &display);

	connection_infoObj(connection_handle &&handle);

	~connection_infoObj();

	// Allocate new XIDs, via xcb_generate_id().

	uint32_t alloc_xid();

	// An XID is no longer in use, make it available to alloc_xid()

	void release_xid(uint32_t) noexcept;


	// Callback used by get_property()

	typedef void get_property_response(xcb_atom_t type,
					   uint8_t format,
					   uint32_t bytes_after,
					   void *data,
					   size_t data_size);

	// A thin wrapper for xcb_get_property()
	//
	// Invokes xcb_get_property. If successfull invokes the response
	// callback.

	void get_property(xcb_window_t window_id,
			  xcb_atom_t property,
			  xcb_atom_t type,
			  uint32_t offset,
			  uint32_t length,
			  bool deleteflag,
			  const function<get_property_response> &response)
		const;

	// Wrap get_property() with a lambda
	template<typename lambda>
		void get_property_with(xcb_window_t window_id,
				       xcb_atom_t property,
				       xcb_atom_t type,
				       uint32_t offset,
				       uint32_t length,
				       bool deleteflag,
				       lambda &&l) const
	{
		get_property(window_id, property, type, offset, length,
			     deleteflag,
			     make_function<get_property_response>
			     (std::forward<lambda>(l)));
	}

	// A wrapper for get_property() that retrieves the entire property.

	// Starts with the offset of 0, and reads the entire property value,
	// in chunks.
	//
	// Repeatedly invokes the callback, with every chunk of the
	// property response received.

	typedef void get_entire_property_response(xcb_atom_t type,
						  uint8_t format,
						  void *data,
						  size_t data_size);
	size_t get_entire_property(xcb_window_t window_id,
				   xcb_atom_t property,
				   xcb_atom_t type,
				   bool deleteflag,
				   const function<get_entire_property_response>
				   &response)
		const;

	// Return entire contents of the property to a lambda
	template<typename lambda>
		size_t get_entire_property_with(xcb_window_t window_id,
						xcb_atom_t property,
						xcb_atom_t type,
						bool deleteflag,
						lambda &&l) const
	{
		return get_entire_property(window_id, property, type,
					   deleteflag,
					   make_function
					   <get_entire_property_response>
					   (std::forward<lambda>(l)));
	}

	// Collect entire property into a buffer.

	// This is a wrapper for get_entire_property_with() that collects
	// the entire property's value into a buffer, invoking the callback
	// exactly once (if property retrieval succeeds).

	void collect_property(xcb_window_t window_id,
			      xcb_atom_t property,
			      xcb_atom_t type,
			      bool deleteflag,
			      const function<get_entire_property_response>
			      &response)
		const;

	template<typename lambda>
		void collect_property_with(xcb_window_t window_id,
					   xcb_atom_t property,
					   xcb_atom_t type,
					   bool deleteflag,
					   lambda &&l) const
	{
		return collect_property(window_id, property, type,
					deleteflag,
					make_function
					<get_entire_property_response>
					(std::forward<lambda>(l)));
	}

	//! Return an atom

	xcb_atom_t get_atom(const std::string_view &name,
			    bool only_if_exists=true) const;

	//! Return an atom's name

	std::string get_atom_name(xcb_atom_t atom) const;

	//! Return a selection's owner.
	xcb_window_t get_selection_owner(xcb_atom_t atom) const;
 private:
	typedef mpobj<std::queue<uint32_t>> available_xids_t;

	available_xids_t available_xids;
};

LIBCXXW_NAMESPACE_END

#endif
