/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_toolboxlayoutmanagerfwd_h
#define x_w_toolboxlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC toolboxlayoutmanagerObj;

class LIBCXX_PUBLIC new_toolboxlayoutmanager;

class LIBCXX_HIDDEN toolbox_info;

typedef mpobj<toolbox_info, std::recursive_mutex> toolbox_info_t;

/*! The toolbox layout manager

The toolbox layout manager arranges its elements in a row up to its
container's width, then continues placing its elements on the next row, and
so on.

It's expected that all elements should have the same metrics, and the
toolbox layout manager sizes all elements to the largest width and height.

\code
#include <INSERT_LIBX_NAMESPACE/w/toolboxfactory.H>

INSERT_LIBX_NAMESPACE::w::new_toolboxlayoutmanager ntlm;

f->create_container([]
                    (const INSERT_LIBX_NAMESPACE::w::container &c)
                    {
                        INSERT_LIBX_NAMESPACE::w::toolboxlayoutmanager tlm=
                               c->get_layoutmanager();

                        INSERT_LIBX_NAMESPACE::w::toolboxfactory f=tlm->append_tools();
                    },
                    ntlm);

// ...

INSERT_LIBX_NAMESPACE::w::toolboxfactory f=tlm->insert_tools(2);

tlm->remove_tool(0);
tlm->remove_tools(0, 2);

size_t n=tlm->size();
\endcode

Passing a \ref new_toolboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_toolboxlayoutmanager"
to \ref factory "factory"'s create_container() creates a new container that
uses the toolbox layout manager.

append_tools() returns a \ref toolboxfactory "factory" that adds new tools
to the end of the toolbox container. insert_tools() inserts new tools before
an existing tool in the container. remove_tool() removes one tool,
remove_tools() removes consecutive number of tools.

Tools are number sequentially, starting at 0. Adding and removing tools
automatically renumbers them, accordingly.

*/

typedef ref<toolboxlayoutmanagerObj> toolboxlayoutmanager;

//! A constant \ref toolboxlayoutmanager "toolboxlayoutmanager".

//! \see toolboxlayoutmanager

typedef const_ref<toolboxlayoutmanagerObj> const_toolboxlayoutmanager;

//! A nullable pointer reference to a \ref toolboxlayoutmanager "toolboxlayoutmanager".

//! \see toolboxlayoutmanager

typedef ptr<toolboxlayoutmanagerObj> toolboxlayoutmanagerptr;

//! A nullable pointer reference to a const \ref toolboxlayoutmanager "toolboxlayoutmanager".

//! \see toolboxlayoutmanager

typedef const_ptr<toolboxlayoutmanagerObj> const_toolboxlayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
