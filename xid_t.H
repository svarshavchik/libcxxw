/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef xid_T_H
#define xid_T_H

#include "connection_thread.H"
#include "connection_info.H"

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

// An allocated xid, from xcb_generate_id()
//
// The constructor runs xcb_generate_id() (actually calling alloc_xid(),
// to recycle an available one, if possible) so by the time the subclass's
// constructor runs, the xid is ready for use. The destructor release_xid()s,
// to recycle it.

class LIBCXX_HIDDEN xidObj : virtual public obj {

 public:

	// Connection thread
	const ref<connectionObj::implObj::threadObj> thread_;

	const uint32_t id_;

	xidObj(const ref<connectionObj::implObj::threadObj> &thread_)
		: thread_(thread_), id_(thread_->info->alloc_xid())
	{
	}

	~xidObj() noexcept
	{
		thread_->info->release_xid(id_);
	}
};

// Allocate a specific xid for an object.
//
// This is subclassed by a handler object that owns an xid_t, such as a
// window, which subclasses xid<xcb_window_t>. A graphical context
// subclasses xid_t<xcb_gcontext_t>, and so on.
//
// This kind of an arrangement is needed to properly dispose of xcb_window_t
// which have marginal scoping rules. Even after a DestroyWindow() there
// could be some messages from the display server, referencing the
// xcb_window_t. This poses some challenges.

template<typename type> class LIBCXX_HIDDEN xid_t  {

 public:
	const ref<xidObj> xid_obj;

	xid_t(const ref<connectionObj::implObj::threadObj> &thread_)
		: xid_obj(ref<xidObj>::create(thread_))
	{
	}

	~xid_t() noexcept=default;

	inline type id() const
	{
		return xid_obj->id_;
	}

	inline auto thread() const { return xid_obj->thread_; }

	inline auto conn() const { return xid_obj->thread_->info; }
};

// When xid_t's indirection is not needed.

template<typename type> class LIBCXX_HIDDEN xid_tObj : public xidObj {

public:

	using xidObj::xidObj;

	~xid_tObj() noexcept=default;

	inline auto thread() const { return this ->thread_; }

	inline auto conn() const { return this->thread_->info; }

	inline type id() const { return this->id_; }
};

#if 0
{
	{
#endif
	}
}

#endif
