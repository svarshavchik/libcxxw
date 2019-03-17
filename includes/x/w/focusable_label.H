/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_label_h
#define x_w_focusable_label_h

#include <x/w/focusable_labelfwd.H>
#include <x/w/focusable_labelobj.H>
#include <x/w/textlabel.H>
#include <x/w/focusable.H>
#include <x/w/alignment.H>
#include <x/w/border_arg.H>
#include <x/w/label.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Focusable label configuration.

//! Optional parameter passed to \ref factory "a factory's"
//! create_focusable_label() method.

struct focusable_label_config : public label_config {

	//! No focus border
	border_arg off_border="inputfocusoff_border";

	//! Focus border
	border_arg on_border="inputfocuson_border";

	//! Destructor
	~focusable_label_config();
};

LIBCXXW_NAMESPACE_END

#endif
