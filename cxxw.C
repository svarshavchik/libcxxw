#include "cxxwoptions.H"
#include "connection.H"
#include "connection_info.H"
#include "pictformat.H"
#include "messages.H"

#include <x/logger.H>
#include <iostream>
#include <iomanip>

LOG_FUNC_SCOPE_DECL("cxxw", cxxwLog);

static void displayinfo()
{
	LOG_FUNC_SCOPE(cxxwLog);

	typedef LIBCXXW_NAMESPACE::depth_t::value_type depth_t;

	auto conn=LIBCXXW_NAMESPACE::connection::create();

	auto impl=conn->impl;

	std::cout << x::gettextmsg(_("Default screen: %1%"),
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

int main(int argc, char **argv)
{
	LOG_FUNC_SCOPE(cxxwLog);

	cxxwoptions options;

	auto args=options.parse(argc, argv)->args;

	try {
		if (options.display->value)
			displayinfo();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}
	return 0;
}
