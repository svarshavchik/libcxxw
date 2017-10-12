/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistlayoutmanagerfwd_h
#define x_w_textlistlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/text_paramfwd.H>
#include <x/w/image_paramfwd.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>

#include <variant>

LIBCXXW_NAMESPACE_START

class textlistlayoutmanagerObj;

/*! A text-only list layout manager.


*/

typedef ref<textlistlayoutmanagerObj> textlistlayoutmanager;

//! A constant \ref textlistlayoutmanager "text-only list layout manager".

//! \see textlistlayoutmanager

typedef const_ref<textlistlayoutmanagerObj> const_textlistlayoutmanager;

//! A nullable pointer reference to a \ref textlistlayoutmanager "text-only list layout manager".

//! \see textlistlayoutmanager

typedef ptr<textlistlayoutmanagerObj> textlistlayoutmanagerptr;

//! A nullable pointer reference to a const \ref textlistlayoutmanager "text-only list layout manager".

//! \see textlistlayoutmanager

typedef const_ptr<textlistlayoutmanagerObj> const_textlistlayoutmanagerptr;

struct listimpl_info_s;

//! Internal \ref textlistlayoutmanager "textlistlayoutmanager" mutex-protected data.

typedef mpobj<listimpl_info_s, std::recursive_mutex> listimpl_info_t;

//! A forward reference to a lock on a \ref listimpl_info_t "listimpl_info_t".

typedef mpobj_lock<listimpl_info_s, std::recursive_mutex> listimpl_info_lock_t;


//! A new list item.

//! Passed to \ref textlistlayoutmanager "textlistlayoutmanager"'s
//! insert_item(), and append_item(), to indicate a text or an image item.

typedef std::variant<text_param, image_param> list_item_param;

LIBCXXW_NAMESPACE_END

#endif
