/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scratch_and_mask_buffer_drawfwd_h
#define scratch_and_mask_buffer_drawfwd_h

#include "scratch_buffer_drawfwd.H"

LIBCXXW_NAMESPACE_START


//! Implementation class for \ref scratch_and_mask_buffer_draw "scratch_and_mask_buffer_draw".

//! \ref scratch_and_mask_buffer_draw "scratch_and_mask_buffer_draw"'s code
//! that does not depend on template parameters, to reduce the template
//! code bloat.

class LIBCXX_HIDDEN scratch_and_mask_buffer_draw_impl {

 public:
	//! Area scratch buffer.

	const scratch_buffer mask_scratch;

	//! Constructor

	scratch_and_mask_buffer_draw_impl(const std::string &label,
					  generic_windowObj::handlerObj &h,
					  dim_t estimated_width,
					  dim_t estimated_height);

	//! Destructor
	~scratch_and_mask_buffer_draw_impl();

	//! Reinvoke do_draw(), after locking area_scratch
	void get_mask_scratch_buffer(IN_THREAD_ONLY,
				     const draw_info &di,
				     const picture &area_picture,
				     const pixmap &area_pixmap,
				     const gc &area_gc,
				     const rectangle &area_entire_rect);

	//! Implement do_draw(), with a convenient scratch buffer.
	virtual void do_draw(IN_THREAD_ONLY,
			     const draw_info &di,
			     const picture &area_picture,
			     const pixmap &area_pixmap,
			     const gc &area_gc,
			     const picture &mask_picture,
			     const pixmap &mask_pixmap,
			     const gc &mask_gc,
			     const rectangle &area_entire_rect)=0;
};

//! A mixin for implementing elementObj::implObj::do_draw() using a scratch buffer.

//! This template subclasses \ref scratch_buffer_draw "scratch_buffer_draw",
//! and adds a second scratch
//! buffer. The template declares a \c mask_scratch class member which is a
//! one bit
//! \ref scratch_buffer "scratch_buffer", usable for masking purposes.
//! The first parameter is the area picture scratch buffer label. The second
//! parameter is the mask picture scratch buffer label. The next two parameters
//! are the initial estimated size of both scratch buffers.
//!
//! The mask picture scratch buffer label, and the estimated size parameters,
//! get used to initialize the scratch buffer. The parameters, except for the
//! mask picture scratch buffer label, get forwarded to
//! the \c scratch_buffer_draw superclass.
//!
//! The template implements \c scratch_buffer_draw's do_draw(), locks the mask
//! scratch buffer, enlarging it to
//! the size of the drawn element, and reinvokes the reimplementation
//! of do_draw(), forwarding the original parameters, and adding three more:
//! the mask scratch picture, pixmap, and graphic context.

template<typename child_type>
class LIBCXX_HIDDEN scratch_and_mask_buffer_draw
	: public scratch_buffer_draw<child_type>,
	scratch_and_mask_buffer_draw_impl {

 public:

	template<typename name1_type, typename ...Args>
		scratch_and_mask_buffer_draw(name1_type &&mask_picture_name,
					     dim_t estimated_width,
					     dim_t estimated_height,
					     Args && ...args);

	~scratch_and_mask_buffer_draw();

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const picture &area_picture,
		     const pixmap &area_pixmap,
		     const gc &mask_gc,
		     const rectangle &area_entire_rect) override;
};


LIBCXXW_NAMESPACE_END

#endif
