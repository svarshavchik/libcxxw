/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "custom_border_cache.H"
#include "current_border_impl.H"
#include "border_impl.H"
#include "x/w/border_infomm_hash.H"
#include "connection.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

custom_border_cacheObj::custom_border_cacheObj()
	: map{map_t::create()}
{
}

custom_border_cacheObj::~custom_border_cacheObj()=default;

border_info screenObj::implObj::convert_to_border_info(const border_infomm &mm)
{
	current_theme_t::lock lock{current_theme};

	return convert_to_border_info(lock, mm);
}

border_info screenObj::implObj
::convert_to_border_info(const current_theme_t::lock &lock,
			 const border_infomm &mm)
{
	border_info info;

	info.colors=mm.colors;

	info.width=compute_width(lock, mm.width);
	info.height=compute_height(lock, mm.height);

	if (info.width == dim_t::infinite() || info.height == dim_t::infinite())
		info.width=info.height=0;

	auto radius_w=compute_width(lock, mm.radius);
	auto radius_h=compute_height(lock, mm.radius);

	if (radius_w == dim_t::infinite() ||
	    radius_h == dim_t::infinite())
		radius_w=radius_h=0;

	if (radius_w == 0 && mm.rounded)
		radius_w=1;

	if (radius_h == 0 && mm.rounded)
		radius_h=1;

	if (radius_w <= 1 && mm.radius)
		radius_w=2;

	if (radius_h <= 1 && mm.radius)
		radius_h=2;

	info.hradius=radius_w;
	info.vradius=radius_h;

	info.dashes.reserve(mm.dashes.size());

	for (const auto &orig_dash:mm.dashes)
	{
		auto dash_w=compute_width(lock, orig_dash);
		auto dash_h=compute_height(lock, orig_dash);

		if (dash_w == dim_t::infinite() ||
		    dash_h == dim_t::infinite())
			dash_w=dash_h=0;

		uint8_t dash=number<uint8_t, uint8_t>::truncate
			((dash_w+dash_h)/2);

		if (dash == 0 && orig_dash > 0)
			dash=1;

		info.dashes.push_back(dash);
	}

	return info;
}

class LIBCXX_HIDDEN custom_current_border_implObj
	: public current_border_implObj {

	const ref<screenObj::implObj> screen;
	const border_infomm info;

	defaulttheme current_theme_thread_only;

	static border_impl border_impl_from_info(const ref<screenObj::implObj>
						 &screen,
						 const border_infomm &info,
						 const current_theme_t::lock
						 &lock)
	{
		auto b=border_impl::create(screen->convert_to_border_info
					   (lock, info));
		b->calculate();
		return b;
	}

 public:
	THREAD_DATA_ONLY(current_theme);

	custom_current_border_implObj(const ref<screenObj::implObj> &screen,
				      const border_infomm &info)
		: custom_current_border_implObj(screen,
						info,
						current_theme_t::lock
						(screen->current_theme))
	{
	}

	custom_current_border_implObj(const ref<screenObj::implObj> &screen,
				      const border_infomm &info,
				      const current_theme_t::lock &lock)
		: current_border_implObj(border_impl_from_info(screen, info,
							       lock)),
		screen(screen),
		info(info),
		current_theme_thread_only(*lock)
		{
		}

	~custom_current_border_implObj()=default;

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
	{
		// This custom border object can be attached to multiple
		// border display elements. Go through the motions of
		// creating a new border object only the first time we're
		// called, here.

		auto &t=current_theme(IN_THREAD);

		if (new_theme == t)
			return;

		t=new_theme;

		current_theme_t::lock lock{screen->current_theme};

		border(IN_THREAD)=border_impl_from_info(screen, info, lock);
	}
};

current_border_impl screenObj::implObj
::get_custom_border(const border_infomm &info)
{
	return custom_borders->map
		->find_or_create
		(info,
		 [&, this]
		 {
			 return ref<custom_current_border_implObj>
				 ::create(ref<implObj>(this), info);
		 });
}

LIBCXXW_NAMESPACE_END
