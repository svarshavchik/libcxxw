/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_draw_info_H
#define richtext_draw_info_H

#include "x/w/impl/richtext/richtext_draw_infofwd.H"
#include "richtext/richtext_alteration_configfwd.H"
#include "x/w/richtext/richtextiterator.H"

LIBCXXW_NAMESPACE_START

//! Additional information to \ref richtext "richtext"'s drawing method.

//! The additional information consists of which portion of the rich text
//! is "selected". This is used by the \ref editorObj::implObj "editor"
//! display element, to show the current selection, naturally.
//!
//! A selection is in place only when neither selection_start and
//! selection_end are null. The editor display element passes the current
//! selection_start, which may be null, and the current cursor for
//! selection_end, which is never null. Hence, when selection_start is not
//! null, there's a selected area to draw.
//!
//! The draw_as_disabled flag, if set, draws the label as if it was disabled.
//! This is used by listlayoutmanager, which simulates individual
//! disabled list items.

struct LIBCXX_HIDDEN richtext_draw_info {

	const richtext_alteration_config &richtext_alteration;

	richtextiteratorptr selection_start;
	richtextiteratorptr selection_end;
	bool draw_as_disabled=false;
};

LIBCXXW_NAMESPACE_END;
#endif
