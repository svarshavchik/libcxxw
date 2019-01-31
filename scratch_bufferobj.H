/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scratch_bufferobj_h
#define scratch_bufferobj_h

#include "x/w/scratch_bufferfwd.H"
#include "x/w/pixmapfwd.H"
#include "x/w/picturefwd.H"
#include "x/w/gcfwd.H"
#include "x/w/types.H"
#include <x/obj.H>
#include <x/functional.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! A \ref scratch_buffer "shared picture object".

class scratch_bufferObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor

	scratch_bufferObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor

	~scratch_bufferObj();

	//! Lock the cached pixmap and picture.

	//! Verify that their size is at least the given minimum. If not,
	//! create a larger pixmap+picture.
	//!
	//! Invoke the callback, passing the pixmap and the picture to the
	//! callback.
	//!
	//! The shared pixmap and picture remain locked until the callback
	//! returns.

	template<typename functor>
	void get(dim_t minimum_width,
		 dim_t minimum_height,
		 functor &&f)
	{
		do_get(minimum_width,
		       minimum_height,
		       make_function<void (const picture &,
					   const pixmap &)>(f));
	}

	//! Lock the cached pixmap and picture.

	//! Internal implementation.
	//!
	//! \see get

	void do_get(dim_t minimum_width,
		    dim_t minimum_height,
		    const function<void (const picture &, const pixmap &,
					 const gc & )> &);

	//! Retrieve the cached picture

	//! Sometimes a scratch_buffer is used for read-only cached pictures,
	//! if so we won't ask for a minimum size

	const_picture get_picture() const;

	//! Retrieve the cached pixmap

	//! Sometimes a scratch_buffer is used for read-only cached pictures,
	//! if so we won't ask for a minimum size

	const_pixmap get_pixmap() const;
};

/////////////////////////////////////////////////////////////////////////////
//
// "Internal" implementation details.

class scratch_bufferObj::implObj : virtual public obj {

	//! Cached pixmap, and its picture.

	struct cached_picture_info {
		pixmap pm;
		picture pic;
		gc graphic_context;
	};

	//! The cached pixmap and picture are mutex-protected.

	typedef mpobj<cached_picture_info> cached_picture_t;

	//! The cached picture.
	cached_picture_t cached_picture;
public:
	//! Constructor
	implObj(const pixmap &pm);

	//! Destructor
	~implObj();

	//! Lock the cached pixmap and picture.

	void do_get(dim_t minimum_width,
		    dim_t minimum_height,
		    const function<void (const picture &, const pixmap &)> &);

	//! Retrieve the cached picture

	//! Sometimes a scratch_buffer is used for read-only cached pictures,
	//! if so we won't ask for a minimum size

	const_picture get_picture();

	//! Retrieve the cached pixmap

	//! Sometimes a scratch_buffer is used for read-only cached pictures,
	//! if so we won't ask for a minimum size

	const_pixmap get_pixmap();

	//! Retrieve the cached graphic context.

	gc get_gc();
};

LIBCXXW_NAMESPACE_END

#endif
