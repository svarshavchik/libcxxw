/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menulayoutmanagerfwd_h
#define x_w_menulayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/ptrfwd.H>
#include <variant>

LIBCXXW_NAMESPACE_START

class menulayoutmanagerObj;

/*! Layout manager for a menu popup.

Inherits from the list layout manager. Do not use the list layout manager's
methods, only use this class's methods to manipulate the menu.

\code
INSERT_LIBX_NAMESPACE::w::menufactory f=l->append_menu_item();

INSERT_LIBX_NAMESPACE::w::menufactory f=l->insert_menu_item(0);

INSERT_LIBX_NAMESPACE::w::menufactory f=l->replace_menu_item(0);

INSERT_LIBX_NAMESPACE::w::menufactory f=l->replace_all_menu_items();
\endcode

By themselves and without any additional parameters,
these methods return a factory that adds new items to the menu.
append_menu_item()'s factory adds new items to the end of the existing menu.
insert_menu_item() inserts new items before an existing menu item.
replace_menu_item() returns a factory that replaces existing menu items.
replace_all_menu_items() removes all existing menu items, before adding the
new ones.

If additional parameters are present, these methods are a short-cut for
specifying new menu items as \ref text_param "plain text labels":

\code
l->append_menu_item("New", "Open", "Close");

l->insert_menu_item(0, "New");

l->replace_menu_item(0, "New");

l->replace_all_menu_items(0, "New", "Open", "Close");
\endcode

The additional parameters are a variadic parameter list. The possible parameters
are:

- Simple text strings or \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param".
An empty text string creates a horizontal divider line that visually
separates groups of menu items.

- A \ref menuitem_type_t "INSERT_LIBX_NAMESPACE::w::menuitem_type_t" for
an immediately following
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param". A
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param" without an immediately
preceding \ref menuitem_type_t "INSERT_LIBX_NAMESPACE::w::menuitem_type_t"
defaults to a \ref menuitem_plain "INSERT_LIBX_NAMESPACE::menuitem_plain".

- A \c std::vector of \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param"s.

- A \c std::vector of \c std::tuple-s of
\ref menuitem_type_t "INSERT_LIBX_NAMESPACE::w::menuitem_type_t" and
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param"s.

\code
l->update(INSERT_LIBX_NAMESPACE::w::menuitem_plain{});
\endcode

update() modifies an existing menu item's
\ref menuitem_type_t "INSERT_LIBX_NAMESPACE::w::menuitem_type_t". A menu's
item are numbered, starting at 0 (including empty strings that draw
horizontal diviers).

\code
INSERT_LIBX_NAMESPACE::w::menulayoutmanagerptr submenu=l->get_item_layoutmanager(2);
\endcode

get_item_layoutmanager() returns a \ref menulayoutmanager "menu layout manager"
for this menu's submenu. A null pointer reference gets returned if the
specified item does not have a submenu.

\code
INSERT_LIBX_NAMESPACE::w::menu_lock lock{l};
\endcode

A \ref menu_lock "menu lock" acquires a lock on the layout manager's menu,
blocking access by other threads, obtaining a consistent snapshot of the
menu's contents.

*/

typedef ref<menulayoutmanagerObj> menulayoutmanager;

//! A constant \ref menulayoutmanager "menu layout manager".

//! \see menulayoutmanager

typedef const_ref<menulayoutmanagerObj> const_menulayoutmanager;

//! A nullable pointer reference to a \ref menulayoutmanager "menu layout manager".

//! \see menulayoutmanager

typedef ptr<menulayoutmanagerObj> menulayoutmanagerptr;

//! A nullable pointer reference to a const \ref menulayoutmanager "menu layout manager".

//! \see menulayoutmanager

typedef const_ptr<menulayoutmanagerObj> const_menulayoutmanagerptr;

struct menuitem_plain;
struct menuitem_submenu;

//! The menu item type

//! The default menu item type is menuitem_plain.

typedef std::variant<menuitem_plain, menuitem_submenu> menuitem_type_t;

//! Menus use the same lock as lists, for now.

typedef list_lock menu_lock;

LIBCXXW_NAMESPACE_END

#endif
