/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_itemlayoutmanagerfwd_h
#define x_w_itemlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/w/busyfwd.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC itemlayoutmanagerObj;
class LIBCXX_HIDDEN new_itemlayoutmanager;
class LIBCXX_PUBLIC itemlayout_lock;
struct LIBCXX_HIDDEN itemlayout_button_container;

//! Mutex-protected info on the items we are managing.

typedef mpobj<itemlayout_button_container,
	      std::recursive_mutex> item_info_t;

//! Callback that gets invoked whenever the close button gets clicked on an item.

//! \see itemlayoutmanager

typedef functionref<void (THREAD_CALLBACK,
			  size_t,
			  const itemlayout_lock &,
			  const callback_trigger_t &,
			  const busy &)> itemlayout_callback_t;

/*! A layout manager for a container that displays a list of items.

This is often used with an \ref input_field "input_field", and placed above
or below it.

This layout manager adjusts its container to any horizontal size. It expects
its container's parent container to size the item layout manager's container
to a specific horizontal size. Each element in the
container gets drawn as a button, typically with a text label, inside it.
There's an "X" next to it. These elements, items, get placed horizontally
in a row until the row reaches the container's horizontal width, then a
new row gets created for the next set of items.

The item layout manager is usually used with an
\ref input_field "input_field", and the container gets placed above or below
it, and ends up being sized to the same width as the input field.
The input field has an on_validate() callback; the
callback checks and parses the content that gets typed into the text field.
The text gets parsed as one or more "items" of some kind, and cleared from
the input field. Each the item gets added to the item layout container,
reprensented by a single button. Clicking on the "X" next to it invokes
a callback that typically removes the item from the list.

\code
#include <INSERT_LIBX_NAMESPACE/w/itemlayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::itemlayoutmanager il=container->get_layoutmanager();

size_t s=il->size();

il->append_item([]
                (const INSERT_LIBX_NAMESPACE::w::factory &f)
                {
                     f->create_label("Lorem ipsum")->show();
                });

il->insert_item(0,
                []
                (const INSERT_LIBX_NAMESPACE::w::factory &f)
                {
                     f->create_label("Lorem ipsum")->show();
                });

il->remove_item(0);

INSERT_LIBX_NAMESPACE::w::element e=il->get_item(0);

\endcode

\c size() returns the current number of items in the container.
\c append_item() adds a new item to the end of the container, and
\c insert_item() adds a new item before an existing item in the container.
\c append_item() and \c insert_item() take a lambda or a callable object as
a parameter and invoke it with a \ref factory "factory". The lambda
must use the factory to create the item's label, usually a
\ref label "label", but it can be any widget. The callback must
explicitly \c show() the label element. The results of using the factory
to create more than one element are not specified.

For best results, all items in the list should have the same height. Always
using \c create_label() with the same font is a simple way to do that.

\c remove_item() removes an existing item from the list.
The items in the container are numbered consecutively, starting at 0. Removing
or adding an item in the middle of the container automatically renumber the
remaining items in the container.
\c get_item() returns an existing item element.

\par Locking and implementing the close button callback.

\code
INSERT_LIBX_NAMESPACE::w::itemlayout_lock lock{il};
\endcode

Constructing an \ref itemlayout_lock "itemlayout_lock" blocks all other
execution threads from accessing the items in the container.

\code
INSERT_LIBX_NAMESPACE::w::new_itemlayoutmanager nilm{
   []
   (ONLY IN_THREAD,
    size_t i,
    const INSERT_LIBX_NAMESPACE::w::itemlayout_lock &lock,
    const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger,
    const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
   {
          lock.layout_manager->remove_item(i);
   }
};

INSERT_LIBX_NAMESPACE::w::focusable_container fc=
    factory->create_container([]
                              (const INSERT_LIBX_NAMESPACE::w::focusable_container &fc)
                              {
                              });
\endcode

Passing an \ref new_itemlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_itemlayoutmanager"
to \c create_focusable_container() creates a new container that uses the
item layout manager.
\ref new_itemlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_itemlayoutmanager"'s
constructor's optional parameter is a callback. Items in the container are
drawn with a small "X" next to them, and clicking on an item's "X" invokes
the callback. The usual behavior is to remove the item from the list,
and the above example shows the default callback that gets installed,
unless a custom one gets passed to the constructor. The default callback
simply invokes \c remove_item() to remove the clicked item.

The callback's first parameter is the number of the item in the container whose
"X" got clicked, and the second parameter is the locked item list.

\code
il->on_remove(
   []
   (ONLY IN_THREAD,
    size_t i,
    const INSERT_LIBX_NAMESPACE::w::itemlayout_lock &lock,
    const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger,
    const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
   {
          lock.layout_manager->remove_item(i);
   });
\endcode

on_remove() removes the existing close button callback and installs the
replacement callback. Only one callback is installed, at a time, and
installing a new close callback replaces the existing one.

*/

typedef ref<itemlayoutmanagerObj> itemlayoutmanager;

//! A constant \ref itemlayoutmanager "item layout manager".

//! \see itemlayoutmanager

typedef const_ref<itemlayoutmanagerObj> const_itemlayoutmanager;

//! A nullable pointer reference to an \ref itemlayoutmanager "item layout manager".

//! \see itemlayoutmanager

typedef ptr<itemlayoutmanagerObj> itemlayoutmanagerptr;

//! A nullable pointer reference to a const \ref itemlayoutmanager "item layout manager".

//! \see itemlayoutmanager

typedef const_ptr<itemlayoutmanagerObj> const_itemlayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
