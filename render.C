/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include <x/logger.H>
#include <x/exception.H>

#include "render.H"
#include "connectionfwd.H"
#include "connection_info.H"
#include "pictformat.H"

LIBCXXW_NAMESPACE_START

static inline depth_t num_bits(uint16_t mask)
{
	depth_t n{0};

	while (mask)
	{
		++n;
		mask >>= 1;
	}

	return n;
}

static inline std::vector<pictformatObj::color_index>
get_indexes(xcb_connection_t *conn,
	    const pictform_s &s, xcb_render_pictformat_t id)
{
	std::vector<pictformatObj::color_index> v;

	if (!s.indexed)
		return v;

	returned_pointer<xcb_generic_error_t *> error;

	auto iv=return_pointer(xcb_render_query_pict_index_values_reply
			       (conn, xcb_render_query_pict_index_values
				(conn, id), error.addressof()));

	if (!iv)
	{
		if (error)
			throw EXCEPTION(connection_error(error));

		throw EXCEPTION("QueryPictIndexValus request failed");
	}

	auto qi=xcb_render_query_pict_index_values_values_iterator(iv);

	v.reserve(qi.rem);

	while (qi.rem)
	{
		auto p=qi.data;

		v.emplace_back(p->pixel,
			       rgb(p->red,
				   p->green,
				   p->blue,
				   p->alpha));

		xcb_render_indexvalue_next(&qi);
	}

	return v;
}

LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::render::render,
		    initialize_renderLog);

render::render(const connection_info &info)
{
	LOG_FUNC_SCOPE(initialize_renderLog);

	returned_pointer<xcb_generic_error_t *> error;

	render_pict_formats=return_pointer
		(xcb_render_query_pict_formats_reply
		 (info->conn,
		  xcb_render_query_pict_formats(info->conn),
		  error.addressof()));

	if (!render_pict_formats)
	{
		if (error)
			throw EXCEPTION(connection_error(error));

		throw EXCEPTION("QueryPictFormats request failed");
	}

	auto i=xcb_render_query_pict_formats_formats_iterator(render_pict_formats);

	available_pictformats_t weak_available_pictformats=
		available_pictformats_t::create();

	while (i.rem)
	{
		auto p=i.data;

		pictform_s s={
			.indexed=p->type==XCB_RENDER_PICT_TYPE_INDEXED,
			.depth=depth_t(p->depth),
			.red_depth=num_bits(p->direct.red_mask),
			.green_depth=num_bits(p->direct.green_mask),
			.blue_depth=num_bits(p->direct.blue_mask),
			.alpha_depth=num_bits(p->direct.alpha_mask),
			.red_shift=p->direct.red_shift,
			.red_mask=p->direct.red_mask,
			.green_shift=p->direct.green_shift,
			.green_mask=p->direct.green_mask,
			.blue_shift=p->direct.blue_shift,
			.blue_mask=p->direct.blue_mask,
			.alpha_shift=p->direct.alpha_shift,
			.alpha_mask=p->direct.alpha_mask,
		};

		auto pf=pictformat::create
			(s, ref<pictformatObj::implObj>
			 ::create(info, weak_available_pictformats, p->id),
			 get_indexes(info->conn, s, p->id));

		weak_available_pictformats->push_back(pf);

		available_pictformats.insert({p->id, pf});

		xcb_render_pictforminfo_next(&i);
	}

	// Now, once we have available_pictformats, build
	// pictformats_by_screen_depth.

	auto s=xcb_render_query_pict_formats_screens_iterator(render_pict_formats);
	pictformats_by_screen_depth.reserve(s.rem);

	while (s.rem)
	{
		auto d=xcb_render_pictscreen_depths_iterator(s.data);

		pictformats_by_screen_depth.push_back(std::map<depth_t,
						      screen_depth_info>());

		auto &screen=*--pictformats_by_screen_depth.end();

		while (d.rem)
		{
			depth_t depth{d.data->depth};

			auto v=xcb_render_pictdepth_visuals_iterator(d.data);

			while (v.rem)
			{
				xcb_visualid_t visual=v.data->visual;
				xcb_render_pictformat_t format=v.data->format;

				auto iter=available_pictformats.find(format);

				if (iter == available_pictformats.end())
				{
					LOG_ERROR("Cannot find picture info");
				}
				else
				{
					screen[depth]
						.visual_to_pictformat
						.insert({visual, iter->second});
				}
				xcb_render_pictvisual_next(&v);
			}

			xcb_render_pictdepth_next(&d);
		}

		xcb_render_pictscreen_next(&s);
	}
}

// Return a standard format for an alpha-only pictformat of given depth.

const_pictformat
render::find_alpha_pictformat_by_depth(depth_t depth) const
{
	xcb_pict_standard_t s;

	switch (depth_t::value_type(depth)) {
	case 8:
		s=XCB_PICT_STANDARD_A_8;
		break;
	case 4:
		s=XCB_PICT_STANDARD_A_4;
		break;
	case 1:
		s=XCB_PICT_STANDARD_A_1;
		break;
	default:
		throw EXCEPTION("No standard format for"
				" alpha depth "
				<< depth);
	}

	return find_standard_format(s);
}

const_pictformat
render::find_standard_format(xcb_pict_standard_t s) const
{
	auto p=xcb_render_util_find_standard_format(render_pict_formats, s);

	if (!p)
		throw EXCEPTION("Standard rendering format not found");

	auto iter=available_pictformats.find(p->id);

	if (iter == available_pictformats.end())
		throw EXCEPTION("Standard rendering format cache lookup failure");
	return iter->second;
}

LIBCXXW_NAMESPACE_END
