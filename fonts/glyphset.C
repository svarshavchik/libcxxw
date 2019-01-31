/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/glyphset.H"
#include "fonts/scanline.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

glyphsetObj::glyphsetObj(const const_screen &screenArg,
			 const const_pictformat &pictformatArg)
	: xidObj(screenArg->impl->thread),
	  pictformat(pictformatArg),
	  connection_impl(screenArg->get_connection()->impl)
{
	xcb_render_create_glyph_set(conn()->conn, id_, pictformat->impl->id);
}

glyphsetObj::~glyphsetObj()
{
	xcb_render_free_glyph_set(conn()->conn, id_);
}


glyphsetObj::addObj::addObj(const ref<glyphsetObj> &glyphsetArg,
			    dim_t estimated_glyph_width,
			    dim_t estimated_glyph_height)
	: glyphset(glyphsetArg), writelock(glyphset->loaded_glyphs),
	  bytes_per_glyph(dim_t::value_type
			  (scanline_sizeof(estimated_glyph_width,
					   glyphset->pictformat->depth,
					   glyphset->connection_impl->setup))),
	  max_glyphs_to_do(1+0xfffe/dim_t::value_type(estimated_glyph_height)
			   /bytes_per_glyph),
	  allocate(true)
{
}

glyphsetObj::addObj::~addObj() noexcept
{
	done();
}

void glyphsetObj::addObj::done()
{
	if (new_glyphs.empty())
		return;

	xcb_render_add_glyphs(glyphset->conn()->conn,
			      glyphset->glyphset_id(),
			      new_glyphs.size(),
			      &new_glyphs[0],
			      &new_glyphinfos[0],
			      data.size(),
			      data.size() == 0 ? nullptr:&data[0]);

	// Ok, record the saved glyphs' information

	for (auto i=new_glyphs.size(); i; )
	{
		--i;
		writelock->erase(new_glyphs[i]);
		writelock->insert({new_glyphs[i], new_glyphinfos[i]});
	}

	new_glyphs.clear();
	new_glyphinfos.clear();
	data.clear();
	loaded.clear();
}

bool glyphsetObj::addObj::ready_to_add_glyph(uint32_t glyph_index)
{
	// Check if: this glyph is already loaded, or
	// it's already queued to be loaded.

	return writelock->find(glyph_index) == writelock->end() &&
		loaded.find(glyph_index) == loaded.end();
}

LIBCXXW_NAMESPACE_END
