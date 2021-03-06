/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_infofwd_h
#define popup_attachedto_infofwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN popup_attachedto_infoObj;

/*! Information needed to figure out where an attached popup is shown.

Stores information about an element a popup gets attached to. The stored
information is the element absolute position on the screen, and how its
popup is attached: above/below, or next to it.

This object is referenced by the top level handler object, which uses
it to physically position the popup, after which it schedules the top
level peephole's recalculation. This is also referenced by the peepholed
element, so that it can inform the peephole the maximum size of the popup
can reach, before scrollbars are necessary.

*/
typedef ref<popup_attachedto_infoObj> popup_attachedto_info;

LIBCXXW_NAMESPACE_END

#endif
