/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_appearancefwd_h
#define x_w_tooltip_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC tooltip_appearanceObj;
class tooltip_appearance_base;

/*! Appearance of a new \ref tooltip_factory "tooltip_factory".

Specifies various properties of a tooltip. The default properties are
set by the current display theme.

\code
element->create_custom_tooltip
    ([]
     (ONLY IN_THREAD,
      const INSERT_LIBX_NAMESPACE::w::tooltip_factory &f)
     {

         INSERT_LIBX_NAMESPACE::w::const_tooltip_appearance custom=INSERT_LIBX_NAMESPACE::w::tooltip_appearance::base::tooltip_theme()
             ->modify([]
                      (const INSERT_LIBX_NAMESPACE::w::tooltip_appearance &custom)
                      {
                            // Modify custom->...
                      });

          f.appearance=custom;

          //
      });


INSERT_LIBX_NAMESPACE::w::static_tooltip_config stc;

stc.appearance=std.appearance->modify
    ([]
     (const INSERT_LIBX_NAMESPACE::w::tooltip_appearance &custom)
     {
          // Modify custom->...
     });
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::tooltip_appearance::base::tooltip_theme() returns
a cached default theme appearance for a popup tooltip.
\c INSERT_LIBX_NAMESPACE::w::tooltip_appearance::base::static_tooltip_theme()
returns a cached default theme appearance for a static tooltip.
\c INSERT_LIBX_NAMESPACE::w::tooltip_appearance::base::direction_tooltip_theme()
returns a cached default theme appearance for an input field's text direction
tooltip
*/

typedef ref<tooltip_appearanceObj,
	    tooltip_appearance_base> tooltip_appearance;

//! Constant \ref tooltip_appearance "tooltip appearance".

//! \see tooltip_appearance

typedef const_ref<tooltip_appearanceObj,
		  tooltip_appearance_base> const_tooltip_appearance;

//! A nullable pointer reference to a \ref tooltip_appearance "tooltip appearance".

//! \see tooltip_appearance

typedef ptr<tooltip_appearanceObj,
	    tooltip_appearance_base> tooltip_appearanceptr;

//! A nullable pointer reference to a const \ref tooltip_appearance "tooltip appearance".

//! \see tooltip_appearance

typedef const_ptr<tooltip_appearanceObj,
		  tooltip_appearance_base> const_tooltip_appearanceptr;

//! Alias

typedef appearance_wrapper<const_tooltip_appearance> const_tooltip_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
