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

#include <optional>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! Inherits from sharedlockObj.
//!
//! A non-IN_THREAD window save() acquires a shared lock before
//! invoking the IN_THREAD version. A screen_positions save() acquires
//! an unique lock.

class screen_positionsObj::implObj : public sharedlockObj {

 public:

	//! Internal XML data.
	const xml::doc data;

	//! Default constructor. An empty list of memorized screen positions.

	implObj();

	//! Destructor
	~implObj();

	//! Load memorized screen positions from a file.

	//! Construct a new instance and initialize it with the contents
	//! of the specified file.
	implObj(const std::string &filename);


	//! Find a memorized position.

	//! \internal
	std::optional<main_window_config::window_info_t
		      > find(const std::string_view &identifier)
		const LIBCXX_HIDDEN;

	//! Create a new node when saving something.

	xml::writelock
	create_writelock_for_saving(const std::string_view &type,
				    const std::string_view &name);

	//! Save all memorized positions in a file.
	void save(const std::string &filename) const;
};

//! Internal Xpath assembler.

//! \internal

std::string saved_element_to_xpath(const std::string_view &type,
				   const std::string_view &name) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
