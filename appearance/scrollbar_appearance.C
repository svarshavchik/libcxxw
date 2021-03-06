/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/scrollbar_appearance.H"
#include "x/w/scrollbar_images_appearance.H"
#include "x/w/focus_border_appearance.H"
#include <x/singleton.H>
LIBCXXW_NAMESPACE_START

scrollbar_appearance_properties::scrollbar_appearance_properties()
	: background_color{"scrollbar_background_color"},
	  focus_border{focus_border_appearance::base::scrollbar_theme()},
	  horizontal1{scrollbar_images_appearance::base::horizontal1()},
	  horizontal2{scrollbar_images_appearance::base::horizontal2()},
	  vertical1{scrollbar_images_appearance::base::vertical1()},
	  vertical2{scrollbar_images_appearance::base::vertical2()}
{
}

scrollbar_appearance_properties::~scrollbar_appearance_properties()=default;

scrollbar_appearanceObj::scrollbar_appearanceObj()=default;

scrollbar_appearanceObj::~scrollbar_appearanceObj()=default;

scrollbar_appearanceObj::scrollbar_appearanceObj
(const scrollbar_appearanceObj &o)
	: scrollbar_appearance_properties{o}
{
}

const_scrollbar_appearance scrollbar_appearanceObj
::do_modify(const function<void (const scrollbar_appearance &)> &closure) const
{
	auto copy=scrollbar_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct scrollbar_appearance_base_themeObj : virtual public obj {

	const const_scrollbar_appearance config=const_scrollbar_appearance::create();

};

#if 0
{
#endif
}

const_scrollbar_appearance scrollbar_appearance_base::theme()
{
	return singleton<scrollbar_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
