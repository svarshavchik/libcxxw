/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menuitemextrainfo.H"
#include "menu/menuitemextrainfo_impl.H"
#include "singletonlayoutmanager_impl.H"
#include "x/w/element.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "x/w/menulayoutmanager.H"
#include "canvas.H"
#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

// Construct the element for the given menuitem_type.

static element element_for(const ref<containerObj::implObj> &parent_container,
			   const menuitem_type_t &menuitem_type)
{
	text_param text;

	std::visit(visitor{
			[&](const menuitem_plain &plain)
			{
				if (plain.menuitem_shortcut)
				{
					text((std::u32string)
					     plain.menuitem_shortcut);
				}
			}}, menuitem_type);

	auto l=factoryObj::create_label_internal(text, 0, halign::left,
						 parent_container);

	l->show();
	return l;
}

// Subclass the singleton layout manager.

// In order to provide internal padding separation between the menu item
// name, and its extra info element.

class LIBCXX_HIDDEN menuitemextrainfo_layoutmanager_implObj
	: public singletonlayoutmanagerObj::implObj {

	typedef singletonlayoutmanagerObj::implObj superclass_t;

 public:

	const ref<menuitemextrainfoObj::implObj> impl;

	menuitemextrainfo_layoutmanager_implObj
		(const ref<menuitemextrainfoObj::implObj> &impl,
		 const elementptr &initial_element)
		: superclass_t(impl, initial_element),
		impl(impl)
	{
	}

	dim_t get_left_padding(IN_THREAD_ONLY) override
	{
		return impl->pixels(IN_THREAD);
	}

};

menuitemextrainfoObj::menuitemextrainfoObj(const ref<implObj> &impl,
					   const menuitem_type_t &menuitem_type)
	: containerObj(impl,
		       ref<menuitemextrainfo_layoutmanager_implObj>
		       ::create(impl, element_for(impl, menuitem_type))),
	  impl(impl),
	  menuitem_type(menuitem_type)
{
	// Need to trigger the recalculation
	get_layout_impl()->create_public_object();
}

menuitemextrainfoObj::~menuitemextrainfoObj()=default;

void menuitemextrainfoObj::update(const menuitem_type_t &new_type)
{
	singletonlayoutmanager layout_manager=get_layoutmanager();

	auto f=layout_manager->replace();
	auto new_element=element_for(containerObj::impl, new_type);

	menuitem_type.update
		([&]
		 (auto &type)
		 {
			 f->created_internally(new_element);
			 type=new_type;
		 });
}

LIBCXXW_NAMESPACE_END
