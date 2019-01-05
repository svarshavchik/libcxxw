/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/itemlayoutmanager.H"
#include "itemlayoutmanager/itembutton_impl.H"
#include "itemlayoutmanager/itemlayoutmanager_impl.H"
#include "itemlayoutmanager/peepholed_item_container_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "x/w/impl/always_visible_element.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/focusable_container.H"
#include "gridlayoutmanager.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "peephole/peephole.H"
#include "peephole/peephole_impl_element.H"
#include "image_button.H"
#include "image_button_internal_impl.H"

LIBCXXW_NAMESPACE_START

itemlayoutmanagerObj::itemlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj{impl}, impl{impl}
{
}

itemlayoutmanagerObj::~itemlayoutmanagerObj()=default;


static auto create_new_itembutton(const ref<itemlayoutmanagerObj::implObj>
				  &impl,
				  const function<void (const factory &)>
				  &callback)
{
	auto new_itembutton_impl=
		ref<bordercontainer_elementObj<
			itembuttonObj::implObj>>::create
		(impl->button_border,
		 impl->button_border,
		 impl->button_border,
		 impl->button_border,
		 richtextptr{},
		 0,
		 impl->itembutton_h_padding,
		 impl->itembutton_v_padding,
		 impl->layout_container_impl,
		 child_element_init_params{"background@libcxx.com"});

	image_button_config i_config;
	create_image_button_info cibi{new_itembutton_impl, true, i_config};

	cibi.button_background_color=impl->itembutton_background_color;
	cibi.click_anywhere=false;

	auto b=create_image_button
		(cibi,
		 [&]
		 (const container_impl &parent_container)
		 {
			 return scroll_imagebutton_specific_height
				 (parent_container,
				  "itemdelete1",
				  "itemdelete2",
				  0);
		 },
		 [&]
		 (const auto &f)
		 {
			 callback(f);
		 });

	auto blm=ref<borderlayoutmanagerObj::implObj>::create
		(new_itembutton_impl, new_itembutton_impl, b,
		 halign::center, valign::middle);

	return itembutton::create(new_itembutton_impl, blm, b);
}

void itemlayoutmanagerObj::do_append(const function<void (const factory &)>
				     &callback)
{
	impl->append(create_new_itembutton(impl, callback));
}

void itemlayoutmanagerObj::do_insert(size_t i,
				     const function<void (const factory &)>
				     &callback)
{
	impl->insert(create_new_itembutton(impl, callback), i);
}

size_t itemlayoutmanagerObj::size() const
{
	return impl->size();
}

void itemlayoutmanagerObj::on_remove(const itemlayout_callback_t &callback)
{
	item_info_t::lock lock{impl->item_info};

	lock->callback=callback;
}

void itemlayoutmanagerObj::remove_item(size_t i)
{
	impl->remove_item(i);
}

element itemlayoutmanagerObj::get_item(size_t i) const
{
	return impl->get_item(i);
}

itemlayout_lock::itemlayout_lock(const itemlayoutmanager &lm)
	: item_info_t::lock{lm->impl->item_info}, layout_manager{lm}
{
}

itemlayout_lock::~itemlayout_lock()=default;

/////////////////////////////////////////////////////////////////////////
new_itemlayoutmanager::new_itemlayoutmanager()
	: new_itemlayoutmanager{[]
				(ONLY IN_THREAD,
				 size_t i,
				 const auto &lock,
				 const auto &trigger,
				 const auto &busy_mcguffin)
				{
					lock.layout_manager->remove_item(i);
				}}
{
}

new_itemlayoutmanager::new_itemlayoutmanager(const itemlayout_callback_t
					     &callback)
	: callback{callback},
	  button_border{"itembutton_border"},
	  itembutton_h_padding{"itembutton-h-padding"},
	  itembutton_v_padding{"itembutton-v-padding"},
	  itemlayout_h_padding{"itemlayout-h-padding"},
	  itemlayout_v_padding{"itemlayout-v-padding"},
	  itembutton_background_color{"itembutton_background_color"}
{
}

new_itemlayoutmanager::~new_itemlayoutmanager()=default;

namespace {
#if 0
}
#endif


//! The public object for the public container that uses the item layout manager

//! The container is actually a peephole with scrollbars. Overrides
//! \ref focusable_container "focusable_container".
//!
//! get_layout_impl() gets overridden. The item layout manager that gets
//! created for the container in the peephole is saved in this public object.
//!
//! get_layout_impl() gets overriden to return the item layout manager
//! implementation object, so calling get_layoutmanager() on the public
//! container returns the item layout manager.
//!
//! Also overrides focusable's get_impl(), to correctly handle focusable
//! semantics.

class item_containerObj : public focusable_containerObj {

public:
	const ref<itemlayoutmanagerObj::implObj> item_layout_impl;

	item_containerObj(const container_impl &impl,
			  const layout_impl &container_layout_impl,
			  const ref<itemlayoutmanagerObj::implObj>
			  &item_layout_impl)
		: focusable_containerObj{impl, container_layout_impl},
		  item_layout_impl{item_layout_impl}
	{
	}

	~item_containerObj()=default;

	layout_impl get_layout_impl() const override
	{
		return item_layout_impl;
	}

	focusable_impl get_impl() const override
	{
		return item_layout_impl->get_main_focusable()->get_impl();
	}

	void do_get_impl(const function<internal_focusable_cb> &cb)
		const override
	{
		process_focusable_impls_from_focusables
			(cb, item_layout_impl->get_all_focusables());
	}
};

#if 0
{
#endif
}

//////////////////////////////////////////////////////////////////////////////
//
// The actual container we create is a container with a grid layout manager
// containing a peephole. The peephole contains the real container that's
// managed by the item layout manager.

focusable_container new_itemlayoutmanager::create(const container_impl &parent)
	const
{
	// The new container, and it's actual layout manager, the grid
	// layout manager.
	auto impl=ref<nonrecursive_visibilityObj<
		container_elementObj<child_elementObj>>>::create
		(parent,
		 child_element_init_params{"background@libcxx.com",
						   // The new container can be
						   // sized to any width, and
						   // initially has no height.
					   {{0, 0, dim_t::infinite()},
					    {0, 0, 0}}});

	// The first element in the grid is a peephole. Here's its container.

	auto peephole_container_impl=ref<always_visible_elementObj<
		peephole_impl_elementObj<container_elementObj
					 <child_elementObj>>>>
		::create(impl,
			 child_element_init_params{"background@libcxx.com",
						   {{0, 0, dim_t::infinite()},
						    {0, 0, 0}}});

	peephole_style p_style;

	p_style.height_algorithm=peephole_algorithm::stretch_peephole;

	ptr<itemlayoutmanagerObj::implObj> itemlm_implptr;

	const auto &[layout_impl, grid_impl, grid]=
		create_peephole_with_scrollbars
		([&]
		 (const ref<peepholeObj::layoutmanager_implObj> &layout_impl)
		 -> peephole_element_factory_ret_t
		 {
			 auto peephole_container=
				 peephole::create(peephole_container_impl,
						  layout_impl);

			 return {
				 peephole_container,
				 peephole_container,
				 std::nullopt,
				 std::nullopt,
				 {},
			 };
		 },
		 [&, this]
		 (const auto &info, const auto &scrollbars)
		 {
			 // The peephole shows this peepholed container,
			 // the real container with the item layout manager.

			 auto peepholed_container_impl=
				 ref<peepholed_item_containerObj::implObj>
				 ::create(peephole_container_impl,
					  scrollbars.horizontal_scrollbar,
					  scrollbars.vertical_scrollbar,
					  *this);

			 auto itemlm_impl=ref<itemlayoutmanagerObj::implObj>
				 ::create(peepholed_container_impl,
					  *this);

			 itemlm_implptr=itemlm_impl;

			 // The container is the peepholed container.
			 auto peepholed_container=
				 ref<peepholed_item_containerObj>
				 ::create(peepholed_container_impl,
					  itemlm_impl);

			 return ref<peepholeObj::layoutmanager_implObj
				    ::scrollbarsObj>
				 ::create(info, scrollbars,
					  peephole_container_impl,
					  peepholed_container);
		 },
		 create_peephole_gridlayoutmanager,
		 {
		  impl,
		  std::nullopt,
		  p_style,
		  scrollbar_visibility::automatic,
		  scrollbar_visibility::never,
		 });

	return ref<item_containerObj>::create(impl, grid_impl, itemlm_implptr);
}

LIBCXXW_NAMESPACE_END
