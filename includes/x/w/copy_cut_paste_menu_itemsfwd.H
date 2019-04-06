/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_copy_cut_paste_menu_itemsfwd_h
#define x_w_copy_cut_paste_menu_itemsfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN copy_cut_paste_menu_itemsObj;

/*! Standard "Copy"/"Cut"/"Paste" menu items.

\code
INSERT_LIBX_NAMESPACE::w::copy_cut_paste_menu_items ccp=l->append_copy_cut_paste(main_window);

INSERT_LIBX_NAMESPACE::w::copy_cut_paste_menu_itemsptr ccp;

INSERT_LIBX_NAMESPACE::w::menu file_menu=
        mbf->add([]
             (const auto &f)
             {
                 f->create_label("File");
             },
             [&]
             (const x::w::listlayoutmanager &lm)
             {
                 ccp=lm->append_copy_cut_paste(mw);
             });

    // Attach a callback that gets invoked before the "File" menu
    // becomes visible (or hidden). Use this callback to enable or
    // disable the Cut/Copy/Paste menu items depending upon whether the
    // given operation is possible at this time.

file_menu->on_popup_state_update
        ([ccp=x::w::copy_cut_paste_menu_items{ccp}]
         (ONLY IN_THREAD,
          const x::w::element_state &es,
          const x::w::busy &mcguffin)
         {
             if (es.state_update != es.before_showing)
                 return;

             ccp->update(IN_THREAD);
         });
\endcode

This object gets returned by the \ref listlayoutmanager "list layout manager's"
append_copy_paste() method which creates default "Cut", "Copy", and "Paste"
menu items, which are enabled by default.

It is desirable to enable the individual items only if the corresponding
operation is possible (there's an available selection). This is done by
taking the return object and calling its update() method. update()
enables or disables these menu items, accordingly.

The typical approach is to take the object that append_copy_paste() returns,
and call its update() just before the menu becomes visible, using
\ref menu "on_popup_state_update()".

A \c copy_cut_paste_menu_items does maintain strong references to the
created "Cut", "Copy", and "Paste" menu items, only weak references.
The weak references follow these items in their menu, if other menu items
get added or delete from the menu. update() does nothing
if its menu items, or their entire menu, get removed.

*/

typedef ref<copy_cut_paste_menu_itemsObj> copy_cut_paste_menu_items;

//! Constant \ref copy_cut_paste_menu_items "copy/cut/paste menu items".

//! \see copy_cut_paste_menu_items

typedef const_ref<copy_cut_paste_menu_itemsObj> const_copy_cut_paste_menu_items;

//! A nullable pointer reference to \ref copy_cut_paste_menu_items "copy/cut/paste menu items".

//! \see copy_cut_paste_menu_items

typedef ptr<copy_cut_paste_menu_itemsObj> copy_cut_paste_menu_itemsptr;

//! A nullable pointer reference to const \ref copy_cut_paste_menu_items "copy/cut/paste menu items".

//! \see copy_cut_paste_menu_items

typedef const_ptr<copy_cut_paste_menu_itemsObj> const_copy_cut_paste_menu_itemsptr;

LIBCXXW_NAMESPACE_END

#endif
