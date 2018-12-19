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

	//! Constructor
new_borderlayoutmanager::new_borderlayoutmanager(const functionref<void
						 (const factory &)>
						 &element_factory)
	: element_factory{element_factory},
	  border{"frame_border"},
	  title_indent{"frame_title_indent"},
	  no_background{false},
	  hpad{"frame_hpad"},
	  vpad{"frame_vpad"}
{
}

new_borderlayoutmanager::~new_borderlayoutmanager()=default;

layout_impl new_borderlayoutmanager::create(const container_impl &parent) const
{
	throw EXCEPTION("Not implemented");
}

container new_borderlayoutmanager::create(const container_impl &parent,
					  const function<void(const container &)
					  > &creator)
	const
{
	richtextptr richtext_title;

	if (title.string.size())
	{
		auto &e=parent->container_element_impl();

		richtextmeta meta
			{
			 e.create_background_color(e.label_theme_color()),
			 e.create_current_fontcollection
			 (theme_font{e.label_theme_font()})
			};

		auto string=e.create_richtextstring(meta, title);
		richtext_title=richtext::create(string, halign::left, 0);
	}

	auto c_impl=ref<bordercontainer_elementObj<container_elementObj
						   <child_elementObj>>>
		::create(border,
			 border,
			 border,
			 border,
			 richtext_title,
			 title_indent,
			 hpad, vpad,
			 parent);

	// Invoke the creator to set the element for which we're providing
	// the border.
	auto f=capturefactory::create(c_impl);

	element_factory(f);

	auto e=f->get();

	// Finish everything up.
	auto lm_impl=ref<borderlayoutmanagerObj::implObj>::create(c_impl,
								  c_impl,
								  e,
								  halign::fill,
								  valign::fill);
	auto new_container=container::create(c_impl, lm_impl);

	if (!no_background && title.string.empty())
		e->set_background_color("frame_background");

	creator(new_container);

	return new_container;
}

void borderlayoutmanagerObj::update_title(const text_param &title)
{
	impl->run_as([me=ref{this}, title]
		     (ONLY IN_THREAD)
		     {
			     me->update_title(IN_THREAD, title);
		     });
}

void borderlayoutmanagerObj::update_title(ONLY IN_THREAD,
					  const text_param &title)
{
	richtextptr richtext_title;

	if (title.string.size())
	{
		auto &e=impl->get_element_impl();

		richtextmeta meta
			{
			 e.create_background_color(e.label_theme_color()),
			 e.create_current_fontcollection
			 (theme_font{e.label_theme_font()})
			};

		auto string=e.create_richtextstring(meta, title);
		richtext_title=richtext::create(string, halign::left, 0);
	}

	impl->bordercontainer_impl->set_title(IN_THREAD, richtext_title);
}

LIBCXXW_NAMESPACE_END
