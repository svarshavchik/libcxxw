/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_scratch_and_mask_buffer_drawfwd_h
#define x_w_impl_scratch_and_mask_buffer_drawfwd_h

#include <x/w/impl/scratch_buffer_drawfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation class for \ref scratch_and_mask_buffer_draw "scratch_and_mask_buffer_draw".

//! \ref scratch_and_mask_buffer_draw "scratch_and_mask_buffer_draw"'s code
//! that does not depend on template parameters, to reduce the template
//! code bloat.

class scratch_and_mask_buffer_draw_impl {

 public:
	//! Area scratch buffer.

	const scratch_buffer mask_scratch;

	//! Constructor

	scratch_and_mask_buffer_draw_impl(const std::string &label,
					  generic_windowObj::handlerObj &h,
					  dim_t estimated_width,
					  dim_t estimated_height)
		LIBCXX_PUBLIC;

	//! Destructor
	~scratch_and_mask_buffer_draw_impl();

	//! Reinvoke do_draw(), after locking area_scratch
	void get_mask_scratch_buffer(ONLY IN_THREAD,
				     const draw_info &di,
				     const picture &area_picture,
				     const pixmap &area_pixmap,
				     const gc &area_gc,
				     const clip_region_set &clipped,
				     const rectangle &area_entire_rect);

	//! Implement do_draw(), with a convenient scratch buffer.
	virtual void do_draw(ONLY IN_THREAD,
			     const draw_info &di,
			     const picture &area_picture,
			     const pixmap &area_pixmap,
			     const gc &area_gc,
			     const picture &mask_picture,
			     const pixmap &mask_pixmap,
			     const gc &mask_gc,
			     const clip_region_set &clipped,
			     const rectangle &area_entire_rect)=0;
};

/*! A mixin for implementing elementObj::implObj::do_draw() using a scratch buffer.

\code
class my_implObj : public INSERT_LIBX_NAMESPACE::w::scratch_and_mask_buffer_draw<INSERT_LIBX_NAMESPACE::w::child_elementObj> {

    typedef INSERT_LIBX_NAMESPACE::w::scratch_and_mask_buffer_draw<INSERT_LIBX_NAMESPACE::w::child_elementObj> superclass_t;

};

my_implObj::my_implObj(// ...
                       ) : superclass_t{"mask@example.com",
                                        50, 50,
                                        // ...
                                        }
{
   // ...
}

void my_implObj::do_draw(ONLY IN_THREAD,
                         const INSERT_LIBX_NAMESPACE::w::draw_info &di,
                         const INSERT_LIBX_NAMESPACE::w::picture &area_picture,
                         const INSERT_LIBX_NAMESPACE::w::pixmap &area_pixmap,
                         const INSERT_LIBX_NAMESPACE::w::gc &area_gc,
                         const INSERT_LIBX_NAMESPACE::w::picture &mask_picture,
                         const INSERT_LIBX_NAMESPACE::w::pixmap &mask_pixmap,
                         const INSERT_LIBX_NAMESPACE::w::gc &mask_gc,
                         const INSERT_LIBX_NAMESPACE::w::clip_region_set &clipped,
                         const INSERT_LIBX_NAMESPACE::w::rectangle &area_entire_rect)
{
    // ...
}
\endcode

This template subclasses \ref scratch_buffer_draw "scratch_buffer_draw",
and adds a second scratch buffer, which is a 1-bit scratch buffer suitable
for masking purposes when compositing the contents of the display element.
The first parameter is the label for the scratch buffer. All display elements
that specify the same label will reuse the same scratch buffer.
The next two parameters is the initial estimated size of the scratch buffer.
Optimally, the initial size would match the size of the display element when
it becomes visible, but if not the mask scratch buffer gets sized accordingly.

The mask picture scratch buffer label, and the estimated size parameters,
get used to initialize the scratch buffer. The remaining parameters,
get forwarded to
the \c scratch_buffer_draw superclass.

The template implements \c scratch_buffer_draw's do_draw(), locks the mask
scratch buffer, enlarging it to
the size of the drawn element, and reinvokes the reimplementation
of do_draw(), passing the following parameters:

- the draw_info object.

- the scratch buffer's picture, pixmap, and graphic context. The scratch
buffer may be larger than area_entire_rect's size. When do_draw() returns
the display element's new contents come from the scratch buffer's rectangle
anchored at the scratch buffer's top-left corner, (0, 0).

- a \c clip_region_set handle, which proves, by contract, that all drawing
operations will eventually be clipped to the display element's current
position.

- a rectangle for the entire display element. area_entire_rect's x and y
position will always be 0, and its width and height indicates the current
size of the display element.

*/

template<typename child_type>
class scratch_and_mask_buffer_draw : public scratch_buffer_draw<child_type>,
	scratch_and_mask_buffer_draw_impl {

 public:

	template<typename name1_type, typename ...Args>
		scratch_and_mask_buffer_draw(name1_type &&mask_picture_name,
					     dim_t estimated_width,
					     dim_t estimated_height,
					     Args && ...args);

	~scratch_and_mask_buffer_draw();

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const picture &area_picture,
		     const pixmap &area_pixmap,
		     const gc &mask_gc,
		     const clip_region_set &clipped,
		     const rectangle &area_entire_rect) override;
};


LIBCXXW_NAMESPACE_END

#endif
