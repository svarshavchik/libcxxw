/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pictureobj_h
#define x_w_pictureobj_h

#include <x/w/namespace.H>
#include <x/w/picturefwd.H>
#include <x/w/pictformatfwd.H>
#include <x/w/rectanglefwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/types.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A picture object.

//! \see picture

class pictureObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor

	pictureObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor

	~pictureObj();

	//! Composite RENDER operation

	void composite(const const_picture &src,
		       coord_t src_x,
		       coord_t src_y,
		       coord_t dst_x,
		       coord_t dst_y,
		       dim_t width,
		       dim_t height,
		       render_pict_op op=render_pict_op::op_src);

	//! Composire RENDER operation

	void composite(const const_picture &src,
		       coord_t src_x,
		       coord_t src_y,
		       const rectangle &rect,
		       render_pict_op op=render_pict_op::op_src);

	//! Clip the picture with a pixmap.

	//! The constructor installs the clip mask, the destructor removes it.
	//! Refer to this class as \c LIBCXX_NAMESPACE::w::picture::base::clip_mask

	class clip_mask {
		//! My picture
		const picture clipped_picture;

		//! My clip mask
		const pixmap picture_clip_mask;
	public:
		clip_mask(const picture &clipped_picture,
			  const pixmap &picture_clip_mask,
			  coord_t clip_x_origin,
			  coord_t clip_y_origin);
		~clip_mask();

		clip_mask(const clip_mask &)=delete;
		clip_mask &operator=(const clip_mask &)=delete;
	};

};

LIBCXXW_NAMESPACE_END

#endif
