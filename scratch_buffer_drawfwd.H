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
	//! Constructor

	scratch_buffer_draw_impl();

	//! Destructor
	~scratch_buffer_draw_impl();

	//! Reinvoke do_draw(), after locking area_scratch
	void get_scratch_buffer(IN_THREAD_ONLY,
				elementObj::implObj &element,
				const draw_info &di);

	//! Implement do_draw(), with a convenient scratch buffer.
	virtual void do_draw(IN_THREAD_ONLY,
			     const draw_info &di,
			     const picture &area_picture,
			     const pixmap &area_pixmap,
			     const gc &area_gc,
			     const rectangle &area_entire_rect)=0;
};

//! A mixin for implementing do_draw() using a scratch buffer.

//! The template class must be derived from elementObj::implObj, and this
//! template derives from it.
//!
//! This template implements do_draw(), inherited from elementObj::implObj,
//! by using the element_scratch_buffer for the entire element, at once.
//! Simple display elements that draw their entire contents, at once,
//! inherit from this template.
//!
//! The template implements and overrides elementObj::implObj::do_draw().
//! do_draw() obtains and locks the element_scratch_buffer, enlarging it to
//! the size of the drawn element, sets the clip region for the display
//! element, and invokes draw_using_scratch_buffer(), specifying the entire
//! area of the display element to be drawn, invoking the callback to
//! actually draw it.

template<typename child_type>
class LIBCXX_HIDDEN scratch_buffer_draw : public child_type,
					  public scratch_buffer_draw_impl {

public:

	using child_type::child_type;

	//! Destructor
	~scratch_buffer_draw();

	//! Implement element's do_draw().

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas) override;

};


LIBCXXW_NAMESPACE_END

#endif
