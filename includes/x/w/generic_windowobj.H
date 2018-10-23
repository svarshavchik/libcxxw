/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_windowobj_h
#define x_w_generic_windowobj_h

#include <x/w/generic_windowfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/containerobj.H>
#include <x/w/drawableobj.H>
#include <x/obj.H>
#include <x/logger.H>
#include <x/functionalrefptrfwd.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Generic window facilities.

//! \see generic_window

class generic_windowObj : public containerObj,
			  public drawableObj {

public:
	LOG_CLASS_SCOPE;

	class LIBCXX_HIDDEN implObj;
	class LIBCXX_HIDDEN handlerObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor
	generic_windowObj(const ref<implObj> &impl,
			  const layout_impl &lm)
		LIBCXX_HIDDEN;

	//! Destructor
	~generic_windowObj() LIBCXX_HIDDEN;

	using drawableObj::get_screen;

	//! Set the window's title
	void set_window_title(const std::string_view &);

	//! Set the window's title
	void set_window_title(const std::u32string_view &);

	//! Set the window's class
	void set_window_class(const std::string_view &instance,
			      const std::string_view &resource);

	//! Raise this window
	void raise();

	//! Lower this window
	void lower();

	//! Invoke this lambda if the server disconnects unexpectedly.

	//! \see connection

	void on_disconnect(const functionref<void ()> &callback);

	//! Whether a selection is available.

	//! This gets forwarded to this window's
	//! \ref connection "connection"'s selection_has_owner(), for
	//! convenience.
	bool selection_has_owner(const std::string_view &selection="SECONDARY")
		const;

	//! Whether a selection can be received right now.

	bool selection_can_be_received() const override;

	using containerObj::receive_selection;

	//! Receive the specified selection

	void receive_selection(ONLY IN_THREAD,
			       const std::string_view &selection="SECONDARY")
		override;
};

LIBCXXW_NAMESPACE_END

#endif
