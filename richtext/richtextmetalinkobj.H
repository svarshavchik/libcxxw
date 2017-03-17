/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextmetalinkobj_H
#define richtextmetalinkobj_H

#include <x/obj.H>
#include "x/w/rgbfwd.H"
#include "richtext/richtextmetafwd.H"
#include <unordered_set>

LIBCXXW_NAMESPACE_START

//! Optional link, of some kind, from some portion of a rich text fragment.

class richtextmetalinkObj : virtual public obj {

public:

	//! Constructor
	richtextmetalinkObj();

	//! Destructor
	~richtextmetalinkObj() noexcept;

	//! Override markup for rendering text.

	//! override_text_markup() may leave the markup unchanged, or
	//! override its values.

	virtual void override_text_markup(richtextmeta &markup) const=0;

	//! Possible reasons that event() was invoked.

	enum class event_t { selected_event };

	//! Label object would like to report that something has happened.

	virtual void event(event_t) const=0;

	//! Shortcut for invoking event(selected_event_t)

	inline void selected()
	{
		event(event_t::selected_event);
	}
};

LIBCXXW_NAMESPACE_END

#endif
