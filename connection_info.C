/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "messages.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

//! Internal helper class used during construction.

class LIBCXX_INTERNAL connection_infoObj::connection_handle {
 public:
	xcb_connection_t * conn;

	int default_screen;

	connection_handle(const std::string &display)
	{
		conn=xcb_connect(display.size() == 0 ?
				 nullptr:display.c_str(), &default_screen);
	}

	~connection_handle()=default;
};

connection_infoObj::connection_infoObj(const std::experimental::string_view &display)
	: connection_infoObj(connection_handle(std::string(display.begin(),
							   display.end())))
{
}

connection_infoObj::connection_infoObj(connection_handle &&handle)
	: conn(handle.conn), default_screen(handle.default_screen),
	  atoms_info(handle.conn)
{
	if (xcb_connection_has_error(conn))
	{
		xcb_disconnect(conn);
		throw EXCEPTION(_("Display connection failed."));
	}

	if (!xcb_render_util_query_version(conn))
	{
		xcb_disconnect(conn);
		throw EXCEPTION(_("The display server does not support the X render extension"));
	}
}

connection_infoObj::~connection_infoObj()
{
	xcb_render_util_disconnect(conn);
	xcb_disconnect(conn);
}

uint32_t connection_infoObj::alloc_xid()
{
	{
		available_xids_t::lock lock(available_xids);

		if (!lock->empty())
		{
			uint32_t xid=lock->front();
			lock->pop();
			return xid;
		}
	}

	uint32_t id=xcb_generate_id(conn);

	if (id == (uint32_t)-1)
		throw EXCEPTION("xcb_generate_id() failed");

	return id;
}

void connection_infoObj::release_xid(uint32_t xid) noexcept
{
	available_xids_t::lock lock(available_xids);

	lock->push(xid);
}

////////////////////////////////////////////////////////////////////////////
//
// Fetch properties.

void connection_infoObj
::get_property(xcb_window_t window_id,
	       xcb_atom_t property,
	       xcb_atom_t type,
	       uint32_t offset,
	       uint32_t length,
	       bool deleteflag,
	       const function<get_property_response> &response) const
{
	returned_pointer<xcb_generic_error_t *> error;

	auto value=return_pointer(xcb_get_property_reply
				  (conn,
				   xcb_get_property(conn,
						    deleteflag,
						    window_id,
						    property,
						    type,
						    offset,
						    length),
				   error.addressof()));

	if (error)
		throw EXCEPTION(connectionObj::implObj::get_error(error));

	response(value->type, value->format, value->bytes_after,
		 xcb_get_property_value(value),
		 xcb_get_property_value_length(value));
}

size_t connection_infoObj
::get_entire_property(xcb_window_t window_id,
		      xcb_atom_t property,
		      xcb_atom_t type,
		      bool deleteflag,
		      const function<get_entire_property_response> &response)
	const
{
	size_t bytes=0;
	uint32_t offset=0;
	bool bytes_after;

	do
	{
		get_property_with(window_id, property, type,
				  offset/4,
				  16384,
				  deleteflag,
				  [&]
				  (xcb_atom_t type,
				   uint8_t format,
				   uint32_t bytes_afterArg,
				   void *data,
				   size_t data_size)
				  {
					  bytes += data_size;
					  offset += data_size / 4;

					  if (data_size)
						  response(type, format,
							   data, data_size);
					  bytes_after=bytes_afterArg
						  != 0;
				  });
	} while (bytes_after);

	return bytes;;
}

void connection_infoObj
::collect_property(xcb_window_t window_id,
		   xcb_atom_t property,
		   xcb_atom_t type,
		   bool deleteflag,
		   const function<get_entire_property_response> &response)
	const
{
	std::vector<uint8_t> buffer;

	xcb_atom_t type_ret=0;
	xcb_atom_t format_ret=0;
	bool received=false;

	get_entire_property_with(window_id,
				 property,
				 type,
				 deleteflag,
				 [&]
				 (xcb_atom_t typeArg,
				  uint8_t formatArg,
				  void *data,
				  size_t data_size)
				 {
					 type_ret=typeArg;
					 format_ret=formatArg;
					 received=true;

					 auto p=reinterpret_cast<uint8_t *>
						 (data);

					 buffer.insert(buffer.end(),
						       p, p+data_size);
				 });

	if (received)
		response(type_ret,
			 format_ret,
			 reinterpret_cast<void *>(buffer.size() ?
						  &*buffer.begin():
						  nullptr),
			 buffer.size());
}

std::string connection_infoObj::get_atom_name(xcb_atom_t atom) const
{
	returned_pointer<xcb_generic_error_t *> error;

	auto value=return_pointer(xcb_get_atom_name_reply
				  (conn, xcb_get_atom_name(conn, atom),
				   error.addressof()));

	if (error)
		return "(unknown)";

	auto p=xcb_get_atom_name_name(value);

	return std::string(p, p+xcb_get_atom_name_name_length(value));
}

LIBCXXW_NAMESPACE_END
