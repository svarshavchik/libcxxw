/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_window_appearancefwd_h
#define x_w_generic_window_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class generic_window_appearanceObj;
class generic_window_appearance_base;

/*! Common appearance properties of a top level window.

Specifies various common properties of top level windows like
\ref main_window "main windows", and popup windows like the
\ref standard_comboboxlayoutmanager "combo-boxes". A
generic window appearance object is typically attached to another
appearance object related to the relevant display element.

\code

INSERT_LIBX_NAMESPACE::w::const_generic_window_appearance custom=INSERT_LIBX_NAMESPACE::w::generic_window_appearance::base::main_window_theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::generic_window_appearance &custom)
             {
                    // Modify custom->...
             });

INSERT_LIBX_NAMESPACE::w::main_window_config config;

config.appearance=config.appearance
     ->modify([&]
              (const auto &main_window_appearance)
              {
                  main_window_appearance->toplevel_appearance=custom;
              });

auto main_window=INSERT_LIBX_NAMESPACE::w::main_window::create
    (config,
     []
     (const auto &main_window)
     {
     });
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::generic_window_appearance::base::main_window_theme()
returns a cached default theme appearance. Other cached objects include a
date_input_field_theme(), list_contents_theme(), combobox_theme(),
and tooltip_theme().


*/

typedef ref<generic_window_appearanceObj,
	    generic_window_appearance_base> generic_window_appearance;

//! Constant \ref generic_window_appearance "WHATITIS".

//! \see generic_window_appearance

typedef const_ref<generic_window_appearanceObj,
		  generic_window_appearance_base> const_generic_window_appearance;

//! A nullable pointer reference to a \ref generic_window_appearance "WHATITIS".

//! \see generic_window_appearance

typedef ptr<generic_window_appearanceObj,
	    generic_window_appearance_base> generic_window_appearanceptr;

//! A nullable pointer reference to a const \ref generic_window_appearance "WHATITIS".

//! \see generic_window_appearance

typedef const_ptr<generic_window_appearanceObj,
		  generic_window_appearance_base> const_generic_window_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
