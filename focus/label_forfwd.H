/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_forfwd_h
#define label_forfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class label_forObj;

//! A link before a focusable, and its label.

//! Pointer clicks on a label get forwarded to its focusable, as if the
//! focusable itself was clicking, with the same results.
//!
//! The labeled element's enabled flag also tracks its focusable's enabled
//! flag.
//!
//! A strong reference to this link is owned by the label. The focusable keeps
//! a weak list of existing links from all labels for the focusable.

typedef ref<label_forObj> label_for;

//! A nullable pointer reference to a \ref label_for "label link".

typedef ptr<label_forObj> label_forptr;

//! A reference to a constant \ref label_for "label link".

typedef const_ref<label_forObj> const_label_for;

//! A nullable pointer reference to a constant \ref label_for "label link".

typedef const_ptr<label_forObj> const_label_forptr;

LIBCXXW_NAMESPACE_END

#endif
