/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_impl.H"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/firstlistitemcontainer.H"
#include "listlayoutmanager/listitemlayoutmanager_impl.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listlayoutstyle.H"
#include "themedim_element.H"
#include "child_element.H"
#include "grid_element.H"
#include "grid_map_info.H"
#include "background_color.H"
#include "background_color_element.H"
#include "icon_images_vector_element.H"
#include "icon.H"
#include "image.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

//! Common code for both highlighted_list_style_impl and checked_list_style_impl

class LIBCXX_HIDDEN listlayoutstyle_common : public listlayoutstyle {

 public:

	//! Implement create_item()

	void create_item(const ref<listlayoutmanagerObj::implObj> &lilm,
			 const gridfactory &underlying_factory,
			 const batch_queue &queue,
			 const new_list_items_t &new_list_items) const override;

	//! Called by create_item()

	//! Number of actual elements to create. It's new_list_items.size()
	//! for a highlighted list, and one more for a checked list.

	virtual size_t n_actual_elements(const new_list_items_t &new_list_items)
		const =0;

	//! Called by create_item()

	//! Return element #i. A highlighted list returns new_list_items.at(i).
	//! A checked_list creates an image for element #0, then goes back
	//! to new_list_items for the rest of the story.

	virtual element get_element_n(const gridfactory &underlying_factory,
				      const new_list_items_t &new_list_items,
				      size_t i) const=0;

	//! Draw the given item highlighted.

	void highlight(IN_THREAD_ONLY,
		       listlayoutmanagerObj::implObj &layout_manager,
		       grid_map_t::lock &lock,
		       size_t row_number) const override;

	//! Set the given row to the given background color.

	static void set_background_color(IN_THREAD_ONLY,
					 grid_map_t::lock &lock,
					 size_t row_number,
					 const background_color &color);

	//! Remove background color from the given row.
	static void remove_background_color(IN_THREAD_ONLY,
					    grid_map_t::lock &lock,
					    size_t row_number);
};

void listlayoutstyle_common
::create_item(const ref<listlayoutmanagerObj::implObj> &lilm,
	      const gridfactory &underlying_factory,
	      const batch_queue &queue,
	      const new_list_items_t &new_list_items) const
{
	size_t n=n_actual_elements(new_list_items);

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

		auto item_e=get_element_n(underlying_factory,
					  new_list_items, i);

		ref<child_elementObj> item_e_impl=item_e->impl;

		// listcontainer shoved a temporary container as the
		// new element's container.

		auto item_c=item_e_impl->child_container;

		auto lmi=ref<listitemlayoutmanagerObj::implObj>
			::create(item_c, item_e,
				 l, r, v_padding);
		listitemcontainer c=
			i == 0 ? (listitemcontainer)firstlistitemcontainer
			::create(item_c, lmi,
				 new_list_items.status_change_callback)
			: listitemcontainer::create(item_c, lmi);

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

void listlayoutstyle_common
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

	set_background_color(IN_THREAD, lock, row_number, color);
}

void listlayoutstyle_common::set_background_color(IN_THREAD_ONLY,
						  grid_map_t::lock &lock,
						  size_t row_number,
						  const background_color &color)
{
	for (const auto &element:(*lock)->elements.at(row_number))
		element->grid_element->impl->set_background_color(IN_THREAD,
								  color);
}

void listlayoutstyle_common::remove_background_color(IN_THREAD_ONLY,
						     grid_map_t::lock &lock,
						     size_t row_number)
{
	for (const auto &element: (*lock)->elements.at(row_number))
		element->grid_element->impl->remove_background_color(IN_THREAD);
}

/////////////////////////////////////////////////////////////////////////////
//
// Highlighted list style.

class LIBCXX_HIDDEN highlighted_list_style_impl
	: public listlayoutstyle_common {

 public:

	//! Implement n_actual_elements()

	size_t n_actual_elements(const new_list_items_t &new_list_items)
		const override
	{
		return new_list_items.elements.size();
	}

	//! Implement get_element_n()

	element get_element_n(const gridfactory &underlying_factory,
			      const new_list_items_t &new_list_items,
			      size_t i) const override
	{
		return new_list_items.elements.at(i);
	}

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

	void initialize(const ref<listlayoutmanagerObj::implObj> &l) const
		override
	{
		l->requested_col_width(0, 100);
	}

	size_t physical_column(size_t logical_column) const override
	{
		return logical_column;
	}
};

static const highlighted_list_style_impl highlighted_list_style_instance;

const listlayoutstyle &highlighted_list=highlighted_list_style_instance;

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

		set_background_color(IN_THREAD, lock, row_number, color);
	}
	else
	{
		remove_background_color(IN_THREAD, lock, row_number);
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

/////////////////////////////////////////////////////////////////////////////
//
// Bulleted list style.

// Implementation object for the bullet element.

typedef ref<icon_images_vector_elementObj<imageObj::implObj>> bullet_image_impl;

class LIBCXX_HIDDEN bulleted_list_style_impl
	: public listlayoutstyle_common {

 public:

	//! Implement n_actual_elements()

	size_t n_actual_elements(const new_list_items_t &new_list_items)
		const override
	{
		return new_list_items.elements.size()+1;
	}

	//! Implement get_element_n()

	element get_element_n(const gridfactory &underlying_factory,
			      const new_list_items_t &new_list_items,
			      size_t i) const override
	{
		if (i == 0)
		{
			// The first element in the row is a bullet element.

			auto container_impl=underlying_factory->container_impl;
			auto icons=container_impl->get_window_handler()
				.create_icon_vector({"bullet1", "bullet2"});

			auto impl=bullet_image_impl::create(icons,
							    container_impl,
							    icons.at(0));

			return element::create(impl);
		}

		return new_list_items.elements.at(i-1);
	}

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

	void initialize(const ref<listlayoutmanagerObj::implObj> &l) const
		override
	{
		l->requested_col_width(1, 100);
	}

	size_t physical_column(size_t logical_column) const override
	{
		return logical_column+1;
	}
};

static const bulleted_list_style_impl bulleted_list_style_instance;

const listlayoutstyle &bulleted_list=bulleted_list_style_instance;

void bulleted_list_style_impl
::unhighlight(IN_THREAD_ONLY,
	      listlayoutmanagerObj::implObj &layout_manager,
	      grid_map_t::lock &lock,
	      size_t row_number) const
{
	if (row_number >= (*lock)->elements.size())
		return; // Shouldn't happen.

	remove_background_color(IN_THREAD, lock, row_number);
}

void bulleted_list_style_impl::refresh(IN_THREAD_ONLY,
				       listlayoutmanagerObj::implObj
				       &layout_manager,
				       grid_map_t::lock &lock,
				       size_t row_number,
				       bool is_highlighted) const
{
	if (row_number >= (*lock)->elements.size())
		return; // Shouldn't happen.

	auto &elements=(*lock)->elements.at(row_number);

	if (elements.empty())
		return; // Shouldn't happen either.

	listitemcontainer item_0=elements.at(0)->grid_element;

	// This container should have one element, a dummy element
	// with the bullet_image_impl, created by the style object.

	item_0->elementObj::impl->for_each_child
		(IN_THREAD,
		 [&]
		 (const auto &child)
		 {
			 bullet_image_impl impl=child->impl;

			 impl->set_icon(IN_THREAD,
					impl->icon_images(IN_THREAD)
					.at(item_0->impl->selected() ? 1:0));
		 });

	if (is_highlighted)
		highlight(IN_THREAD, layout_manager, lock, row_number);
	else
		unhighlight(IN_THREAD, layout_manager, lock, row_number);
}

LIBCXXW_NAMESPACE_END
