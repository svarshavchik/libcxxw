/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerfwd_h
#define x_w_listlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/ptrfwd.H>
#include <functional>

LIBCXXW_NAMESPACE_START

class listlayoutmanagerObj;

/*! The list layout manager.

The list layout manager arranges its contents in a vertical list
and implements a selection list. Each item in the list is on its own row.
Moving the pointer over the item and selects it or unselects it; or unselects
any other item that was selected previously, this depends on
the \ref new_listlayoutmanager "new_listlayoutmanager".

The list layout manager can also be tabbed to. With the keyboard focus in the
selection list "Cursor-Down" highlights each item in the list starting with
the first one, "Cursor-Up" highlights the previous item in the list, and
"Enter" selects or unselects the list item.

\code
#include <INSERT_LIBX_NAMESPACE/w/listlayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::listlayoutmanager l=container->get_layoutmanager();

size_t s=l->size();

size_t item_number;

bool flag=l->selected(item_number);

l->selected(item_number, true);

l->autoselect(item_number);

l->unselect();

\endcode

size() returns the number of elements in the list. selected(item_number)
indicates whether list item \#item_number is currently selected.

selected(item_number, flag) selects or unselects the given item number.
autoselect(item_number) produces the same results as if the given item
number was clicked on, with a pointer, selecting/unselect any items
according to the list's \ref new_listlayoutmanager "selection_type".

unselect() unselects all currently selected items (if any).

\note
List selection gets updated by an internal execution thread. Calling
selected(item_number) after selecting or unselecting an item,
using selected() or autoselect(), will not report list item's revised
selection status until the internal execution thread officially updates it.

The \c selection_changed callback, installed when the list was created,
gets invoked when list items' selected status gets officially updated.

\par List item callbacks

\code
l->callback_factory([]
                    {
                         return []
                                (INSERT_LIBX_NAMESPACE::w::list_lock &lock,
                                 size_t i,
                                 bool flag)
                                {
                                     // ...
                                };
                     });

l->remove_callback_factory();
\endcode

Each time a new list item is created by this list layout manager the
callback_factory() gets called to get the new item's callback. This callback
also gets invoked, but only for its list item, when it gets selected or
unselected. The callback remains attached to its list item, after any other
items are added or removed to the list.

remove_callback_factory() removes the list layout manager's current callback
factory. Until it gets removed, or by a replacement callback_factory(), the
factory gets invoked for every new list item, and the returned lambda becomes
the new item's callback.

The list item callback receives, as parameters:

- a list lock object.

- the current index of the item, in the list.

- an indication whether the item is now selected or unselected.

\par Locking the list

\code
INSERT_LIBX_NAMESPACE::w::list_lock lock{l};
\endcode

This locks the contents of the list, preventing all other execution threads
from accessing the list until the lock object goes out of scope and gets
destroyed.
All other list layout manager methods, like \c size() and \c selected()
see a consistent snapshot of the list, until the lock goes away.

\note
This lock blocks the internal execution thread if it tries to access the list
in order to process a pointer or a keybaord event (this includes pointer
movement). The lock should not be held for an extended time.

List items that get selected() or autoselect()ed get their official selection
status updated only after the lock gets released.

\par Modifying the contents of the list

\code
#include <INSERT_LIBX_NAMESPACE/w/factory.H>

l->append_item("Lorem ipsum");

size_t item_number;

l->insert_item(item_number, "Lorem ipsum");

l->remove_item(item_number);

l->replace_item(item_number, "Lorem ipsum");

\endcode

The contents of the selection list are modified by obtaining a
\ref factory "factory" from the list layout manager and using the
factory to create display elements.

\note
List item text may be
a \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param" with multiple
fonts and colors; but using non-default fonts of a different height results
in the actual list's height not matching the exact number of rows specified.
The height of the list gets calculated based on
the list's default label font, and the number of rows.

append_item() appends a new list item to the end of the list.

insert_item() inserts a new list item in the middle of the existing list.
The parameter specifies the position of the new list
item, with all existing list items starting at this position shifted to make
room for the new item.

append_item() returns a factory which appends the new item to the end of
the list. Using the same factory to create another display element
appends another item to the end of the list.

\note
An empty list item text string creates a thin horizontal separator divider
that's designed to visually separate multiple group of items in the list.

Using the same factory to create another display
element insert another item after the first one, shifting the existing
items again.

remove_item() removes an existing item from the list. Any items that follow
the remove items get shifted over by one position, to fill in the missing
gap.

\code
l->append_item("Lorem ipsum", "dolor sit amet");

std::vector<INSERT_LIBX_NAMESPACE::w::text_param> items={
	"Lorem ipsum",
	"dolor sit amet"};

l->append_item(items);

l->insert_item(0, "Lorem ipsum", "dolor sit amet");
l->insert_item(0, items);

l->replace_item(0, "Lorem ipsum", "dolor sit amet");
l->replace_item(0, items);

\endcode

Overloads of append_item(), insert_item(), and replace_item() insert multiple
list items in one operation. Multiple items may be specified either as
variadic parameters, or a vector of \ref text_param "text_param"s.

\par Enabling and disabling list items

\code
if (l->enabled(4))
    l->enabled(4, false);
\endcode

This disables list item #4. Disabled list items cannot be selected with
the pointer or keyboard.

\note
List item status gets updated by the library's internal execution thread.
Enabling or disabling an item sends a message to the execution thread, so
immediately calling \c enabled() after changing the item's status may still
report that the item's status did not change.

\par Lists with multiple columns.

Overloaded bulk methods must be used with multiple column lists.

\code
INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager new_list;

new_list.columns=3;

f->create_focusable_container
    ([]
     (const INSERT_LIBX_NAMESPACE::w::focusable_container &c)
     {
         INSERT_LIBX_NAMESPACE::w::listlayoutmanager l=c->get_layoutmanager();

         auto f=l->append_item(

               // First list item
               "Lorem ipsum", "dolor sit amet", "consectetur",
               "adipisicing", "elit", "sed do eiusmod");
     },
     new_list);
\endcode

Setting \ref new_listlayoutmanager "new_listlayoutmanager"'s \c columns
member creates a list with multiple columns. The overloaded versions
of append_item(), insert_item(), and replace_item() must be used, and
the number of items appended/inserted/replaced must be an even multiple
of the number of columns. Each group of items specified by the column count
becomes a new item, with each text string in the corresponding column,

\par Non-text list items

\code
#include <INSERT_LIBX_NAMESPACE/w/factory.H>

INSERT_LIBX_NAMESPACE::w::factory f=l->append_item();

f->create_label("Lorem ipsum");

size_t item_number;

INSERT_LIBX_NAMESPACE::w::factory f=l->insert_item(item_number);

f->create_label("Lorem ipsum");

INSERT_LIBX_NAMESPACE::w::factory f=l->replace_item(item_number);

f->create_label("Lorem ipsum");
\endcode

Items in the selection list are not limited to text strings. Any
non-focusable display element can serve as a list item. This includes
a nested container, with multiple elements inside the nested container.

Overloaded append_item(), insert_item(), and replace_item() return a
generic \ref factory "factory", for creating any non-focusable display
element to serve as a list item.

Adding a text item to a list is exactly equivalent to creating a factory,
then using create_label(), except that an empty text string creates
a horizontal divider row. create_label("") creates a verbatim empty list
item. Only append_item(), insert_item(), and replace_item() create
horizontal dividers instead of empty list items.

For lists with multiple columns, the factory gets used to create the requisite
number of display elements, which become a single list item.

\code
         auto f=l->append_item();

         // First list item
         f->create_label("Lorem ipsum");
	 f->create_label("dolor sit amet");
         f->create_label("consectetur");
\endcode

For multi-column lists, the same factory must be used to create each
column's display element.

The overloaded append_item() returns a factory that adds the display element(s)
as a new item at the end of th elist.

insert_item() returns a factory which inserts the new item in the middle
of the existing list.

replace_item() returns a factory that replaces an existing list item with
a new one.

\note
The list layout manager automatically show()s new display element. It is not
necessary to manually show new list item elements.

*/

typedef ref<listlayoutmanagerObj> listlayoutmanager;

//! A constant \ref listlayoutmanager "list layout manager".

//! \see listlayoutmanager

typedef const_ref<listlayoutmanagerObj> const_listlayoutmanager;

//! A nullable pointer reference to a \ref listlayoutmanager "list layout manager".

//! \see listlayoutmanager

typedef ptr<listlayoutmanagerObj> listlayoutmanagerptr;

//! A nullable pointer reference to a const \ref listlayoutmanager "list layout manager".

//! \see listlayoutmanager

typedef const_ptr<listlayoutmanagerObj> const_listlayoutmanagerptr;

class listlayoutstyle;

class new_listlayoutmanager;

class list_lock;

//! The list highlights items to indicate they're selected

extern const listlayoutstyle &highlighted_list;

//! A bullet is to the left of each selected item

extern const listlayoutstyle &bulleted_list;

//! Single selection list.

//! Default selection list type specified by the
//! \ref new_listlayoutmanager "new_listlayoutmanager".
//!
//! Only one list item may be selected at any time.

void single_selection_type(list_lock &lock,
			   const listlayoutmanager &layout_manager,
			   size_t i,
			   const busy &mcguffin);

//! Multiple selection list.

//! Multiple list items may be selected at any time.
void multiple_selection_type(list_lock &lock,
			     const listlayoutmanager &layout_manager,
			     size_t i,
			     const busy &mcguffin);

//! List item callback's signature

//! The list item callback receives a lock object, the current index of the
//! callback's list item, and whether the list item is currently selected.
typedef void list_item_status_change_callback_t(list_lock &lock,
						size_t i,
						bool selected);

//! A list item callback factory

//! The factory gets called when a new list item gets created. The factory
//! returns a callback for the new list item.
typedef std::function<list_item_status_change_callback_t
		      > list_item_status_change_callback_factory_t();

LIBCXXW_NAMESPACE_END

#endif
