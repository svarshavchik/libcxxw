/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef installed_shortcutfwd_h
#define installed_shortcutfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN installed_shortcutObj;

/*! An installed and active shortcut key.

The \ref shortcut_activation_elementObj "shortcut_activation_elementObj"
mixin installs this object into \ref generic_windowObj::handlerObj "window's"
installed_shortcut mixin, representing an active shortcut.

If a key press event is not otherwise handled, handle_key_event() finds the
best matching shortcut that's still enabled(), and activated()-s it.

*/

typedef ref<installed_shortcutObj> installed_shortcut;

//! A constant \ref installed_shortcut "installed shortcut key".

//! \see installed_shortcut

typedef const_ref<installed_shortcutObj> const_installed_shortcut;

//! A nullable pointer reference to a \ref installed_shortcut "installed shortcut key".

//! \see installed_shortcut

typedef ptr<installed_shortcutObj> installed_shortcutptr;

//! A nullable pointer reference to a const \ref installed_shortcut "installed shortcut key".

//! \see installed_shortcut

typedef const_ptr<installed_shortcutObj> const_installed_shortcutptr;


//! A container installed shortcuts.

//! This map is kept in the top level generic window object handler.
//!
//! shortcut_activation_elementObj mixin registers an
//! \ref installed_shortcut "installed_shortcut" here.

typedef std::unordered_multimap<char32_t, installed_shortcut> shortcut_lookup_t;

LIBCXXW_NAMESPACE_END

#endif
