/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef glyphset_H
#define glyphset_H

#include <x/sipobj.H>
#include "xid_t.H"
#include "screen.H"
#include "connection.H"
#include "pictformat.H"
#include "fonts/scanline.H"

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <xcb/xcb.h>
#include <xcb/render.h>

LIBCXXW_NAMESPACE_START

//! Manage a glyphset.

//! This is a wrapper for RENDER's GlyphSet. The object implements loading
//! glyphs into the GlyphSet.
//!
//! The constructor takes the glyphset's pictformat. To add glyphs, call
//! add_glyphs(), which returns a handle that holds an exclusive lock on the
//! glyphset, and is used to add more glyphs to the glyphset.

class LIBCXX_HIDDEN glyphsetObj : public xidObj {

 public:
	//! My pictformat
	const const_pictformat pictformat;

	//! My connection
	const ref<connectionObj::implObj> connection_impl;

	//! RENDER GlyphSet id.
	inline xcb_render_glyphset_t glyphset_id() const { return id_; }

	//! Constructor
	glyphsetObj(const const_screen &screenArg,
		    const const_pictformat &pictformatArg);

	//! Destructor
	~glyphsetObj();

 private:

	// Container for loaded glyphs.

	typedef sipobj<std::unordered_map<uint32_t, xcb_render_glyphinfo_t>> glyphs_t;

	//! Which glyphs have been loaded into this glyphset.

	glyphs_t loaded_glyphs;

	//! Glyphs in the process of being added to a glyphset.

	class addObj : virtual public obj {

		//! My glyphset.
		const ref<glyphsetObj> glyphset;

		//! Exclusive locks.
		glyphs_t::writelock writelock;

		//! New glyphs that have been created, but not yet added to loaded_glyphs.
		std::vector<uint32_t> new_glyphs;
		//! New glyphs that have been created, but not yet added to loaded_glyphs.
		std::vector<xcb_render_glyphinfo_t> new_glyphinfos;

		//! A lookup map for new_glyphs.
		std::unordered_set<uint32_t> loaded;

		//! Raw data that has been created, but not yet added.
		std::vector<uint8_t> data;

		//! Calculated in the constructor.
		const uint32_t bytes_per_glyph;

		//! Calculated in the constructor, for optimum buffering.
		const uint16_t max_glyphs_to_do;

		//! Flag to create all buffers when the first glyph gets added.
		bool allocate;

		//! Flush buffered glyphs.

		//! Actually calls xcb_render_add_glyphs(). Called from the
		//! destructor, and when the internal buffers are full.
		void done();
	public:
		//! Constructor

		//! Sizes the buffers based on the estimated glyph size.
		addObj(const ref<glyphsetObj> &glyphsetArg,
		       dim_t estimated_glyph_width,
		       dim_t estimated_glyph_height);

		//! Destructor
		~addObj() noexcept;

		//! Must call ready_to_add_glyph() before calling add_glyph().

		bool ready_to_add_glyph(uint32_t glyph_index);

		template<typename get_pixelrow_lambda>
		void add_glyph(uint32_t glyph_index,
			       const xcb_render_glyphinfo_t &glyphinfo,
			       get_pixelrow_lambda &&get_pixelrow);
	};

 public:

	//! Begin the process of loading glyphs into this glyphset.

	ref<addObj> add_glyphs(dim_t estimated_glyph_width,
			       dim_t estimated_glyph_height)
	{
		return ref<addObj>::create(ref<glyphsetObj>(this),
					   estimated_glyph_width,
					   estimated_glyph_height);
	}

	//! Access font's defined glyphs' information

	class get_loaded_glyphs : public glyphs_t::readlock {

	public:
		get_loaded_glyphs(glyphsetObj &glyphset)
			: glyphs_t::readlock(glyphset.loaded_glyphs)
		{
		}

		using glyphs_t::readlock::operator *;
		using glyphs_t::readlock::operator ->;
	};
};

//! Add new glyphs toi the glyphset.

//! This is used by \ref freetypefont "freetypefont" to make sure that
//! the required glyphs are loaded. The overall approach here is:
//!
//! Call add_glyphs() to add new glyphs to the glyphset. Pass the
//! estimated width and height of each glyph. This is used to size
//! the arrays, then:
//!
//! Call ready_to_add_glyph(), passing the new glyph's index.
//! ready_to_add_glyph() returns false if the glyph is already loaded,
//! else it returns true. If so, proceed and:
//!
//! Call add_glyph(), passing:
//! 1. The glyph index
//! 2. Glyphs' xcb_render_glyphinfo_t
//! 3. A lambda that gets called for each row in the glyph's bitmap.
//! The lambda receives a size_t parameter that starts at 0, and counts
//! up to glyph's height, less 1.
//! 4. The lambda should return a second lambda, that's going to be
//! called with a parameter that starts at 0, and counts up to the
//! glyph's width. The second lambda should return the value of the
//! pixel, from the bitmap's row and column.

template<typename get_pixelrow_lambda>
void glyphsetObj::addObj::add_glyph(uint32_t glyph_index,
				    const xcb_render_glyphinfo_t &glyphinfo,
				    get_pixelrow_lambda &&get_pixelrow)
{
	if (allocate)
	{
		// First time, allocate the arrays.

		new_glyphs.reserve(max_glyphs_to_do);
		new_glyphinfos.reserve(max_glyphs_to_do);
		data.reserve(bytes_per_glyph * max_glyphs_to_do);
		allocate=false;
	}
	// Figure out the buffer for each scanline.

	const auto &setup=glyphset->connection_impl->setup;
	auto depth=glyphset->pictformat->depth;
	auto pad=scanline_sizeof(glyphinfo.width, depth, setup);

	uint8_t buffer[pad];

	// Get each scanline for this glyph, append it to data.
	for (decltype(glyphinfo.height) y=0; y<glyphinfo.height; ++y)
	{
		scanline(buffer, glyphinfo.width, depth, setup,
			 get_pixelrow(y));
		data.insert(data.end(), buffer, buffer+pad);
	}

	// Pad each glyph to 32 bit boundary
	while (data.size() % 4)
		data.push_back(0);

	new_glyphs.push_back(glyph_index);
	new_glyphinfos.push_back(glyphinfo);
	loaded.insert(glyph_index);

	if (new_glyphinfos.size() >= max_glyphs_to_do)
		done();
}

LIBCXXW_NAMESPACE_END

#endif
