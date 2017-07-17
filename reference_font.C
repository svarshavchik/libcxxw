/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "reference_font.H"
#include "generic_window_handler.H"
#include "fonts/current_fontcollection.H"
#include "fonts/fontcollection.H"
#include "element_screen.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

reference_fontObj::reference_fontObj(elementObj::implObj &e,
				     const std::string_view
				     &fontname)
	: reference_fontObj(e.get_window_handler().create_theme_font(fontname))
{
}

reference_fontObj::reference_fontObj(const current_fontcollection &reference_font)
	: reference_font_thread_only(reference_font)
{
}

reference_fontObj::~reference_fontObj()=default;

void reference_fontObj::initialize(IN_THREAD_ONLY)
{
	auto theme=*current_theme_t::lock{
		font_element().get_screen()->impl->current_theme
	};

	theme_updated(IN_THREAD, theme);
}

void reference_fontObj::theme_updated(IN_THREAD_ONLY,
				      const defaulttheme &new_theme)
{
	reference_font(IN_THREAD)->theme_updated(IN_THREAD, new_theme);
	font_nominal_width(IN_THREAD)=
		reference_font(IN_THREAD)->fc(IN_THREAD)->nominal_width();
	font_height(IN_THREAD)=
		reference_font(IN_THREAD)->fc(IN_THREAD)->height();
}

LIBCXXW_NAMESPACE_END
