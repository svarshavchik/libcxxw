/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen_depthinfo.H"
#include "messages.H"
#include "x/w/pictformat.H"

#include <x/vector.H>

LIBCXXW_NAMESPACE_START

// Metadata describing the available depths for each screen.

screenObj::visualObj::implObj::implObj(xcb_visualid_t visual_id)
	: visual_id(visual_id)
{
}

screenObj::visualObj::implObj::~implObj() noexcept=default;



//! Screen depth visual information

screenObj::visualObj::visualObj(const ref<implObj> &impl,
				const const_pictformat &render_format,
				visual_class_t visual_class_type,
				uint8_t bits_per_rgb,
				uint16_t colormap_entries,
				uint32_t red_mask,
				uint32_t green_mask,
				uint32_t blue_mask)
	: impl(impl),
	  render_format(render_format),
	  visual_class_type(visual_class_type),
	  bits_per_rgb(bits_per_rgb),
	  colormap_entries(colormap_entries),
	  red_mask(red_mask),
	  green_mask(green_mask),
	  blue_mask(blue_mask)
{
}
screenObj::visualObj::~visualObj() noexcept=default;

bool screenObj::visualObj::rgb_matches(const const_pictformat &format) const
{
	// Not sure what's the right thing to do if the pictformat's indexed.
	// X rendering extension says: "If the drawable is a Window then the
	// Red, Green and Blue masks must match those in the visual for the
	// window else a Match error is generated." and "Indexed PictFormats
	// hold a list of pixel values and RGBA values while Direct
	// PictFormats hold bit masks for each of R, G, B and A."
	//
	// X protocol extension says "The red-mask, green-mask, and blue-mask
	// are only defined for DirectColor and TrueColor."
	//
	// Based on that: if the pictformat is indexed, then we'll match it
	// against any non-direct visual class type. If the pictformat is
	// direct, we'll match against only against a direct color or true
	// color visual class type, and then we'll compare the masks.

	if (format->indexed)
	{
		if (visual_class_type == visual_class_t::true_color ||
		    visual_class_type == visual_class_t::direct_color)
			return false;

		return true;
	}

	if (visual_class_type != visual_class_t::true_color &&
	    visual_class_type != visual_class_t::direct_color)
		return false;

#define MATCHES(channel) \
	((decltype(channel ## _mask )(format->channel ##_mask)	\
	  << format->channel ## _shift) == channel ## _mask)

	return MATCHES(red) && MATCHES(green) && MATCHES(blue);
}


screenObj::depthObj::depthObj(depth_t depth,
			      std::vector<const_ref<visualObj>> &&visuals)
	: depth(depth),
	  visuals(std::move(visuals))
{
}

screenObj::depthObj::~depthObj() noexcept=default;

static inline std::vector<const_ref<screenObj::visualObj>>
visuals_for_screen_and_depth(xcb_depth_t *depth,
			     const render &render,
			     size_t screen_number,
			     const xcb_screen_t *s)
{
	LOG_FUNC_SCOPE(screenObj::implObj::logger);

	typedef screenObj::visualObj visualObj;

	auto visualtypes=xcb_depth_visuals_iterator(depth);

	std::vector<const_ref<visualObj>> visuals;

	visuals.reserve(visualtypes.rem);

	for ( ; visualtypes.rem; xcb_visualtype_next(&visualtypes))
	{
		if (screen_number >=
		    render.pictformats_by_screen_depth.size())
		{
			LOG_ERROR("No pictformats on screen " << screen_number);
			continue;
		}

		const auto &pf=
			render.pictformats_by_screen_depth[screen_number];

		const auto &depthvisuals=pf.find(depth_t(depth->depth));

		if (depthvisuals == pf.end())
		{
			LOG_ERROR("No pictformats on screen "
				  << screen_number
				  << " for depth "
				  << (int)depth->depth);
			continue;
		}

		if (depth->depth == s->root_depth)
		{
			// Ignore all visuals with the
			// same depth as the root window's
			// depth, except for the root
			// window's visual.
			if (visualtypes.data->visual_id != s->root_visual)
				continue;
		}

		auto format=depthvisuals->second
			.visual_to_pictformat
			.find(visualtypes.data->visual_id);

		if (format == depthvisuals->second.visual_to_pictformat.end())
		{
			LOG_ERROR("No pictformats on screen "
				  << screen_number
				  << " for depth "
				  << (int)depth->depth
				  << ", visual "
				  << visualtypes.data
				  ->visual_id);
			continue;
		}

		visuals.push_back
			(ref<visualObj>::create
			 (ref<visualObj::implObj>
			  ::create(visualtypes.data->visual_id),
			  format->second,
			  (visual_class_t)
			  visualtypes.data->_class,
			  visualtypes.data->bits_per_rgb_value,
			  visualtypes.data->colormap_entries,
			  visualtypes.data->red_mask,
			  visualtypes.data->green_mask,
			  visualtypes.data->blue_mask));
	}

	return visuals;
}

vector<const_ref<screenObj::depthObj>>
screenObj::implObj::create_screen_depths(const xcb_screen_t *s,
					 const render &render,
					 size_t screen_number)
{
	auto v=vector<const_ref<screenObj::depthObj>>::create();

	auto depth_iter=xcb_screen_allowed_depths_iterator(s);

	v->reserve(depth_iter.rem);

	while (depth_iter.rem)
	{
		v->push_back(ref<screenObj::depthObj>
			     ::create(depth_t(depth_iter.data->depth),
				      visuals_for_screen_and_depth
				      (depth_iter.data,
				       render,
				       screen_number,
				       s)));
		xcb_depth_next(&depth_iter);
	}
	return v;
}

LIBCXXW_NAMESPACE_END
