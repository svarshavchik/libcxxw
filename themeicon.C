/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "themeiconobj.H"
#include "defaulttheme.H"
#include "pixmap.H"
#include "pixmap_with_picture.H"
#include "screen.H"
#include "x/w/dim_arg.H"

LIBCXXW_NAMESPACE_START

template<typename dim_type>
themeiconObj<dim_type>::themeiconObj(const std::string &name,
				     const defaulttheme &theme,
				     const dim_type &width,
				     const dim_type &height,
				     icon_scale scale,
				     const pixmap_with_picture &image)
	: iconObj(image),
	  name(name),
	  theme(theme),
	  width(width),
	  height(height),
	  scale(scale)
{
}

template<typename dim_type>
themeiconObj<dim_type>::~themeiconObj()=default;

template<>
icon themeiconObj<dim_arg>::initialize(ONLY IN_THREAD)
{
	auto drawable=image->impl;

	auto current_theme=drawable->get_screen()->impl->current_theme.get();

	if (theme == current_theme)
		return icon(this);

	return drawable->create_icon({name, image->repeat, width, height});
}

template<>
icon themeiconObj<dim_arg>::theme_updated(ONLY IN_THREAD,
					  const defaulttheme &new_theme)
{
	if (new_theme == theme)
		return icon(this);

	return image->impl->create_icon({name, image->repeat, width, height});
}

template<>
icon themeiconObj<dim_t>::initialize(ONLY IN_THREAD)
{
	auto drawable=image->impl;

	auto current_theme=drawable->get_screen()->impl->current_theme.get();

	if (theme == current_theme)
		return icon(this);

	return drawable->create_icon_pixels(name, image->repeat,
					    width, height, scale);
}

template<>
icon themeiconObj<dim_t>::theme_updated(ONLY IN_THREAD,
					const defaulttheme &new_theme)
{
	if (new_theme == theme)
		return icon(this);

	return image->impl
		->create_icon_pixels(name, image->repeat, width, height, scale);
}

template class themeiconObj<dim_arg>;

template class themeiconObj<dim_t>;

LIBCXXW_NAMESPACE_END
