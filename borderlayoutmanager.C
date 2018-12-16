/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/background_color.H"
#include "x/w/borderlayoutmanager.H"
#include "x/w/factory.H"
#include "x/w/container.H"
#include "x/w/font_arg.H"
#include "x/w/rgb.H"
#include "x/w/impl/container.H"
#include "capturefactory.H"

LIBCXXW_NAMESPACE_START

borderlayoutmanagerObj::borderlayoutmanagerObj(const ref<implObj> &impl)
	: singletonlayoutmanagerObj{impl}, impl{impl}
{
}

borderlayoutmanagerObj::~borderlayoutmanagerObj()=default;

container factoryObj
::do_create_bordered_element(const function<void (const factory &)>
			     &creator,
			     const new_element_border &info)
{
	// Retrieve my parent, and create the container for the
	// border layout manager.
	auto parent=get_container_impl();

	richtextptr title;

	if (info.title.string.size())
	{
		auto &e=parent->container_element_impl();

		richtextmeta meta
			{
			 e.create_background_color(e.label_theme_color()),
			 e.create_current_fontcollection
			 (theme_font{e.label_theme_font()})
			};

		auto string=e.create_richtextstring(meta,
						    info.title.string);
		title=richtext::create(string, halign::left, 0);
	}

	auto c_impl=ref<bordercontainer_elementObj<container_elementObj
						   <child_elementObj>>>
		::create(info.border,
			 info.border,
			 info.border,
			 info.border,
			 title,
			 info.title_indent,
			 info.hpad, info.vpad,
			 parent);

	// Invoke the creator to set the element for which we're providing
	// the border.
	auto f=capturefactory::create(c_impl);

	creator(f);

	auto e=f->get();

	// Finish everything up.
	auto lm_impl=ref<borderlayoutmanagerObj::implObj>::create(c_impl,
								  c_impl,
								  e,
								  halign::fill,
								  valign::fill);
	auto new_container=container::create(c_impl, lm_impl);
	created(new_container);
	return new_container;
}

LIBCXXW_NAMESPACE_END
