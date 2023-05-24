/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholelayoutmanagerfwd_h
#define x_w_peepholelayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC new_peepholelayoutmanager;
class LIBCXX_PUBLIC new_scrollable_peepholelayoutmanager;
class LIBCXX_PUBLIC peepholelayoutmanagerObj;

/*! The peephole layout manager

This layout manager puts another widget inside a (usually)
smaller area, and scrolls parts of it into view, as needed.

\see new_scrollable_peepholelayoutmanager

\see new_peepholelayoutmanager

\code
INSERT_LIBX_NAMESPACE::w::new_peepholelayoutmanager nplm{
    [&]
    (const INSERT_LIBX_NAMESPACE::w::factory &f)
    {
        f->create_container(
            [&]
            (const INSERT_LIBX_NAMESPACE::w::container &c)
            {
                INSERT_LIBX_NAMESPACE::w::gridlayoutmanager glm=c->get_layoutmanager();
                // ...
            },
            INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager{});
     }
};

npl.width({20, 50, 100});
npl.height({20, 50, 100});

f->create_focusable_container(
    []
    (const auto &c)
    {
           // Unused
    });
\endcode

width() and height() sets the peephole's size, given as
\ref dim_axis_arg "dim_axis_arg"'s.
new_scrollable_peepholelayoutmanager/new_peepholelayoutmanager's callback
creates the peepholed widget, and gets scrolled within the peephole
if its size exceeds the peephole's.

create_container() and create_focusable_container()'s creators are not used
because the callback creates the peepholed element; but the creators can
tweak some generic container-specific settings.

Not setting the width or the height makes the peephole's size, in that
dimension, always match the peepholed element's. This results in a peephole
that typically scrolls only the other dimension.

*/

typedef ref<peepholelayoutmanagerObj> peepholelayoutmanager;

//! A constant \ref peepholelayoutmanager "peephole layout manager".

//! \see peepholelayoutmanager

typedef const_ref<peepholelayoutmanagerObj> const_peepholelayoutmanager;

//! A nullable pointer reference to a \ref peepholelayoutmanager "peephole layout manager".

//! \see peepholelayoutmanager

typedef ptr<peepholelayoutmanagerObj> peepholelayoutmanagerptr;

//! A nullable pointer reference to a const \ref peepholelayoutmanager "peephole layout manager".

//! \see peepholelayoutmanager

typedef const_ptr<peepholelayoutmanagerObj> const_peepholelayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
