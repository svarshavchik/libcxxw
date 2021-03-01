/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_connectionobj_h
#define x_w_connectionobj_h

#include <x/w/connectionfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/pictformatfwd.H>
#include <x/obj.H>
#include <x/functionalrefptrfwd.H>
#include <vector>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! A connection to the server

//! \see connection

class connectionObj : virtual public obj {

 public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor
	connectionObj(const std::string_view &display=
		      std::string_view());

	//! Destructor
	~connectionObj() LIBCXX_HIDDEN;

	//! Numbers of screens on this display.
	size_t screens() const;

	//! Which screen is the default one.
	size_t default_screen() const;

	//! Returns the mcguffin for the underlying display server connection.
	ref<obj> mcguffin() const;

	//! Invoke this lambda if the server disconnects unexpectedly.

	void on_disconnect(const functionref<void ()> &callback);

	//! Returns the current theme and scaling factor.

	std::tuple<std::string, int, enabled_theme_options_t
		   > current_theme() const;

	//! Sets a new theme and scaling factor.

	void set_theme(//! Called only in the connection thread
		       ONLY IN_THREAD,
		       //! Theme name
		       const std::string &identifier,
		       //! Scaling factor
		       int factor,
		       //! Theme options
		       const enabled_theme_options_t &enabled_options,

		       //! Update this connection's theme only. Don't save it.
		       bool this_connection_only,

		       //! Which settings were updated

		       //! Settings that begin with underscore do not affect
		       //! the visual appearance and won't trigger theme
		       //! updated processing.
		       const std::unordered_set<std::string> &updated_settings);

	//! Sets and saves new theme and scaling factor.

	void set_and_save_theme(ONLY IN_THREAD,
				const std::string &identifier,
				int factor,
				const enabled_theme_options_t &enabled_options);


	//! Shortcut to render information.
	const_pictformat find_alpha_pictformat_by_depth(depth_t d) const;

	//! Invoke the callback in the execution thread.

	void in_thread(const functionref<void (THREAD_CALLBACK)> &cb) const;

	//! Invoke the callback in the execution thread, after all other callbacks.

	void in_thread_idle(const functionref<void (THREAD_CALLBACK)> &cb)
		const;

	//! Whether a selection is available.

	//! Returns an indication whether there's an owner of the specified
	//! selection on the server.
	bool selection_has_owner(const std::string_view &selection)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
