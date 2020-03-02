/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/menu.H"
#include "x/w/label.H"
#include "x/w/popup_list_appearance.H"

LIBCXXW_NAMESPACE_START

menubarfactoryObj::menubarfactoryObj(const menubarlayoutmanager &layout)
	: layout(layout)
{
}

menubarfactoryObj::~menubarfactoryObj()=default;

menu menubarfactoryObj::do_add_text(const text_param &t,
				    const function<menu_content_creator_t> &cf,
				    const extra_args_t &args)
{
	return do_add(make_function<menu_creator_t>
		      ([&](const auto &f)
		       {
			       f->create_label(t);
		       }),
		      cf,
		      args);
}

menu menubarfactoryObj::do_add(const function<menu_creator_t> &creator,
			       const function<menu_content_creator_t> &ccreator,
			       const extra_args_t &args)
{
	std::optional<shortcut> default_shortcut;
	std::optional<const_popup_list_appearance> default_appearance;

	return do_add_impl(creator, ccreator,
			   optional_arg_or<shortcut>(args, default_shortcut),
			   optional_arg_or<const_popup_list_appearance>
			   (args, default_appearance,
			    popup_list_appearance::base::menu_theme()));
}


LIBCXXW_NAMESPACE_END
