/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/standard_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_combobox_popup_container.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

standard_comboboxlayoutmanagerObj::implObj
::implObj(const ref<custom_combobox_containerObj::implObj> &container_impl,
	  const new_custom_comboboxlayoutmanager &style)
	: custom_comboboxlayoutmanagerObj::implObj(container_impl, style)
{
}

standard_comboboxlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager standard_comboboxlayoutmanagerObj::implObj::create_public_object()
{
	return standard_comboboxlayoutmanager
		::create(ref<implObj>(this),
			 container_impl->popup_container->layout_impl);

}

bool standard_comboboxlayoutmanagerObj::implObj
::search(grid_map_t::lock &lock,
	 size_t starting_index,
	 const std::u32string &text,
	 size_t &found)
{
	size_t n=text_items(lock).size();

	size_t search_size=text.size();

	for (size_t i=0; i<n; ++i)
	{
		size_t j=(i+starting_index) % n;

		const auto &string=text_items(lock).at(j).string;

		if (string.size() < search_size)
			continue;

		size_t l;

		for (l=0; l<search_size; ++l)
			if (unicode_lc(string[l]) != unicode_lc(text[l]))
				break;

		if (l == search_size)
		{
			found=j;
			return true;
		}
	}

	return false;
}

LIBCXXW_NAMESPACE_END
