/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_frame_appearancefwd_h
#define x_w_frame_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC frame_appearanceObj;
class frame_appearance_base;

/*! Appearance of the \ref borderlayoutmanager "border layout manager's frame".

Specifies various properties of the frame drawn by the border layout manager.
 The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::new_borderlayoutmanager lm{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_frame_appearance custom=INSERT_LIBX_NAMESPACE::w::frame_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::frame_appearance &custom)
             {
                    // Modify custom->...
             });

lm->appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::frame_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<frame_appearanceObj,
	    frame_appearance_base> frame_appearance;

//! Constant \ref frame_appearance "border frame".

//! \see frame_appearance

typedef const_ref<frame_appearanceObj,
		  frame_appearance_base> const_frame_appearance;

//! A nullable pointer reference to a \ref frame_appearance "border frame".

//! \see frame_appearance

typedef ptr<frame_appearanceObj,
	    frame_appearance_base> frame_appearanceptr;

//! A nullable pointer reference to a const \ref frame_appearance "border frame".

//! \see frame_appearance

typedef const_ptr<frame_appearanceObj,
		  frame_appearance_base> const_frame_appearanceptr;

//! Alias

typedef appearance_wrapper<const_frame_appearance> const_frame_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
