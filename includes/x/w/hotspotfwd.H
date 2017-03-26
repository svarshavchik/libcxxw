/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_hotspotfwd_h
#define x_w_hotspotfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

#include <functional>

LIBCXXW_NAMESPACE_START

class hotspotObj;

//! Signature for a hotspot activation function.

typedef std::function<void ()> hotspot_callback_t;

//! A display element that can be activated.

//! A display element that can be activated, in some way, with a pointer
//! button or ENTER or SPACE.
//!
//! \code
//! element->on_activate([]
//!                      {
//!                      });
//! \endcode
//!
//! Installs a callback that gets invoked when the hotspot is activated.
//! Only one callback can be installed at a time. A new callback replaces
//! the previously installed one. To uninstall the current callback,
//! install another one thta does nothing.
//!
//! \note
//! The callbacks get invoked by an internal execution thread, and
//! installation of a new callback can happen while the existing one is
//! being executed. Furthermore, even after on_activate() returns it's
//! possible for the previous callback to be executed just after
//! on_activate() returns, before the new callback takes effect for all
//! activations in the future.

typedef ref<hotspotObj> hotspot;

//! A nullable pointer reference to an activatable display element.

//! \see hotspot

typedef ptr<hotspotObj> hotspotptr;

//! A reference to a constant activatable display element.

//! \see hotspot

typedef const_ref<hotspotObj> const_hotspot;

//! A nullable pointer reference to a constant activatable display element.

//! \see hotspot

typedef const_ptr<hotspotObj> const_hotspotptr;

//! Current temperature of a \ref hotspot "hot spot".

enum class temperature {

	//! Pointer not inside the hotspot, hotspot does not have input focus, nothing is pressed
	cold,

	//! Pointer is inside the hotspot, or it has input focus, nothing is pressed
	warm,

	//! Space, enter, or button 1 is pressed
	hot};


LIBCXXW_NAMESPACE_END

#endif
