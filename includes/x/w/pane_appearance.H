/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_appearance_h
#define x_w_pane_appearance_h

#include <x/w/pane_appearancefwd.H>
#include <x/w/pane_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref pane_appearance "pane appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::pane_appearance::base.

class pane_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_pane_appearance theme();

	//! Default theme

	static const_pane_appearance focusable_list();

	//! Default theme

	static const_pane_appearance file_dialog_dir();

	//! Default theme

	static const_pane_appearance file_dialog_file();
};

LIBCXXW_NAMESPACE_END

#endif
