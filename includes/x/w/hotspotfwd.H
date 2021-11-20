/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_hotspotfwd_h
#define x_w_hotspotfwd_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC hotspotObj;

//! Signature for a hotspot activation function.

typedef functionref<void (THREAD_CALLBACK,
			  const callback_trigger_t &,
			  const busy &)> hotspot_callback_t;

//! \anchor hotspot A widget that can be activated.

//! A widget that can be activated, in some way, with a pointer
//! button or ENTER or SPACE.
//!
//! \code
//! element->on_activate([]
//!                     (ONLY IN_THREAD,
//!                      const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &,
//!                      const INSERT_LIBX_NAMESPACE::w::busy &)
//!                     {
//!                     });
//!
//! element->on_activate(IN_THREAD,
//!                     []
//!                     (ONLY IN_THREAD,
//!                      const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &,
//!                      const INSERT_LIBX_NAMESPACE::w::busy &)
//!                     {
//!                     });
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

//! A nullable pointer reference to an \ref hotspot "activatable widget".

typedef ptr<hotspotObj> hotspotptr;

//! A reference to a constant \ref hotspot "activatable widget".

typedef const_ref<hotspotObj> const_hotspot;

//! A nullable pointer reference to a constant \ref hotspot "activatable widget".

typedef const_ptr<hotspotObj> const_hotspotptr;

//! Current temperature of an \ref hotspot "activatable widget".

enum class temperature {

	//! Pointer not inside the hotspot, hotspot does not have input focus, nothing is pressed
	cold,

	//! Pointer is inside the hotspot, or it has input focus, nothing is pressed
	warm,

	//! Space, enter, or button 1 is pressed
	hot};


LIBCXXW_NAMESPACE_END

#endif
