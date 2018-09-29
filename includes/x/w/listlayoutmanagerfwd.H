/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerfwd_h
#define x_w_listlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/text_paramfwd.H>
#include <x/w/image_paramfwd.H>
#include <x/w/shortcutfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/busyfwd.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>
#include <x/mpobj.H>

#include <variant>
#include <type_traits>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC listlayoutmanagerObj;
struct list_item_param;
class LIBCXX_HIDDEN listlayoutstyle_impl;
class LIBCXX_HIDDEN new_listlayoutmanager;
class LIBCXX_PUBLIC list_lock;

/*! The list layout manager.

The list layout manager arranges its contents in a vertical list
and implements a selection list. Each item in the list is on its own row.
Moving the pointer over the item and clicking it selects it or unselects it;
or unselects any other item that was selected previously, this depends on
the \ref new_listlayoutmanager "new_listlayoutmanager".

The list layout manager can also be tabbed to. With the keyboard focus in the
selection list "Cursor-Down" highlights each item in the list starting with
the first one, "Cursor-Up" highlights the previous item in the list, and
"Enter" selects or unselects the list item.

The list layout manager also serves as the underlying layout manager for
pop-up menus and the list of options for a combo-box; in which case selecting
a list item results in the action that corresponding to selecting the
menu item, or the combo-box option item. The
\ref standard_comboboxlayoutmanager "standard combo-box layout manager"
overrides many of the methods in order to require, by definition, that only
text labels can be added to the list.

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

autoselect(item_number) produces the same results as if the given item
number was clicked on with a pointer or the keyboard,
selecting/unselecting any items
according to the list's \ref new_listlayoutmanager "selection_type".

selected(item_number, flag) manually selects or unselects the given item
number. Normally each list item gets selected or unselected with the
pointer or keyboard. This manually changes an individual list item's
status to either the selected or the unselected status.

unselect() unselects all currently selected items (if any).

With one exception, select() and unselect() should not be used with
menus or combo-boxes, only with generic containers that use this layout
manager. The exception: menu items that are selectable menu options.

\note
List selection gets updated by an internal execution thread. Calling
selected(item_number) after selecting or unselecting an item,
using selected() or autoselect(), will not report list item's revised
selection status until the internal execution thread officially updates it.

The \c selection_changed callback, installed when the list was created,
gets invoked when list items' selected status gets officially updated.

\par Appearance

\code
INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager new_list;

new_list.height=10;
new_list.vertical_scrollbar=INSERT_LIBX_NAMESPACE::w::scrollbar_visibility::always;

f->create_focusable_container
    ([]
     (const INSERT_LIBX_NAMESPACE::w::focusable_container &c)
     {
         INSERT_LIBX_NAMESPACE::w::listlayoutmanager l=c->get_layoutmanager();

         // ...
     }, new_list);

\endcode

The list gets automatically sized to accomodate its widest item.
Several fields in the \ref new_listlayoutmanager "new_listlayoutmanager"
control other aspects of the list's appearance. \c height sets the list's
height, this example sets the list to be ten rows tall. The list scrolls
if there are more than ten items in the list.
The list's actual height gets computed
based on the list's default font, and the actual number of items that get
displayed may vary if there are list items that are anything other than
plain text with the default font. Setting \c vertical_scrollbar adjusts the
\ref scrollbar_visibility "scroll-bar's appearance"

\code
INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager new_list;

new_list.variable_height();
new_list.vertical_scrollbar=INSERT_LIBX_NAMESPACE::w::scrollbar_visibility::never;
\endcode

A list added as a vertical pane inside another container that uses
the \ref panelayoutmanager "pane layout manager" should turn off its own
scroll-bar entirely, and specify a variable_height(). This makes the list's
height adjustable, and the pane layout manager supplies the vertical
scroll-bar for the list, when needed.

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

l->append_items({"Lorem ipsum"});

size_t item_number;

l->insert_items(item_number, {"Lorem ipsum"});

l->remove_item(item_number);

l->replace_items(item_number, {"Lorem", "ipsum"});

l->replace_all_items(item_number, {"Lorem ipsum"});

\endcode

Unlike other layout managers, the list layout manager does not manage
arbitrary display elements. The list layout manager contains only simple
text labels.

append_items() adds new list items to the end of the existing list.
insert_items() inserts new list items before an existing item in the list.
replace_item() replaces existing items in the list.
removes_item() removes a single item from the list.
replace_all_items() is equivalent to removing all existing items from the
list and then using append_items() to set new list items.

Each list item gets identified by its index number, starting with index 0.
This item number gets passed to insert_items(), replace_items(), and
remove_item(). Adding and removing items from the list automatically
renumbers the remaining items in the list.

New list items get specified by a \c std::vector of
\ref list_item_param "list_item_param"s, which is a \c std::variant with
several possibilites. The possible values consist of two classes of parameters

- Parameters that specify discrete items, such as text labels and separator
items. Each such parameter creates a new list item.

- List item metadata, such as selection callbacks and keyboard shortcuts.

All metadata parameters apply to the immediately following list item, and
therefore cannot appear at the end of the vector. When more than one
metadata parameter gets specified for a single list item, the parameters
may be specified in any relative order.

\par Text label items

\code
l->insert_items(0, {"Lorem", INSERT_LIBX_NAMESPACE::w::text_param{"ipsum"}});
\endcode

A \c std::string, or a \c std::u32string specifies an individual list
item, as a locale-encoded string or a unicode string. Alternatively, an
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param" object specifies
a list item with font and color mark-ups.

This example inserts two text items before the existing item #0.

An \ref image_param "INSERT_LIBX_NAMESPACE::w::image_param" value
is not documented, and reserved for internal library use, at this time.

\par Separators

\code
l->append_items(0, {"Lorem",
                    INSERT_LIBX_NAMESPACE::w::separator{},
                    "Ipsum"});
\endcode

An \c INSERT_LIBX_NAMESPACE::w::separator parameter serves as a placeholder
for a horizontal line that visually separates groups of items in the list.
The separator line is considered a list item, gets numbered like all
other items, and may be removed or replaced, with remove_item(), or
replace_items().

\par Sorting the list

\code
std::vector<size_t> index;

l->resort_items(index);
\endcode

resort_items() reorders the items in the list. The index vector's size must
match the number of items in the list. The index vector's values must consist
of strictly increasing values, starting at 0, in some particular order,
meaning that: for a list of 8 items, the index vector must contain values
0 through 7.

resort_items() reorders the items in its list as if the index vector gets
sorted in its natural order, and the corresponding item in the list gets
repositioned accordingly. For example, when the index vector contains
the values:

\code
std::vector<size_t> index={6, 1, 2, 0, 4, 5, 3, 7};

l->resort_items(index);
\endcode

This ends up moving the first item in the list, item index \#0 to item \#6,
item \#6 to \#3, and item \#3 to position \#0.

The results are unspecified if the index vector fails to meet the stated
requirements.

\par Selection callbacks

\code
l->insert_items(0,
               {
                   []
                   (ONLY IN_THREAD,
                    const INSERT_LIBX_NAMESPACE::w::list_item_status_info_t &)
                   {
                   },
		   "Lorem",
                   "Ipsum"
	       });
\endcode

A callable object (typically a simple lambda) specifies a
\ref list_item_status_change_callback_t "callback" that
gets invoked whenever the following list item gets selected or unselected.
The callable object gets attached to the immediately-following text item.
The above example attaches the callback object to the "Lorem" item.
No callback gets attached to the following "Ipsum" item, unless one is
also specified.

\par Selectable menu options

\code
l->append_items({
                   []
                   (ONLY IN_THREAD,
                    const INSERT_LIBX_NAMESPACE::w::list_item_status_info_t &)
                   {
                   },
                   INSERT_LIBX_NAMESPACE::w::menuoption{},
		   "Detailed view",
                   });
\endcode

This is used only with list layout managers that manage \ref menu "menu"s.
By default, selecting a menu item does not render it
as selected or unselected, as with separate list items. Only its
callback gets invoked.

An \c INSERT_LIBX_NAMESPACE::w::menuoption parameter indicates that the
following item is a selectable option. In addition to invoking its callback,
a visual indicator (usually a small bullet) gets added or removed next to
the list item.

\par Menu item shortcuts

\code
l->append_items({
                   INSERT_LIBX_NAMESPACE::w::shortcut{"Alt", 'O'),
                   []
                   (ONLY IN_THREAD,
                    const INSERT_LIBX_NAMESPACE::w::list_item_status_info_t &)
                   {
                   },
		   "Open",
                   });
\endcode

This is used only with list layout managers that manage \ref menu "menu"s.
Passing a \ref shortcut "shortcut" parameter sets a keyboard shortcut for
the following list item. The shortcut key combination results in the same
action as when the following item gets selected with the mouse pointer or
the keyboard.

You cannot install a shortcut for a separator item (makes no sense), or
a submenu.

\par Submenus

\code
l->append_items({
                   INSERT_LIBX_NAMESPACE::w::submenu{
                       []
                       (const INSERT_LIBX_NAMESPACE::w::listlayoutmanager &)
                       {
                       }
                   },
		   "Recent files...",
                   });
\endcode

This is used only with list layout managers that manage \ref menu "menu"s.
The \ref submenu "INSERT_LIBX_NAMESPACE::w::submenu" parameter gets initialized
with a callable object (typically a closure) that creates a sub-menu that
gets attached to the following list item. The closure receives another
list layout manager as its sole parameter, that represents the sub-menu's
list layout manager.

The closure serves to create the contents of the sub-menu. The closure gets
invoked by the library, and is responsible for creating the contents of the
sub-menu.

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

\code
INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager new_list;

new_list.columns=3;

f->create_focusable_container
    ([]
     (const INSERT_LIBX_NAMESPACE::w::focusable_container &c)
     {
         INSERT_LIBX_NAMESPACE::w::listlayoutmanager l=c->get_layoutmanager();

         auto f=l->append_items(
               {
                   // First list item
		   []
                   (const INSERT_LIBX_NAMESPACE::w::list_item_status_info_t &)
                   {
		       // ...
                   },
                  "Lorem ipsum", "dolor sit amet", "consectetur",

                   // Second list item, a separator
		   INSERT_LIBX_NAMESPACE::w::separator{},

                   // Third list item
		   []
                   (const INSERT_LIBX_NAMESPACE::w::list_item_status_info_t &)
                   {
		       // ...
                   },
		   "adipisicing", "elit", "sed do eiusmod"
	       });
     },
     new_list);
\endcode

Setting \ref new_listlayoutmanager "new_listlayoutmanager"'s \c columns
member creates a list with multiple columns. The number of discrete items
passed to append_items(), insert_items(), replace_items(),
and replace_all_items must be an even multiple
of the number of columns, with one exception. Each group of items specified
by the column count
becomes a new item, with each text string in the corresponding column,

The sole exception is a list item separator. This parameter always gets
specified by itself, as a single value.

\par Internal thread methods

remove_item(), append_items(), insert_items(), replace_items(),
replace_all_items(),
autoselect(),
unselect(),
selected(),
and the enabled() overload which enables or disable a list item,
get executed by the internal connection thread. There are two versions
of each method. The first version sends a message to the internal
library connection thread that implements this method. The \c IN_THREAD
version of each message is the implemention method that executes the
operation.

*/

typedef ref<listlayoutmanagerObj> listlayoutmanager;

//! A constant \ref listlayoutmanager "text-only list layout manager".

//! \see listlayoutmanager

typedef const_ref<listlayoutmanagerObj> const_listlayoutmanager;

//! A nullable pointer reference to a \ref listlayoutmanager "text-only list layout manager".

//! \see listlayoutmanager

typedef ptr<listlayoutmanagerObj> listlayoutmanagerptr;

//! A nullable pointer reference to a const \ref listlayoutmanager "text-only list layout manager".

//! \see listlayoutmanager

typedef const_ptr<listlayoutmanagerObj> const_listlayoutmanagerptr;

struct LIBCXX_HIDDEN listimpl_info_s;

//! Internal \ref listlayoutmanager "listlayoutmanager" mutex-protected data.

typedef mpobj<listimpl_info_s, std::recursive_mutex> listimpl_info_t;

//! A forward reference to a lock on a \ref listimpl_info_t "listimpl_info_t".

typedef mpobj_lock<listimpl_info_s, std::recursive_mutex> listimpl_info_lock_t;

//! This is a menu item entry for a sub menu.

struct submenu {

	//! Callback invoked to create the submenu.

	functionref<void (const listlayoutmanager &)> creator;
};

//! This is a selectable menu option.

struct menuoption {};

//! This is a separator.

struct separator {};

//! Parameters passed to a list item status change callback.

//! \see listlayoutmanager

struct LIBCXX_PUBLIC list_item_status_info_t {

	//! The list layout manager.
	const listlayoutmanager &layout_manager;

	//! No need to create a lock on the list, here it is.

	const list_lock &lock;

	//! Which item's status has changed
	size_t item_number;

	//! New item status
	bool selected;

	//! What triggered the selection.
	const callback_trigger_t &trigger;

	//! A busy mcguffin, if needed.
	const busy &mcguffin;
};

//! List item callback's signature

//! \see listlayoutmanager
typedef void list_item_status_change_callback_t(THREAD_CALLBACK,
						const list_item_status_info_t
						&);

//! Type-erased list_item_status_change_callback_t.

typedef functionref<list_item_status_change_callback_t
		      > list_item_status_change_callback;

//! SFINAE template for overloading a list_item_status_change_callback_t parameter.

template<typename callback>
using is_list_item_status_change_callback=
	std::is_same<std::void_t<
			     decltype(std::declval<callback &&>()
				      (std::declval<THREAD_CALLBACK>(),
				       std::declval<const
				       list_item_status_info_t &>()))>,
		     void>;

//! A new list item.

//! Base class for \ref listlayoutmanager "listlayoutmanager"'s
//! insert_items(), and append_items() parameters.

typedef std::variant<text_param, image_param, separator, shortcut,
		     menuoption, submenu,
		     list_item_status_change_callback> list_item_param_base;

//! The list highlights items to indicate they're selected

extern const listlayoutstyle_impl &highlighted_list LIBCXX_PUBLIC;

//! A bullet is to the left of each selected item

extern const listlayoutstyle_impl &bulleted_list LIBCXX_PUBLIC;


//! selection_type callback.

//! \see new_listlayoutmanager

typedef functionref<void (THREAD_CALLBACK,
			  const listlayoutmanager &, size_t,
				  const callback_trigger_t &,
			  const busy &)
		    > list_selection_type_cb_t;

//! Single selection list.

//! Default selection list type specified by the
//! \ref new_listlayoutmanager "new_listlayoutmanager".
//!
//! Only one list item may be selected at any time. Selecting another item
//! de-selects the currently-selected item.

extern const list_selection_type_cb_t single_selection_type LIBCXX_PUBLIC;


//! Single, but optional, selection list.

//! Like single_selection_type(), but selecting the currently selected item
//! again will deselect it.

extern const list_selection_type_cb_t single_optional_selection_type
LIBCXX_PUBLIC;


//! Multiple selection list.

//! Multiple list items may be selected at any time.

extern const list_selection_type_cb_t multiple_selection_type LIBCXX_PUBLIC;

//! No-selection list.

//! No list items can be selected at any time.
//!
//! This results in a list, potentially scrollable, that exists solely
//! for the virtue of presenting a potentially scrollable list. Clicking
//! on a list item has no effect. (The individual list item can still be
//! selected().

extern const list_selection_type_cb_t no_selection_type LIBCXX_PUBLIC;

LIBCXXW_NAMESPACE_END

#endif
