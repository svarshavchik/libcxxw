/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_borderlayoutmanagerfwd_h
#define x_w_borderlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/border_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/text_param.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC new_borderlayoutmanager;
class LIBCXX_PUBLIC borderlayoutmanagerObj;

/*! Draw a border around an element.

A subclass of the \ref singletonlayoutmanager "singleton layout manager"
that draws a border around the widget.

The same effect can be obtained by using the
\ref gridlayoutmanager "grid layout manager", but this is more efficient.

Passing a \ref new_borderlayoutmanager "new_borderlayoutmanager" to
a \ref factory "factory"'s create_container() creates a container
that uses the border layout manager.

\code
f->update_title("New title");

f->update_title(IN_THREAD, "New title");
\endcode

update_title() replaces the title shown in the upper-left corner.
An empty string removes the title. update_title()'s parameter is
an \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param".

\code
f->update_border("thin");
f->update_borders("thin", "thin", "thick", "thick")

f->update_border(IN_THREAD, "thin");
f->update_borders(IN_THREAD, "thin", "thin", "thick", "thick")
\endcode

update_border() changes the shown border. update_borders() individually
specified the left, right, top, and bottom
\ref border_arg "borders".

\code
INSERT_LIBX_NAMESPACE::w::const_uigenerators generators;
INSERT_LIBX_NAMESPACE::w::uielements elements;

f->generate("options", generators, elements);
\endcode

generate() \ref uigenerators "generates the contents"
of the border layout manager's container from a theme file

*/

typedef ref<borderlayoutmanagerObj> borderlayoutmanager;

//! A constant \ref borderlayoutmanager "border layout manager".

//! \see borderlayoutmanager

typedef const_ref<borderlayoutmanagerObj> const_borderlayoutmanager;

//! A nullable pointer reference to a \ref borderlayoutmanager "border layout manager".

//! \see borderlayoutmanager

typedef ptr<borderlayoutmanagerObj> borderlayoutmanagerptr;

//! A nullable pointer reference to a const \ref borderlayoutmanager "border layout manager".

//! \see borderlayoutmanager

typedef const_ptr<borderlayoutmanagerObj> const_borderlayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
