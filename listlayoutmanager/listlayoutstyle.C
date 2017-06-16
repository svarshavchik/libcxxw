/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_impl.H"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/listitemlayoutmanager_impl.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listlayoutstyle.H"
#include "themedim_element.H"
#include "child_element.H"
#include "grid_element.H"
#include "grid_map_info.H"
#include "background_color.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN highlighted_list_style_impl
	: public listlayoutstyle {

 public:

	void create_item(IN_THREAD_ONLY,
			 const ref<listlayoutmanagerObj::implObj> &lilm,
			 const gridfactory &underlying_factory,
			 const batch_queue &queue,
			 const new_list_items_t &new_list_items) const override;

	void highlight(IN_THREAD_ONLY,
		       listlayoutmanagerObj::implObj &layout_manager,
		       grid_map_t::lock &lock,
		       size_t row_number) const override;

	//! Unhighlight the given list item.

	void unhighlight(IN_THREAD_ONLY,
			 listlayoutmanagerObj::implObj &layout_manager,
			 grid_map_t::lock &lock,
			 size_t row_number) const override;

	//! Refresh appearance.
	void refresh(IN_THREAD_ONLY,
		     listlayoutmanagerObj::implObj &layout_manager,
		     grid_map_t::lock &lock, size_t i, bool is_highlighted)
		const override;
};

/////////////////////////////////////////////////////////////////////////////
//
// Highlighted list style.

static const highlighted_list_style_impl highlighted_list_style_instance;

const listlayoutstyle &highlighted_list=highlighted_list_style_instance;

void highlighted_list_style_impl
::create_item(IN_THREAD_ONLY,
	      const ref<listlayoutmanagerObj::implObj> &lilm,
	      const gridfactory &underlying_factory,
	      const batch_queue &queue,
	      const new_list_items_t &new_list_items) const
{
	size_t n=new_list_items.size();

	auto container_impl=lilm->container_impl;

	auto v_padding=
		static_cast<themedim_element<listcontainer_dim_v> &>
		(*container_impl).getref();
	auto left_padding=
		static_cast<themedim_element<listcontainer_dim_left> &>
		(*container_impl).getref();
	auto inner_padding=
		static_cast<themedim_element<listcontainer_dim_inner> &>
		(*container_impl).getref();
	auto right_padding=
		static_cast<themedim_element<listcontainer_dim_right> &>
		(*container_impl).getref();

	// For each new_list_item:
	//
	// 1) Create the listitemlayoutmanager implementation object.
	//
	// 2) Construct a container, with the listitemlayoutmanager, for it.
	//
	// 3) Pass it to the underlying_factory.

	for (size_t i=0; i<n; i++)
	{
		auto l=i == 0 ? left_padding:inner_padding;
		auto r=i+1 == lilm->columns ?
			right_padding:inner_padding;

		auto item_e=new_list_items.at(i);

		ref<child_elementObj> item_e_impl=item_e->impl;

		// listcontainer shoved a temporary container as the
		// new element's container.

		auto item_c=item_e_impl->container;

		auto lmi=ref<listitemlayoutmanagerObj::implObj>
			::create(item_c, item_e,
				 l, r, v_padding);
		auto c=listitemcontainer::create(item_c, lmi);

		lmi->needs_recalculation(queue);

		// Show everything
		item_e->show();
		c->show();
		// No padding, and fill the item
		underlying_factory->padding(0);
		underlying_factory->halign(halign::fill);
		underlying_factory->valign(valign::fill);
		underlying_factory->created_internally(c);
	}
}

void highlighted_list_style_impl
::highlight(IN_THREAD_ONLY,
	    listlayoutmanagerObj::implObj &layout_manager,
	    grid_map_t::lock &lock,
	    size_t row_number) const
{
	if (row_number >= (*lock)->elements.size())
		return; // Shouldn't happen.

	auto color=layout_manager.container_impl->hotspot_temperature(IN_THREAD)
		== temperature::hot
		?
		layout_manager.container_impl->
		background_color_element<listcontainer_highlighted_color>
		::get(IN_THREAD)
		:
		layout_manager.container_impl->
		background_color_element<listcontainer_current_color>
		::get(IN_THREAD);

	for (const auto &element:(*lock)->elements.at(row_number))
		element->grid_element->impl->set_background_color(IN_THREAD,
								  color);
}

void highlighted_list_style_impl
::unhighlight(IN_THREAD_ONLY,
	      listlayoutmanagerObj::implObj &layout_manager,
	      grid_map_t::lock &lock,
	      size_t row_number) const
{
	if (row_number >= (*lock)->elements.size())
		return; // Shouldn't happen.

	auto &elements=(*lock)->elements.at(row_number);

	if (elements.empty())
		return; // Shouldn't happen either.

	listitemcontainer item_0=elements.at(0)->grid_element;

	if (item_0->impl->selected())
	{
		auto color=layout_manager.container_impl->
			background_color_element<listcontainer_selected_color>
			::get(IN_THREAD);

		for (const auto &element: (*lock)->elements.at(row_number))
			element->grid_element->impl
				->set_background_color(IN_THREAD, color);
	}
	else
	{
		for (const auto &element: (*lock)->elements.at(row_number))
			element->grid_element->impl->remove_background_color(IN_THREAD);
	}
}

void highlighted_list_style_impl::refresh(IN_THREAD_ONLY,
					  listlayoutmanagerObj::implObj
					  &layout_manager,
					  grid_map_t::lock &lock,
					  size_t row,
					  bool is_highlighted) const
{
	if (is_highlighted)
		highlight(IN_THREAD, layout_manager, lock, row);
	else
		unhighlight(IN_THREAD, layout_manager, lock, row);
}


LIBCXXW_NAMESPACE_END
