/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pictformat.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

pictformatObj::color_index::color_index(uint32_t indexArg, rgb &&colorArg)
	: index(indexArg), color(std::move(colorArg))
{
}

pictformatObj::color_index::~color_index()
{
}

pictformatObj::pictformatObj(const pictform_s &s, ref<implObj> &&implArg,
			     std::vector<color_index> &&color_indexesArg)
	: pictform_s(s), impl(std::move(implArg)),
	  color_indexes(std::move(color_indexesArg))
{
}

pictformatObj::~pictformatObj()
{
}


pictformatObj::implObj::implObj(const connection_info &info,
				const available_pictformats_t
				&available_pictformats,
				xcb_render_pictformat_t idArg)
	: info(info), id(idArg), available_pictformats(available_pictformats)
{
}

pictformatObj::implObj::~implObj()
{
}

bool pictformatObj::rgb_compatible(const const_pictformat &format) const
{
	// See rgb_matches().

	if (indexed != format->indexed)
		return false;

	if (indexed)
		return depth == format->depth;

	return red_mask == format->red_mask &&
		red_shift == format->red_shift &&
		green_mask == format->green_mask &&
		green_shift == format->green_shift &&
		blue_mask == format->blue_mask &&
		blue_shift == format->blue_shift;
}

std::list<const_pictformat> pictformatObj::compatible_pictformats()
	const
{
	std::list<const_pictformat> formats;

	for (auto &f:*impl->available_pictformats)
	{
		auto p=f.getptr();

		if (!p)
			continue; // Shouldn't happen.

		if (p->impl->id == impl->id)
			continue;

		if (rgb_compatible(p))
			formats.push_back(p);
	}

	return formats;
}

const_pictformat pictformatObj::compatible_pictformat_with_alpha_channel()
	const
{
	auto format=const_pictformat(this);

	auto pictformats=compatible_pictformats();

	for (const auto &candidate:pictformats)
	{
		if (candidate->alpha_depth > format->alpha_depth)
                        format=candidate;
	}

	return format;
}

LIBCXXW_NAMESPACE_END
