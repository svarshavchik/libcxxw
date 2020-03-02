/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/tooltip_border_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

tooltip_border_appearance_properties::tooltip_border_appearance_properties()
{
}

tooltip_border_appearance_properties::~tooltip_border_appearance_properties()=default;

tooltip_border_appearanceObj::tooltip_border_appearanceObj()=default;

tooltip_border_appearanceObj::~tooltip_border_appearanceObj()=default;

tooltip_border_appearanceObj::tooltip_border_appearanceObj
(const tooltip_border_appearanceObj &o)
	: tooltip_border_appearance_properties{o}
{
}

const_tooltip_border_appearance tooltip_border_appearanceObj
::do_modify(const function<void (const tooltip_border_appearance &)> &closure) const
{
	auto copy=tooltip_border_appearance::create(*this);
	closure(copy);
        return copy;
}

void tooltip_border_appearanceObj::set_theme_border(const std::string &border)
{
	this->border=border;
	this->hpad=border + "_padding_h";
	this->vpad=border + "_padding_v";
}

static inline tooltip_border_appearance default_alpha_border()
{
	auto b=tooltip_border_appearance::create();

	b->set_theme_border("tooltip_border");

	return b;
}

static inline tooltip_border_appearance default_nonalpha_border()
{
	auto b=tooltip_border_appearance::create();

	b->set_theme_border("tooltip_border_square");

	return b;
}

namespace {
#if 0
}
#endif

struct tooltip_border_appearance_base_themeObj : virtual public obj {

	const const_tooltip_border_appearance config=default_alpha_border();

};

#if 0
{
#endif
}

const_tooltip_border_appearance tooltip_border_appearance_base::theme()
{
	return singleton<tooltip_border_appearance_base_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct tooltip_border_appearance_base_nonalpha_themeObj : virtual public obj {

	const const_tooltip_border_appearance config=default_nonalpha_border();

};

#if 0
{
#endif
}

const_tooltip_border_appearance
tooltip_border_appearance_base::nonalpha_theme()
{
	return singleton<tooltip_border_appearance_base_nonalpha_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
