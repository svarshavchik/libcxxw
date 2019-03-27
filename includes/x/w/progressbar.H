/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_progressbar_h
#define x_w_progressbar_h

#include <x/w/progressbarfwd.H>
#include <x/w/progressbarobj.H>
#include <x/w/progressbar_appearancefwd.H>
#include <x/ref.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Progress bar configuration

//! \see progressbar

struct progressbar_config {

	//! The appearance of the new progress bar

	const_progressbar_appearance appearance;

	//! Default constructor

	progressbar_config();

	//! Destructor

	~progressbar_config();

	//! Copy constructor

	progressbar_config(const progressbar_config &);

	//! Assignment operator

	progressbar_config &operator=(const progressbar_config &);
};

LIBCXXW_NAMESPACE_END

#endif
