/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/theme_font.H"
#include "generic_window_handler.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/impl/fonts/fontcollection.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

theme_fontObj::theme_fontObj(elementObj::implObj &e,
			     const font_arg &f)
	: theme_fontObj(e.create_fontcollection(f))
{
}

theme_fontObj::theme_fontObj(const current_fontcollection &current_font)
	: current_font_thread_only(current_font)
{
}

theme_fontObj::~theme_fontObj()=default;

void theme_fontObj::initialize(ONLY IN_THREAD)
{
	auto theme=font_element().get_screen()->impl->current_theme.get();

	theme_updated(IN_THREAD, theme);
}

void theme_fontObj::theme_updated(ONLY IN_THREAD,
				  const defaulttheme &new_theme)
{
	current_font(IN_THREAD)->theme_updated(IN_THREAD, new_theme);
	font_nominal_width(IN_THREAD)=
		current_font(IN_THREAD)->fc(IN_THREAD)->nominal_width();
	font_height(IN_THREAD)=
		current_font(IN_THREAD)->fc(IN_THREAD)->height();
}

LIBCXXW_NAMESPACE_END
