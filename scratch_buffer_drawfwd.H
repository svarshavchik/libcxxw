/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scratch_buffer_drawfwd_h
#define scratch_buffer_drawfwd_h

#include "x/w/namespace.H"
#include "x/w/picturefwd.H"
#include "x/w/pixmapfwd.H"
#include "x/w/gcfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "x/w/rectangle.H"
#include "x/w/generic_windowobj.H"
#include "draw_infofwd.H"
#include "connection_threadfwd.H"

#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Implementation class for \ref scratch_buffer_draw "scratch_buffer_draw".

//! \ref scratch_buffer_draw "scratch_buffer_draw"'s code that does not
//! depend on template parameters, to reduce the template code bloat.

class LIBCXX_HIDDEN scratch_buffer_draw_impl {

 public:
	//! Area scratch buffer.

	const scratch_buffer area_scratch;

	//! Constructor

	scratch_buffer_draw_impl(const std::string &label,
				 generic_windowObj::handlerObj &h,
				 dim_t estimated_width,
				 dim_t estimated_height);

	//! Destructor
	~scratch_buffer_draw_impl();

	//! Reinvoke do_draw(), after locking area_scratch
	void get_scratch_buffer(IN_THREAD_ONLY,
				elementObj::implObj &element,
				const draw_info &di,
				const rectangle_set &areas);

	//! Implement do_draw(), with a convenient scratch buffer.
	virtual void do_draw(IN_THREAD_ONLY,
			     const draw_info &di,
			     const picture &area_picture,
			     const pixmap &area_pixmap,
			     const gc &area_gc,
			     const rectangle &area_entire_rect)=0;
};

//! A mixin for implementing elementObj::implObj::do_draw() using a scratch buffer.

//! The template class must be derived from elementObj::implObj, and the
//! template derives from it.
//!
//! This template declares an \c area_scratch class member, which is a
//! \ref scratch_buffer "scratch_buffer". The first three parameters to the
//! constructor are forwarded to the scratch_buffer's constructor: the
//! scratch buffer's label, and its initial estimated width and height,
//! together with the element's window handler's pictformat.
//!
//! The template implements and overrides elementObj::implObj::do_draw().
//! The do_draw() obtains and locks the scratch buffer, enlarging it to
//! the size of the drawn element, and reinvokes the reimplementation
//! of do_draw() that takes the following parameters: the forwarded
//! \ref draw_info "draw_info" parameter, the scratch picture, pixmap,
//! and graphic context, and the rectangle whose size is the element's
//! current_position's size, i.e. the size of the area_picture to draw into.

template<typename child_type>
class LIBCXX_HIDDEN scratch_buffer_draw : public child_type,
					  public scratch_buffer_draw_impl {

public:

	template<typename name_type, typename ...Args>
		scratch_buffer_draw(name_type &&name,
				    dim_t estimated_width,
				    dim_t estimated_height,
				    Args && ...args);

	~scratch_buffer_draw();

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas) override;

};


LIBCXXW_NAMESPACE_END

#endif
