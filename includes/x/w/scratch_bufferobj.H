/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scratch_bufferobj_h
#define x_w_scratch_bufferobj_h

#include <x/w/scratch_bufferfwd.H>
#include "x/w/pixmapfwd.H"
#include "x/w/picturefwd.H"
#include "x/w/gcfwd.H"
#include "x/w/types.H"
#include <x/obj.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Callback passed to scratch_buffer's get().

template<typename F>
concept scratch_buffer_callback=requires(F f,
					 const picture &pc,
					 const pixmap &pm,
					 const gc &g)
{
	{ f(pc, pm, g) };
};

//! A \ref scratch_buffer "shared picture object".

class scratch_bufferObj : virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

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

	template<scratch_buffer_callback functor>
	void get(dim_t minimum_width,
		 dim_t minimum_height,
		 functor &&f)
	{
		do_get(minimum_width,
		       minimum_height,
		       make_function<void (const picture &,
					   const pixmap &,
					   const gc &)>(f));
	}

	//! Lock the cached pixmap and picture.

	//! Internal implementation.
	//!
	//! \see get

	void do_get(dim_t minimum_width,
		    dim_t minimum_height,
		    const function<void (const picture &, const pixmap &,
					 const gc &)> &);
};

LIBCXXW_NAMESPACE_END

#endif
