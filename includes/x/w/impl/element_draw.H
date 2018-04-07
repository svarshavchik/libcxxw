/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_element_draw_h
#define x_w_impl_element_draw_h

#include <x/w/connection_threadfwd.H>
#include <x/w/picturefwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/gcfwd.H>
#include <x/w/rectanglefwd.H>
#include <x/w/scratch_bufferfwd.H>
#include <x/w/generic_windowobj.H>
#include <x/w/impl/draw_infofwd.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Clip drawing operation to draw_info.element_viewport.

//! This installs a clip mask into di.window_picture that clips
//! any drawinfo operation (to the di.absolute_location region)
//! into the element_viewport.
//!
//! A reference to this object gets passed to elementObj::implObj's
//! drawing methods in order to prove, by contract, that the clipping
//! region has been set.

class clip_region_set {

public:
	//! Constructor - install clip mask for drawing.

	clip_region_set(ONLY IN_THREAD,
			generic_windowObj::handlerObj &,
			const draw_info &di);

	clip_region_set(const clip_region_set &)=delete;

	clip_region_set &operator=(const clip_region_set &)=delete;

	//! For drawing as disabled.

	//! If set, draw_using_scratch_buffer draws as disabled, even if the
	//! element is not disabled.
	bool draw_as_disabled=false;
};

//! Scratch buffer-based element draw interface.

//! Defines the draw_using_scratch_buffer() interface. elementObj::implObj
//! inherits from this and implements the virtual functions. Modules that
//! handle drawing a display element's contents can include only this
//! header file, and avoid pulling in all of elementObj::implObj's
//! dependencies.

class element_drawObj : virtual public obj {

public:

	//! Return my window

	virtual generic_windowObj::handlerObj &get_window_handler()=0;

	//! Return my window
	virtual const generic_windowObj::handlerObj &get_window_handler() const=0;

	//! Signature of draw_using_scratch_buffer()'s callback.
	typedef void scratch_buffer_draw_func_t(const picture &,
						const pixmap &,
						const gc &);

	//! Draw into this display element using a scratch buffer.

	//! Subclass implements do_draw() using draw_using_scratch_buffer(),
	//! which allocates a scratch buffer, clears it to the element's
	//! background color, then invokes the callback.
	//!
	//! When the callback returns, the contents of the scratch buffer
	//! get copied into the actual window using draw_to_window_picture().
	//!
	//! The first parameter is a callback.
	//!
	//! The second parameter is a rectangle that specifies which portion
	//! of the display element is going to get drawn. The scratch buffer
	//! is made sure to be at least as big as the rectangle.
	//!
	//! The remaining parameters are what get_draw_info() returned for
	//! this element, what get_draw_info() returned for the element whose
	//! background color gets used to clear the initial contents of the
	//! scratch buffer, an instantiated clip_region, and finally the
	//! scratch buffer to be used.
	//!
	//! The scratch buffer parameter is optional, if not specified
	//! it defaults to our element_scratch_buffer.
	//!
	//! Typically, both draw_info parameter are the same, this element's
	//! background color. The grid layout manager, however, uses the
	//! child elements' background color for clearing the padding it
	//! adds around its child elements.

	template<typename lambda_type,
		 typename ...Args>
	void draw_using_scratch_buffer(ONLY IN_THREAD,
				       lambda_type &&lambda,
				       const rectangle &rect,
				       const draw_info &di,
				       const draw_info &background_color_di,
				       const clip_region_set &clipped,
				       Args && ...args)
	{
		do_draw_using_scratch_buffer(IN_THREAD,
					     make_function
					     <scratch_buffer_draw_func_t>
					     (std::forward<lambda_type>
					      (lambda)),
					     rect,
					     di,
					     background_color_di,
					     clipped,
					     std::forward<Args>(args)...);
	}

	//! Implementation of draw_using_scratch_buffer() with element_scratch_uffer.
	virtual void do_draw_using_scratch_buffer
	(ONLY IN_THREAD,
	 const function<scratch_buffer_draw_func_t> &cb,
	 const rectangle &rect,
	 const draw_info &di,
	 const draw_info &background_color_di,
	 const clip_region_set &clipped)=0;

	//! Implementation of draw_using_scratch_buffer() with a custom scratch buffer.
	virtual void do_draw_using_scratch_buffer
	(ONLY IN_THREAD,
	 const function<scratch_buffer_draw_func_t> &cb,
	 const rectangle &rect,
	 const draw_info &di,
	 const draw_info &background_color_di,
	 const clip_region_set &clipped,
	 const scratch_buffer &buffer)=0;
};

LIBCXXW_NAMESPACE_END

#endif
