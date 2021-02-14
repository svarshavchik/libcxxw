/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextcursorlocation_H
#define richtextcursorlocation_H

#include "x/w/input_field_lock.H"
#include "x/w/impl/richtext/richtextcursorlocationfwd.H"
#include "x/w/impl/richtext/richtextiteratorfwd.H"
#include "richtext/richtextfragment.H"

LIBCXXW_NAMESPACE_START

//! \ref richtextcursorlocation "richtextcursorlocation" implementation object.

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
			size_t offset,
			new_location location_option);

	// Alternative entry point used in set().

	void initialize(richtextfragmentObj *fragment,
			size_t offsetArg,
			fragment_cursorlocations_t::iterator new_iter,
			new_location location_option);

	//! Alternative initialization used by clone().
	void initialize(const richtextcursorlocation &clone);

	//! Deinitialize this location

	//! The public wrapper for this object is responsible for invoking
	//! deinitialize(), in the public object's destructor.

	void deinitialize();

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
	fragment_cursorlocations_t::iterator my_fragment_iter;

	//! If set to true(), richtextfragmentObj::insert() does not adjust
	//! this location.

	bool do_not_adjust_in_insert=false;

	//! Where this location is positioned in the fragment.

	class position_t {

	public:
		//! This cursor is positioned on character text[offset].
		size_t offset=0;

		//! Whether horiz_pos is valid.
		bool horiz_pos_is_valid=false;

		//! Actual horizontal offset of this cursor, in pixels.
		dim_squared_t cached_horiz_pos=0;

		//! Targeted horiz_pos when the cursor is moving up/down
		dim_squared_t targeted_horiz_pos=0;

		//! Reset targeted_horiz_pos

		inline void set_targeted_horiz_pos(dim_squared_t p)
		{
			targeted_horiz_pos=p;
		}
	};

 private:
	//! The position of this location in the fragment
	position_t position;

	//! Set a new targeted horiz pos for the current actual cursor location.
	void new_targeted_horiz_pos(ONLY IN_THREAD);

	//! Whether this is right-to-left text.
	bool rl() const;

	//! What will move() this location one position to the left.

	ssize_t moveleft_howmuch() const;

	//! What will move() this location one position to the right.

	ssize_t moveright_howmuch() const;

 public:
	//! What is says.
	inline void horiz_pos_no_longer_valid()
	{
		position.horiz_pos_is_valid=false;
	}

	//! Return our offset.
	inline auto get_offset() const
	{
		return position.offset;
	}

	//! Return our position
	size_t pos(get_location location_option) const;

	//! After a split(), subtract from offset.

	//! \internal
	inline void split_from_fragment(size_t howmuchremoved)
	{
		position.offset -= howmuchremoved;
		horiz_pos_no_longer_valid();
	}

	//! After a merge(), there are more characters before us.

	//! \internal
	inline void merged_from_fragment(size_t howmanymore)
	{
		position.offset += howmanymore;
		horiz_pos_no_longer_valid();
	}

	//! There were nchars characters removed starting at position #pos.

	//! We are advised, so, by richtextfragment. Adjust ourselves
	//! accordingly.

	void removed_from_fragment(size_t pos, size_t nchars);

	//! This is a cursor location for the currently-visible cursor
	bool cursor_on=false;

	//! Reset horiz_pos to point exactly to this location.

	//! If it is not already cached.
	void cache_horiz_pos(ONLY IN_THREAD);

	//! Return the current "targeted" horizontal position.
	dim_squared_t get_targeted_horiz_pos(ONLY IN_THREAD);

	//! Reset offset and horiz pos to point to targeted_horiz_pos
	void set_targeted_horiz_pos(ONLY IN_THREAD,
				    dim_squared_t targeted_horiz_pos);

	//! Return the current horizontal position

	dim_t get_horiz_pos(ONLY IN_THREAD);

	//! Adjust location by character count.
	void move(ONLY IN_THREAD, ssize_t howmuch);

	//! Adjust location to delete the character under the cursor.

	//! Called from the editor to advance the cursor in order to select
	//! the character under the cursor.
	void move_for_delete(ONLY IN_THREAD);

 private:
	void leftby1(ONLY IN_THREAD);
	void rightby1(ONLY IN_THREAD);
 public:
	//! Beginning of fragment
	void start_of_line();

	//! End of fragment
	void end_of_line();

	//! Up
	void up(ONLY IN_THREAD);

	//! Down
	void down(ONLY IN_THREAD);

	//! Up
	void page_up(ONLY IN_THREAD, dim_t height);

	//! Down
	void page_down(ONLY IN_THREAD, dim_t height);

	//! Move to the given coordinates

	//! Returns true if the X coordinate lies within the fragment.
	//! If it is outside the fragment, the location is moved to the first
	//! or the last character.
	bool moveto(ONLY IN_THREAD, coord_t x, coord_t y);

	//! richtextfragment inserted nchars at offset #pos, with extra_width pixels

	void inserted_at(ONLY IN_THREAD,
			 size_t pos, size_t nchars, dim_t extra_width);

	//! The same location?
	bool same(const richtextcursorlocationObj &b) const;

	//! Compare locations.

	//! Returns 0 if the two cursors point to the same spot.
	//! Returns 1 or -1 if the two cursors point to the same fragment,
	//! but they're not the same. Returns >1 or <-1 if the two cursors
	//! point to different fragments. Basically, this is the number
	//! of fragments between the two cursors, inclusively.

	std::ptrdiff_t compare(const richtextcursorlocationObj &b) const;

	//! Flip the location on its fragment to the opposite side.

	//! Does nothing with a left-to-right paragraph embedding level.
	//!
	//! Otherwise updates this location position on the line accordingly.
	//! This is used when the cursor is moved to a logical character
	//! position. Well, if it winds up at the beginning of the line, it
	//! should really be at the end of the line, and vice-versa, for
	//! right-to-left paragraph embedding level.
	void mirror_position(internal_richtext_impl_t::lock &must_be_locked);

	//! Move this location to some other fragment entirely.

	void reposition(const richtextfragment &new_fragment,
			size_t new_pos);
};

#define richtextcursorlocation_H_defined
#include "richtext/richtextcursorlocationparagraphtext.H"

LIBCXXW_NAMESPACE_END

#endif
