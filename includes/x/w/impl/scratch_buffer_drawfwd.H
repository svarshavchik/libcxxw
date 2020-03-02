/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_scratch_buffer_drawfwd_h
#define x_w_impl_scratch_buffer_drawfwd_h

#include <x/w/namespace.H>
#include <x/w/picturefwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/gcfwd.H>
#include <x/w/scratch_bufferfwd.H>
#include <x/w/rectangle.H>
#include <x/w/generic_windowobj.H>
#include <x/w/impl/draw_infofwd.H>
#include <x/w/connection_threadfwd.H>

#include <x/ref.H>

LIBCXXW_NAMESPACE_START

class clip_region_set;

//! Implementation class for \ref scratch_buffer_draw "scratch_buffer_draw".

//! \ref scratch_buffer_draw "scratch_buffer_draw"'s code that does not
//! depend on template parameters, to reduce the template code bloat.

class scratch_buffer_draw_impl {

 public:
	//! Constructor

	scratch_buffer_draw_impl();

	//! Destructor
	~scratch_buffer_draw_impl();

	//! Reinvoke do_draw(), after locking area_scratch
	void get_scratch_buffer(ONLY IN_THREAD,
				elementObj::implObj &element,
				const draw_info &di);

	//! Implement do_draw(), with a convenient scratch buffer.
	virtual void do_draw(ONLY IN_THREAD,
			     const draw_info &di,
			     const picture &area_picture,
			     const pixmap &area_pixmap,
			     const gc &area_gc,
			     const clip_region_set &clipped,
			     const rectangle &area_entire_rect)=0;
};

/*! A default implementation of elementObj::implObj::do_draw().

\code
class my_implObj : public INSERT_LIBX_NAMESPACE::w::scratch_buffer_draw<INSERT_LIBX_NAMESPACE::w::child_elementObj> {

void my_implObj::do_draw(ONLY IN_THREAD,
                         const INSERT_LIBX_NAMESPACE::w::draw_info &di,
                         const INSERT_LIBX_NAMESPACE::w::picture &area_picture,
                         const INSERT_LIBX_NAMESPACE::w::pixmap &area_pixmap,
                         const INSERT_LIBX_NAMESPACE::w::gc &area_gc,
                         const INSERT_LIBX_NAMESPACE::w::clip_region_set &clipped,
                         const INSERT_LIBX_NAMESPACE::w::rectangle &area_entire_rect)
{
    // ...
}
\endcode

The template class must be derived from elementObj::implObj, and this
template derives from it (the template parameter).

This template implements do_draw(), inherited from elementObj::implObj.
Implementing do_draw() directly provides for maximum flexibility, redrawing
only the parts of the widget that need redrawing.

But most simple widgets don't need this flexibility, and it's simpler
to simply redraw the entire widget, every time, using the element's
default scratch buffer.

The template implements and overrides elementObj::implObj::do_draw();
and defines its own do_draw(), that the subclass implements instead. The
implemented
do_draw() obtains and locks the element_scratch_buffer, enlarging it to
the size of the drawn element, sets the clip region for the widget, invokes draw_using_scratch_buffer(), specifying the entire
area of the widget to be drawn, and then invokes a modified
do_draw(), passing in the scratch buffer's picture, pixmap, and graphic
context; and a rectangle that specifies the element's current size.

The passed-in scratch buffer is at least the size of the element's current
size, but may be larger. All widgets with the same scratch buffer
id share the same scratch buffer. The scratch buffer gets automatically
replaced with a larger one if the current scratch buffer is not big enough
for the drawn element's current size.

Before invoking do_draw(), the scratch buffer gets cleared with the element's
background color.

*/

template<typename child_type>
class scratch_buffer_draw : public child_type,
			    public scratch_buffer_draw_impl {

public:

	using child_type::child_type;

	//! Destructor
	~scratch_buffer_draw();

	//! Implement element's do_draw().

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas) override;

};


LIBCXXW_NAMESPACE_END

#endif
