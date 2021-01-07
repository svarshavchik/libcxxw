/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef installed_shortcutobj_h
#define installed_shortcutobj_h

#include "shortcut/installed_shortcutfwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include "activated_in_threadfwd.H"
#include "activated_in_threadobj.H"
#include "x/w/shortcut.H"
#include <x/weakptr.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see installed_shortcut

class LIBCXX_HIDDEN installed_shortcutObj : public activated_in_threadObj {

	//! The shortcut for this hotspot
	shortcut installed_shortcut_thread_only;

	//! What we should activate.
	const weakptr<activated_in_threadptr> activate;

public:
	THREAD_DATA_ONLY(installed_shortcut);

	//! Constructor
	installed_shortcutObj(const shortcut &installed_shortcut,
			      const activated_in_thread &what_to_activate);

	//! Destructor
	~installed_shortcutObj();

	//! Implement activated().

	//! Forwarded to the real activate().
	void activated(ONLY IN_THREAD,
		       const callback_trigger_t &trigger) override;

	//! Implement enabled().

	//! Forwarded to the real enabled().
	bool enabled(ONLY IN_THREAD) override;
};

LIBCXXW_NAMESPACE_END

#endif
