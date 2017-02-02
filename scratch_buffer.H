/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scratch_buffer_h
#define scratch_buffer_h

#include "x/w/scratch_buffer.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

/////////////////////////////////////////////////////////////////////////////
//
// "Internal" implementation details.

class scratch_bufferObj::implObj : virtual public obj {

	//! Cached pixmap, and its picture.

	struct cached_picture_info {
		pixmap pm;
		picture pic;
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
};

LIBCXXW_NAMESPACE_END

#endif
