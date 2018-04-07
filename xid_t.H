/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef xid_t_h
#define xid_t_h

#include "x/w/connection_threadfwd.H"
#include "connection_info.H"
#include "xid_t_fwd.H"

LIBCXXW_NAMESPACE_START

// An allocated xid, from xcb_generate_id()
//
// The constructor runs xcb_generate_id() (actually calling alloc_xid(),
// to recycle an available one, if possible) so by the time the subclass's
// constructor runs, the xid is ready for use. The destructor release_xid()s,
// to recycle it.

class LIBCXX_HIDDEN new_xid {

 public:

	// Connection thread
	const connection_thread thread_;

	const uint32_t id_;

	new_xid(const connection_thread &thread_);
	~new_xid();

	new_xid(const new_xid &)=delete;
	new_xid &operator=(const new_xid &)=delete;

	inline auto thread() const;

	inline auto conn() const;
};

//! A reference-counted new_xid.

class LIBCXX_HIDDEN xidObj : virtual public obj, public new_xid {

public:

	using new_xid::new_xid;
};

// Store an XID as a  separate object, xid_obj.
//
// This kind of an arrangement is needed to properly dispose of xcb_window_t
// which has delicate scoping rules. Even after a DestroyWindow() there
// could be some messages from the display server, referencing the
// xcb_window_t. This poses some challenges.

class LIBCXX_HIDDEN xid_object_container {

 public:
	const xid xid_obj;

	//! Regular constructor.

	xid_object_container(const connection_thread &thread)
		: xid_obj(xid::create(thread))
	{
	}

	//! Alias constructor

	xid_object_container(const xid &xid_obj) : xid_obj(xid_obj)
	{
	}

	inline auto thread() const;

	inline auto conn() const;
};

// Implement id() to return a typedef.
//
// This is subclassed by a handler object that owns an xid_t, such as a
// window, which subclasses xid<xcb_window_t>. A graphical context
// subclasses xid_t<xcb_gcontext_t>, and so on.
//
// This doesn't really accomplish a great deal at the moment, since the C
// XCB API aliases all of them to an int.

template<typename type> class LIBCXX_HIDDEN xid_t
	: public xid_object_container {

 public:

	using xid_object_container::xid_object_container;

	inline type id() const
	{
		return xid_obj->id_;
	}

};

#define xid_t_included 1
#include "xid_t_connection_thread.H"

LIBCXXW_NAMESPACE_END

#endif
