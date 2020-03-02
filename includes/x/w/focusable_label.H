/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_label_h
#define x_w_focusable_label_h

#include <x/w/focusable_labelfwd.H>
#include <x/w/focusable_labelobj.H>
#include <x/w/focus_border_appearance.H>
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

	//! Input focus appearance.

	const_focus_border_appearance focus_border=
		focus_border_appearance::base::theme();

	//! Destructor
	~focusable_label_config();
};

LIBCXXW_NAMESPACE_END

#endif
