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

	//! Initial value of the progress bar
	size_t value=0;

	//! Maximum value of the progress bar
	size_t maximum_value=100;

	//! Wrap a \ref const_progressbar_appearance "const_progressbar_appearance".

	//! Avoids the need to pull in the entire header file.

	struct appearance_wrapper : public const_progressbar_appearance {

		using const_progressbar_appearance::const_progressbar_appearance;

		using const_progressbar_appearance::operator->;

		//! Default constructor

		appearance_wrapper();

		//! Destructor

		~appearance_wrapper();

		//! Copy constructor

		appearance_wrapper(const appearance_wrapper &);

		using const_progressbar_appearance::operator=;
	};

	//! The appearance of the new progress bar

	appearance_wrapper appearance;
};

LIBCXXW_NAMESPACE_END

#endif
