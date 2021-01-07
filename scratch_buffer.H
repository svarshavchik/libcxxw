/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_scratch_buffer_h
#define x_w_impl_scratch_buffer_h

#include <x/w/scratch_buffer.H>
#include <x/w/pictformatfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/gc.H>
#include <x/mpobj.H>
#include <x/functional.H>
#include <optional>

LIBCXXW_NAMESPACE_START

#define SCRATCH_BUFFER_DEBUG 0

/////////////////////////////////////////////////////////////////////////////
//
// "Internal" implementation details.

class scratch_bufferObj::implObj : virtual public obj {

	//! Cached pixmap, and its picture.

	struct cached_picture_info {
		pixmap pm;
		picture pic;
		gc graphic_context;

		cached_picture_info(const pixmap &pm,
				    const picture &pic,
				    const gc &graphic_context)
			: pm(pm), pic(pic), graphic_context(graphic_context) {}
	};

	//! The cached pixmap and picture are mutex-protected.

	typedef mpobj<std::optional<cached_picture_info>> cached_picture_t;

	//! The scratch buffer's pictformat.
	const const_pictformat scratch_pictformat;

#if SCRATCH_BUFFER_DEBUG
	const std::string identifier;
#endif
	//! The scratch buffer's screen
	const screen scratch_screen;

	//! The cached picture.
	cached_picture_t cached_picture;
public:
	//! Constructor
	implObj(const const_pictformat &scratch_pictformat,
		const std::string &identifier,
		const screen &scratch_screen);

	//! Destructor
	~implObj();

	//! Lock the cached pixmap and picture.

	void do_get(dim_t minimum_width,
		    dim_t minimum_height,
		    const function<void (const picture &, const pixmap &,
					 const gc &)> &);
};

LIBCXXW_NAMESPACE_END

#endif
