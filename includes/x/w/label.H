/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_label_h
#define x_w_label_h

#include <x/w/labelfwd.H>
#include <x/w/labelobj.H>
#include <x/w/textlabel.H>
#include <x/w/text_param.H>
#include <x/w/alignment.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Optional custom configuration parameters for a label display element.

struct label_config {

	//! Label alignment
	halign alignment=halign::left;

	//! Label word wrap width (0=don't wrap)
	double widthmm=0;
};

LIBCXXW_NAMESPACE_END

#endif
