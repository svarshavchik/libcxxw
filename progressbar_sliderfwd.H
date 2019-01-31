/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef progressbar_sliderfwd_h
#define progressbar_sliderfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN progressbar_sliderObj;

//! The progress bar's slider.

//! The progress bar is the container with a grid layout manager containing
//! a single element, the progressbar slider. The grid layout manager draws
//! the borders around the slider.
//!
//! The slider's background color is what visually draws the progressbar's
//! slider.
//!
//! The slider itself is a container whose layout manager gets passed in as
//! the purported layout manager for the progress bar.

typedef ref<progressbar_sliderObj> progressbar_slider;

//! A constant \ref progressbar_slider "progress bar's slider".

//! \see progressbar_slider

typedef const_ref<progressbar_sliderObj> const_progressbar_slider;

//! A nullable pointer reference to a \ref progressbar_slider "progress bar's slider".

//! \see progressbar_slider

typedef ptr<progressbar_sliderObj> progressbar_sliderptr;

//! A nullable pointer reference to a const \ref progressbar_slider "progress bar's slider".

//! \see progressbar_slider

typedef const_ptr<progressbar_sliderObj> const_progressbar_sliderptr;

LIBCXXW_NAMESPACE_END

#endif
