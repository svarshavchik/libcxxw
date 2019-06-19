/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerfwd_h
#define x_w_listlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/text_paramfwd.H>
#include <x/w/image_paramfwd.H>
#include <x/w/shortcutfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/w/popup_list_appearancefwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/listitemhandlefwd.H>
#include <x/w/busyfwd.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>
#include <x/mpobj.H>

#include <variant>
#include <optional>
#include <type_traits>
#include <functional>
#include <vector>
#include <string_view>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC listlayoutmanagerObj;
struct list_item_param;
class LIBCXX_HIDDEN listlayoutstyle_impl;
class LIBCXX_HIDDEN new_listlayoutmanager;
class LIBCXX_PUBLIC const_list_lock;
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

std::optional<size_t> n=l->selected();

std::vector<size_t> v=l->all_selected();

std::optional<size_t> n=l->current_list_item();

l->autoselect(item_number);

l->unselect();

\endcode

size() returns the number of elements in the list. selected(item_number)
indicates whether list item \#item_number is currently selected. An
overloaded selected() returns an optional size_t indicating which list
item is currently selected (or none). For list items that allow more than
one item to be selected, all_selected() indicates which items are selected.
Similarly, current_list_item() returns which list item is currently highlighted,
by the mouse pointer, or via keyboard navigation. This is meaningful only
if invoked from the connection thread; the list of selected items, and the
currently highlighted list item, can change at any time.

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
arbitrary widgets. The list layout manager contains only simple
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
several possibilites. The possible values consist of two classes of parameters:

- Parameters that specify discrete items, such as text labels and separator
items. Each such parameter creates a new list item.

- List item metadata, such as selection callbacks and keyboard shortcuts.

All metadata parameters apply to the immediately following list item, and
therefore cannot appear at the end of the vector. Two or more metadata
parameters for a single list item can appear in any relative order, as long
as they appear before the single list item.

\note
All changes to the context of the list get executed by the connection thread.
Using size() immediately after append_items(), for example, may still return
the previous list size; until the contents of list get updated by the
connection thread. As explained below, the IN_THREAD overloads are invocable
from the connection thread itself, most likely from another callback, and
they take effect immediately.

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
adds an image icon instead of a text label.

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

on_status_update() replaces the existing list item's callback.

\code
l->on_status_update(1,
                    []
                    (ONLY IN_THREAD,
                    const INSERT_LIBX_NAMESPACE::w::list_item_status_info_t &)
                    {
                    });
\endcode

This replaces item #1's callback, and invokes the new callback with
the trigger parameter's value of \c INSERT_LIBX_NAMESPACE::w::initial,
reporting whether or not the list item is currently selected.

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

The second optional parameter to INSERT_LIBX_NAMESPACE::w::submenu's
aggregate initialization list is an
"INSERT_LIBX_NAMESPACE::w::popup_list_appearance"
that specifies custom visual appearance of the menu.

\par Hierarchical lists

\code
auto f=l->append_items(
    {
        INSERT_LIBX_NAMESPACE::w::hierindent{1},
        "Lorem Ipsum",
    });
\endcode

\c INSERT_LIBX_NAMESPACE::w::hierindent indicates an indentation level for
the following list item.
Its parameter is the number of units of indentation.

The list layout manager uses the indentation level to horizontally shift
the contents of the list item; and does not treat the list item any
differently otherwise. This allows using the list layout manager to
present a hierarchical tree of items, with progressively-indented items
intuitively appearing as sub-items of the preceding items.

\note
The application is responsible for making sure that the list items'
hierarchy indentation level makes sense. Removing or adding new list items
has no effect on the hierarchy indentation level of other items in the list.
The hierindent() method returns the indentation level of an existing list
item, for information purposes.
The only way to modify an existing list item indentation level is to
replace it completely, with replace_items().

\note
A list that uses indentation levels should not set any column borders.
Column borders are not drawn for indented list items, at all, only
list items that have a default indentation level of 0.
When adding new list to the list,
\c INSERT_LIBX_NAMESPACE::w::hierindent applies only to the next specified
list item; and a non-zero indentation level must be explicitly specified
for each list item, individually.

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

         l->append_items(
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

\par Connection thread methods

remove_item(), append_items(), insert_items(), replace_items(),
replace_all_items(),
resort_items(),
autoselect(),
unselect(),
selected(),
on_status_update(),
and the enabled() overload which enables or disable a list item,
get executed by the internal connection thread. There are two versions
of each method. The first version sends a message to the internal
library connection thread that implements this method. The \c IN_THREAD
overload of each method is the implemention method that executes the
operation, and can be invoked directly when executing another callback
in the connection thread.

\par Creating handles for new list items

\code
#include <INSERT_LIBX_NAMESPACE/w/listitemhandle.H>

auto ret=l->append_items(
          {
                  "Lorem ipsum",
                  "dolor sit amet",
                  "consectetur",

                   INSERT_LIBX_NAMESPACE::w::get_new_items{},
          });

std::vector<INSERT_LIBX_NAMESPACE::w::listitemhandle> &handles=ret.handles;

\endcode

Additional information gets returned from methods that create new list items,
like append_items(),
when the last value in a
 \c std::vector of \ref list_item_param "list_item_param" variants
is an \ref get_new_items "INSERT_LIBX_NAMESPACE::w::get_new_items" value.

\c get_new_items, if specified, must be the last value in the vector when
creating
new items with append_items(), or other methods that create items.

Currently
the returned object contains a single value, \c handles, which is
a std::vector of
 \ref listitemhandle "INSERT_LIBX_NAMESPACE::w::listitemhandle"s.

This is empty by default, and gets returned only when
\c get_new_items gets specified. It takes additional work to create these
objects, so this work gets done only upon request.

\par Standard Copy/Cut/Paste menu items

\code
#include <INSERT_LIBX_NAMESPACE/w/copy_cut_paste_menu_items.H>

INSERT_LIBX_NAMESPACE::w::copy_cut_paste_menu_items items=l->append_copy_cut_paste(parent);

INSERT_LIBX_NAMESPACE::w::copy_cut_paste_menu_items items=l->append_copy_cut_paste(IN_THREAD, parent);
\endcode

append_copy_cut_paste() calls append_items() to create the standard
"Copy", "Cut", and "Paste" menu items. This is typically used with the
list layout manager to add them to the \ref menu "application's menu".

It's also possible to use append_copy_cut_paste() in a custom right pointer
button context popup menu of an \ref input_field "input field". All input
fields have a default right context popup menu with these options.
Calling an input field's install_contextpopup_callback() replaces the default
popup with a custom one. Using
append_copy_cut_paste() with that menu allows implementing a custom
input field context popup menu that includes these standard menu items.

append_copy_cut_paste()'s parameter is either:

- the \ref main_window "main window" when adding the default
"Copy", "Cut", and "Paste" menu items to an application field.

- a widget when adding the menu items to the element's right context
button popup.

append_copy_cut_paste() returns
an \ref copy_cut_paste_menu_items "INSERT_LIBX_NAMESPACE::w::copy_cut_paste_menu_items"
object.

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

//! A superclass of \ref submenu "submenu" that holds the creator callback.

//! Makes it possible to use aggregate initialization with submenu, and
//! default-construct the optional custom submenu appearance.

struct submenu_creator {

	//! Callback invoked to create the submenu.

	functionref<void (const listlayoutmanager &)> creator;
};

//! A superclass of \ref submenu "submenu" that holds the submenu's appearance.

//! Makes it possible to use aggregate initialization with submenu, and
//! default-construct the const_popup_list_appearance using only a
//! forward-declaration

struct submenu_appearance {

	//! Submenu's appearance.

	const_popup_list_appearance appearance;

	//! Default constructor

	submenu_appearance();

	//! Constructor
	submenu_appearance(const const_popup_list_appearance &appearance);

	//! Copy constructor
	submenu_appearance(const submenu_appearance &appearance);

	//! Assignment operator
	submenu_appearance &operator=(const submenu_appearance &appearance);

	//! Destructor
	~submenu_appearance();
};

//! This is a menu item entry for a sub menu.

struct submenu : submenu_creator, submenu_appearance {};

//! This is a selectable menu option.

struct menuoption {};

//! This is a separator.

struct separator {};

//! Indentation level

struct hierindent { size_t n; };

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

struct LIBCXX_PUBLIC new_items_ret;

//! Save handles for new items.

//! Passed as a last value to a \ref listlayoutmanager "list layout manager"
//! method that creates new list items, to indicate that additional information
//! should be returned from the method.
//!
//! This avoids doing all the work to create the additional objects, that get
//! returned, unless needed.

struct get_new_items {};

//! Saved handles for new items

//! This object is returned by \ref listlayoutmanager "list layout manager"
//! methods that create new items. This object is normally empty unless the
//! list of new items, that gets passed to the method, ends with a
//! \ref get_new_items "get_new_items" value.
//!
//! The copy constructor and the assignment operator are deleted. The return
//! value from the method is intended to be saved in an auto variable,
//! examined, and destroyed.

struct new_items_ret {

	//! New handles
	std::vector<listitemhandle> handles;

	//! Default constructor
	new_items_ret();

	//! Copy constructor declared but not defined

	//! This is used to internally enforce copy elision in all contexts.
	new_items_ret(const new_items_ret &);

	//! Move constructor declared but not defined

	//! This is used to internally enforce copy elision in all contexts.
	new_items_ret(new_items_ret &&);

	//! Assignment operator is deleted

	new_items_ret &operator=(const new_items_ret &)=delete;

	//! Destructor
	~new_items_ret();
};

//! List item callback's signature

//! \see listlayoutmanager
typedef void list_item_status_change_callback_t(THREAD_CALLBACK,
						const list_item_status_info_t
						&);

//! Type-erased list_item_status_change_callback_t.

typedef functionref<list_item_status_change_callback_t
		      > list_item_status_change_callback;

//! Type-erased list_item_status_change_callback_t.

typedef functionptr<list_item_status_change_callback_t
		    > list_item_status_change_callbackptr;

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
		     inactive_shortcut,
		     hierindent,
		     menuoption, submenu,
		     list_item_status_change_callback,
		     get_new_items> list_item_param_base;

//! The list highlights items to indicate they're selected

extern const listlayoutstyle_impl &highlighted_list LIBCXX_PUBLIC;

//! A bullet is to the left of each selected item

extern const listlayoutstyle_impl &bulleted_list LIBCXX_PUBLIC;

//! Return a list layout style by name.

const listlayoutstyle_impl &list_style_by_name(const std::string_view &);

//! selection_type callback.

//! The selection type callback gets invoked with an internal list lock in
//! place. The selection type callback does not need to lock the list.
//!
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
//! visually selected by directly invoking the list layout manager's
//! selected() method).

extern const list_selection_type_cb_t no_selection_type LIBCXX_PUBLIC;

LIBCXXW_NAMESPACE_END

#endif
