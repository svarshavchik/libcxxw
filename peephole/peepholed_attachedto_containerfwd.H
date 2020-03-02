/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholed_attachedto_containerfwd_h
#define x_w_peepholed_attachedto_containerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN peepholed_attachedto_containerObj;

/*! Container for a popup that's attached to a display element.

The date input field and the color picker, for example, have an attached
popup.

This display element is peepholed by the popup's top-level peephole, and
thusly implements the peepholed mixin, using a reference font to set the
(mostly unused) top level scroll increments.

*/

typedef ref<peepholed_attachedto_containerObj> peepholed_attachedto_container;

//! A constant \ref peepholed_attachedto_container "attached popup container".

//! \see peepholed_attachedto_container

typedef const_ref<peepholed_attachedto_containerObj> const_peepholed_attachedto_container;

//! A nullable pointer reference to a \ref peepholed_attachedto_container "attached popup container".

//! \see peepholed_attachedto_container

typedef ptr<peepholed_attachedto_containerObj> peepholed_attachedto_containerptr;

//! A nullable pointer reference to a const \ref peepholed_attachedto_container "attached popup container".

//! \see peepholed_attachedto_container

typedef const_ptr<peepholed_attachedto_containerObj> const_peepholed_attachedto_containerptr;

LIBCXXW_NAMESPACE_END

#endif
