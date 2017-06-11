/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_impl.H"
#include "listlayoutmanager/listitemlayoutmanager_impl.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listlayoutstyle.H"
#include "themedim_element.H"
#include "child_element.H"
#include "themedim_elementfwd.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN highlighted_list_style_impl
	: public listlayoutstyle {

 public:

	void create_item(const ref<listlayoutmanagerObj::implObj> &lilm,
			 const gridfactory &underlying_factory,
			 const batch_queue &queue,
			 const new_list_items_t &new_list_items) const override;
};

static const highlighted_list_style_impl highlighted_list_style_instance;

const listlayoutstyle &highlighted_list=highlighted_list_style_instance;

void highlighted_list_style_impl
::create_item(const ref<listlayoutmanagerObj::implObj> &lilm,
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
		auto c=container::create(item_c, lmi);

		lmi->needs_recalculation(queue);

		// Show everything
		item_e->show();
		c->show();
		underlying_factory->padding(0);
		underlying_factory->created_internally(c);
	}
}

LIBCXXW_NAMESPACE_END
