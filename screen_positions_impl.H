/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_positions_impl_h
#define screen_positions_impl_h

#include "x/w/screen_positions.H"
#include "x/w/main_window.H"
#include "x/w/impl/screen_positions_confighandle.H"

#include <x/xml/doc.H>
#include <x/xml/readlock.H>
#include <x/xml/writelock.H>
#include <x/sharedlock.H>
#include <x/weakunordered_multimap.H>

#include <optional>
#include <string>
#include <string_view>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Restored window position.

struct window_position_t {
	std::optional<size_t> screen_number;
	rectangle coordinates;
};

//! URI identifying widgets belonging to this library.
extern const std::string_view libcxx_uri LIBCXX_HIDDEN;

class screen_positionsObj::widget_typeObj : virtual public obj {

public:
	const std::string ns;
	const std::string type;

	widget_typeObj(const std::string &ns,
		       const std::string &type)
		: ns{ns}, type{type}
	{
	}
};

struct LIBCXX_HIDDEN widget_type_hash {

	size_t operator()(const std::tuple<std::string, std::string> &o) const
	{
		std::hash<std::string> h;

		return h(std::get<0>(o)) + h(std::get<1>(o));
	}
};

//! Find a memorized position.

//! \internal
std::optional<window_position_t> find_window_position(
	const screen_positions_handle &config_handle
) LIBCXX_HIDDEN;

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

	friend class config_handleObj;

	//! Load memorized screen positions from a file.

	//! Construct a new instance and initialize it with the contents
	//! of the specified file.
	implObj(const std::string &filename,
		const std::string &version);

	//! Destructor
	~implObj();

	//! Create a new node when saving something.

	screen_positions_handle config_handle(
		const std::vector<std::string> &window_path,
		const std::string_view &ns,
		const std::string_view &type,
		const std::string_view &name
	);

	//! Save all memorized positions in a file.
	void save();

	//! Container for current_main_window_handlers

	typedef weakunordered_multimap<std::string,
				       main_windowObj::handlerObj
				       > current_main_window_handlers_t;

	//! Current main window handlers

	//! Enforce unique main window ids. Each main_windowObj::handlerObj
	//! inserts itself under its id. If it exists already an exception
	//! gets thrown.

	const current_main_window_handlers_t current_main_window_handlers;

	//! Container for a cache of widget types

	//! Prepare to support many instances of the same widget referencing
	//! the same namespace and type. To avoid creating all these strings
	//! we should be able to use this cache efficiently.

	typedef weakunordered_multimap<std::tuple<std::string, std::string>,
				       widget_typeObj,
				       widget_type_hash> widget_type_cache_t;

	//! A cache of widget types.

	const widget_type_cache_t widget_type_cache;
};

LIBCXXW_NAMESPACE_END

#endif
