/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_positions_impl_h
#define screen_positions_impl_h

#include "x/w/screen_positions.H"
#include "x/w/rectangle.H"

#include <x/xml/doc.H>
#include <optional>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class screen_positionsObj::implObj : virtual public obj {

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


	//! Internal structure

	//! \internal
	struct LIBCXX_HIDDEN window_info {
		xml::doc::base::readlock config;
		std::optional<size_t> screen_number;
		rectangle coordinates;
	};

	//! Find a memorized position.

	//! \internal
	std::optional<window_info> find(const std::string_view &identifier)
		const LIBCXX_HIDDEN;

	//! Create a new node when saving something.

	xml::doc::base::writelock
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
