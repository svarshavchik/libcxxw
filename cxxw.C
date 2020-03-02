/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "cxxwoptions.H"
#include "connection.H"
#include "screen_depthinfo.H"
#include "x/w/screen.H"
#include "pictformat.H"
#include "messages.H"

#include <x/logger.H>
#include <x/destroy_callback.H>
#include <iostream>
#include <iomanip>
#include <algorithm>

LOG_FUNC_SCOPE_DECL("cxxw", cxxwLog);

static void displayinfo()
{
	x::destroy_callback::base::guard guard;

	typedef LIBCXXW_NAMESPACE::depth_t::value_type depth_t;

	auto conn=LIBCXXW_NAMESPACE::connection::create();

	guard( conn->mcguffin() );

	auto impl=conn->impl;

	std::cout << x::gettextmsg(_("%1% screens, default screen: %2%"),
				   conn->screens(),
				   conn->default_screen()) << std::endl;
	for (const auto &info:impl->render_info.available_pictformats)
	{
		auto p=info.second;

		std::cout << x::gettextmsg(_("Pictformat: %1%, indexed: %2%"),
					   info.first,
					   p->indexed) << std::endl;

		std::cout << x::gettextmsg(_("    Depth: %1%, red: %2%,"
					     " green: %3%, blue: %4%,"
					     " alpha: %5%"),
					   (int)(depth_t)p->depth,
					   (int)(depth_t)p->red_depth,
					   (int)(depth_t)p->green_depth,
					   (int)(depth_t)p->blue_depth,
					   (int)(depth_t)p->alpha_depth)
			  << std::endl;

		std::cout << x::gettextmsg(_("    Mask: red: %1%%2%%3%%4%,"
					     " green: %2%%3%%5%,"
					     " blue: %2%%3%%6%,"
					     " alpha: %2%%3%%7%%8%"),
					   std::hex,
					   std::setw(4),
					   std::setfill('0'),
					   (int)p->red_mask,
					   (int)p->green_mask,
					   (int)p->blue_mask,
					   (int)p->alpha_mask,
					   std::dec) << std::endl;

		std::cout << x::gettextmsg(_("    Shift: red: %1%,"
					     " green: %2%, blue: %3%,"
					     " alpha: %4%"),
					   (int)p->red_shift,
					   (int)p->green_shift,
					   (int)p->blue_shift,
					   (int)p->alpha_shift) << std::endl;
	}

	std::cout << std::endl;

	int screen=0;
	for (const auto &screen_depth_info
		     :impl->render_info.pictformats_by_screen_depth)
	{
		for (const auto &depth_info:screen_depth_info)
		{
			std::cout << std::endl
				  << x::gettextmsg(_("Screen %1%, depth %2%:"),
						   screen,
						   (int)(depth_t)
						   depth_info.first)
				  << std::endl;

			for (const auto &visual:depth_info.second
				     .visual_to_pictformat)
			{

				std::cout << x::gettextmsg(_("    Visual %1%: pictformat %2%"),
							   visual.first,
							   visual.second->impl->id)
					  << std::endl;
			}
		}
		++screen;
	}
}

static void displayscreen()
{
	x::destroy_callback::base::guard guard;

	typedef LIBCXXW_NAMESPACE::depth_t::value_type depth_t;

	auto conn=LIBCXXW_NAMESPACE::connection::create();

	guard( conn->mcguffin() );

	size_t n=conn->screens();

	for (size_t i=0; i<n; i++)
	{
		auto s=LIBCXX_NAMESPACE::w::screen::create(conn, i);

		std::cout << x::gettextmsg(_("Screen %1%: "), i)
			  << s->width_in_pixels().n << "x"
			  << s->height_in_pixels().n << ", "
			  << s->width_in_millimeters().n << "x"
			  << s->height_in_millimeters().n << "mm"
			  << std::endl;

		std::vector<std::string> supported{s->supported().begin(),
				s->supported().end()};

		std::sort(supported.begin(), supported.end());

		std::cout << _("    Supported hints:");

		for (const auto &s:supported)
			std::cout << " " << s;
		std::cout << std::endl;

		std::cout << x::gettextmsg(_("    Workarea: %1%"),
					   s->get_workarea()) << std::endl;

		for (const auto &screen_depth: *s->screen_depths)
		{
			std::cout << "    "
				  << x::gettextmsg(_("    Depth %1%"),
						   (int)(depth_t)screen_depth->depth)
				  << std::endl;
			for (const auto &v:screen_depth->visuals)
			{
				std::cout << "      "
					  << x::gettextmsg(_("        Visual %1%, %2% bits, %3% colormap size, %4% red %5%, green %6%, blue %7%%8%"),
							   (int)v->visual_class_type,
							   (int)v->bits_per_rgb,
							   v->colormap_entries,
							   std::hex,
							   v->red_mask,
							   v->green_mask,
							   v->blue_mask,
							   std::dec)
					  << std::endl;
			}
		}
		std::cout << std::endl;
	}
}

int main(int argc, char **argv)
{
	LOG_FUNC_SCOPE(cxxwLog);

	cxxwoptions options;

	auto args=options.parse(argc, argv)->args;

	try {
		if (options.display->value)
			displayinfo();
		if (options.screen->value)
			displayscreen();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}
	return 0;
}
