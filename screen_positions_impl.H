/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_positions_impl_h
#define screen_positions_impl_h

#include "x/w/screen_positions.H"
#include "x/w/main_window.H"

#include <x/xml/doc.H>
#include <x/xml/writelock.H>
#include <x/sharedlock.H>
#include <x/weakmultimap.H>
#include <x/mpobj.H>

#include <optional>
#include <string_view>
#include <unordered_set>

LIBCXXW_NAMESPACE_START

//! Restored window position.

struct window_position_t {
	std::optional<size_t> screen_number;
	rectangle coordinates;
};

//! Internal implementation object.

//! Inherits from sharedlockObj.
//!
//! A non-IN_THREAD window save() acquires a shared lock before
//! invoking the IN_THREAD version. A screen_positions save() acquires
//! an unique lock.

class screen_positionsObj::implObj : public sharedlockObj {

 public:

	//! App id
	const std::string appid;

	//! Filename
	const std::string filename;

	//! Version
	const std::string version;

	//! Internal XML data.
	const xml::doc data;

	//! Load memorized screen positions from a file.

	//! Construct a new instance and initialize it with the contents
	//! of the specified file.
	implObj(const std::string &filename,
		const std::string &version);

	//! Destructor
	~implObj();

	//! Find a memorized position.

	//! \internal
	std::optional<window_position_t> find_window_position(
		const std::string_view &identifier
	) const;

	//! Create a new write lock.

	//! Creates the root <windows> element, for an empty document.

	xml::writelock create_writelock();

	//! Create a new node when saving something.

	xml::writelock
	create_writelock_for_saving(const std::string_view &type,
				    const std::string_view &name);

	//! Save all memorized positions in a file.
	void save();

	//! Container for current_main_window_handlers

	typedef weakmultimap<std::string,
			     main_windowObj::handlerObj
			     > current_main_window_handlers_t;

	//! Current main window handlers

	//! Enforce unique main window ids. Each main_windowObj::handlerObj
	//! inserts itself under its id. If it exists already an exception
	//! gets thrown.

	const current_main_window_handlers_t current_main_window_handlers;
};

//! Internal Xpath assembler.

//! \internal

std::string saved_element_to_xpath(const std::string_view &type,
				   const std::string_view &name) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
