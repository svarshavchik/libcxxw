/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextcursorlocation_H
#define richtextcursorlocation_H

#include "richtext/richtextcursorlocationfwd.H"
#include "richtext/richtextfragment.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN richtextcursorlocationObj : virtual public obj {

 public:

	//! Constructor
	richtextcursorlocationObj();

	//! Destructor
	~richtextcursorlocationObj() noexcept;

	//! Finish initializing.

	//! The public wrapper for this object is responsible for invoking
	//! initialize() to link this location to its fragment.

	void initialize(richtextfragmentObj *fragment,
			size_t offset);

	// Alternative entry point used in set().

	void initialize(richtextfragmentObj *fragment,
			size_t offsetArg,
			richtextfragmentObj::locations_t::iterator new_iter);

	//! Deinitialize this location

	//! The public wrapper for this object is responsible for invoking
	//! deinitialize(), in the public object's destructor.

	void deinitialize();

	//! Clone this cursor.

	//! This does not copy my_fragment and my_fragment_iter.
	//! This is taken care of in richtextcursorObj::clone().

	richtextcursorlocation clone() const;

	//! Convenience typedef
	typedef richtextfragmentObj::locations_t locations_t;

	//! The fragment this location is on.

	//! A nullptr indicates that this location is not fully initialized.
	//! This should never be visible externally.
	//!
	//! Fragments hold refs on their cursor locations. The entire rich text
	//! object is protected by a mutex lock. When the fragment gets
	//! destroyed, all of its locations are nullptred; by that time
	//! all locations should've been moved elsewhere. This native pointer,
	//! therefore, is always valid (so far).
	//!
	//! my_fragment_iter is this locations's iterator in
	//! fragment->locations.
	//!
	//! richtextfragmentObj's destructor nulls out my_fragment.
	//! split() and merge() move affect locations, and repoint them.

	richtextfragmentObj *my_fragment=nullptr;

	//! Where this location is in my_fragment->locations;
	locations_t::iterator my_fragment_iter;

	//! If set to true(), richtextfragmentObj::insert() does not adjust
	//! this location.

	bool do_not_adjust_in_insert=false;

	//! Where this location is positioned in the fragment.

	class position_t {

	public:
		//! This cursor is positioned on character text[offset].
		size_t offset=0;

		//! Actual horizontal offset of this cursor, in pixels.
		dim_squared_t horiz_pos=0;

		//! Targeted horiz_pos when the cursor is moving up/down
		dim_squared_t targeted_horiz_pos=0;

		//! Reset targeted_horiz_pos

		inline void reset_horiz_pos()
		{
			targeted_horiz_pos=horiz_pos;
		}

		//! At start of line

		inline void start_of_line()
		{
			offset=0;
			horiz_pos=0;
			targeted_horiz_pos=0;
		}
	};

	//! The position of this location in the fragment
	position_t position;

	//! This is a cursor location for the currently-visible cursor
	bool cursor_on=false;

	//! Reset horiz_pos to point exactly to the character.
	void update_horiz_pos();

	//! Reset offset and horiz pos to point to targeted_horiz_pos
	void update_offset(dim_squared_t targeted_horiz_pos);

	//! Ahead by 1
	void move(ssize_t howmuch);

	//! Beginning of fragment
	void start_of_line();

	//! End of fragment
	void end_of_line();
};

LIBCXXW_NAMESPACE_END

#endif
