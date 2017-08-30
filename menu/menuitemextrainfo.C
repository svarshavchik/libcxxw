/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menuitemextrainfo.H"
#include "menu/menuitemextrainfo_impl.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "popup/popup_attachedto_handler_element.H"
#include "singletonlayoutmanager_impl.H"
#include "x/w/element.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "x/w/menulayoutmanager.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "canvas.H"
#include "image.H"
#include "icon.H"
#include "generic_window_handler.H"
#include "catch_exceptions.H"
#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

created_menuitem_submenu
::created_menuitem_submenu(const menuitem_submenu &,
			   const popup &submenu_popup,
			   const ref<popup_attachedto_handlerObj>
			   &submenu_popup_handler)
	: submenu_popup{submenu_popup},
	  submenu_popup_handler{submenu_popup_handler}
{
}

// Visitor of a menuitem_type_t variant, that converts it to a
// menuitem_created_type_t.

struct LIBCXX_HIDDEN construct_created_type_t {

	const ref<menuitemextrainfoObj::implObj> impl;

	created_menuitem_type_t operator()(const menuitem_plain &p)
	{
		return created_menuitem_plain{p};
	}

	created_menuitem_type_t operator()(const menuitem_submenu &s)
	{
		auto [popup, handler]=menubarlayoutmanagerObj::implObj
			::create_popup_menu
			(impl,
			 [&]
			 (const auto &layout_manager)
			 {
				 auto &logger=elementObj::implObj::logger;

				 if (s.creator)
					 try {
						 s.creator(layout_manager);
					 } CATCH_EXCEPTIONS;
			 },
			 attached_to::submenu_next);

		return created_menuitem_submenu{s, popup, handler};
	}
};

// Construct the element for the given menuitem_type.

static element element_for(const ref<menuitemextrainfoObj::implObj>
			   &parent_container,
			   const menuitem_type_t &menuitem_type,
			   const created_menuitem_type_t &created_menuitem_type)
{
	text_param text;

	auto e=std::visit(visitor{
			[&](const menuitem_plain &plain)
			{
				if (plain.menuitem_shortcut)
				{
					text((std::u32string)
					     plain.menuitem_shortcut);
				}
				return element{
					factoryObj::create_label_internal
						(text, 0, halign::left,
						 parent_container)
						};

			},
			[&](const menuitem_submenu &submenu)
			{
				// Create a submenu indicator image.

				auto &created=std::get<created_menuitem_submenu>
					(created_menuitem_type);

				auto &h=parent_container->get_element_impl()
					.get_window_handler();

				auto icon=h.create_icon_mm("submenu",
							   render_repeat::none,
							   0, 0);

				auto impl=ref
					<popup_attachedto_handler_elementObj
					 <imageObj::implObj>>
					::create(created.submenu_popup_handler,
						 parent_container, icon);

				return element{image::create(impl)};
			}

		}, menuitem_type);

	e->show();
	return e;
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
	: menuitemextrainfoObj(impl,
			       menuitem_type,
			       std::visit(construct_created_type_t{impl},
					  menuitem_type))
{
}

menuitemextrainfoObj::menuitemextrainfoObj(const ref<implObj> &impl,
					   const menuitem_type_t &menuitem_type,
					   const created_menuitem_type_t
					   &created_menuitem_type)

	: containerObj(impl,
		       ref<menuitemextrainfo_layoutmanager_implObj>
		       ::create(impl, element_for(impl, menuitem_type,
						  created_menuitem_type))),
	  impl(impl),
	  menuitem_type{created_menuitem_type}
{
	// Need to trigger the recalculation
	get_layout_impl()->create_public_object();
}

menuitemextrainfoObj::~menuitemextrainfoObj()=default;

void menuitemextrainfoObj::update_shortcut(const menuitem_type_t &new_type)
{
	impl->uninstall_shortcut();

	std::visit(visitor{
			[&, this](const menuitem_plain &plain)
			{
				if (plain.menuitem_shortcut)
					this->impl->install_shortcut
						(plain.menuitem_shortcut,
						 activated_in_thread(this));
			},

			[this](const menuitem_submenu &submenu)
			{
			}
		}, new_type);
}

void menuitemextrainfoObj::activated(IN_THREAD_ONLY)
{
	// Our parent container must be using the list layout manager.

	listlayoutmanagerptr parent_container_layout_managerptr;

	impl->listlayoutmanager_container->invoke_layoutmanager
		([&]
		 (const auto &lm)
		 {
			 parent_container_layout_managerptr=
				 lm->create_public_object();
		 });

	if (!parent_container_layout_managerptr)
		return; // Maybe things are being deleted...

	listlayoutmanager parent_container_layout_manager=
		parent_container_layout_managerptr;

	list_lock lock{parent_container_layout_manager};

	auto looked_up=parent_container_layout_manager->impl
		->lookup_item(lock, impl);

	if (!looked_up)
		return;

	parent_container_layout_manager->impl
		->autoselect(IN_THREAD,
			     parent_container_layout_manager,
			     lock,
			     looked_up.value());
}

bool menuitemextrainfoObj::enabled(IN_THREAD_ONLY)
{
	if (impl->data(IN_THREAD).removed)
		return false;

	return impl->data(IN_THREAD).enabled;
}

void menuitemextrainfoObj::update(const menuitem_type_t &new_type)
{
	singletonlayoutmanager layout_manager=get_layoutmanager();

	auto new_created_menuitem_type=
		std::visit(construct_created_type_t{impl}, new_type);

	auto f=layout_manager->replace();
	auto new_element=element_for(containerObj::impl, new_type,
				     new_created_menuitem_type);

	menuitem_type.update
		([&]
		 (auto &type)
		 {
			 f->created_internally(new_element);
			 type=new_created_menuitem_type;
		 });
	update_shortcut(new_type);
}

LIBCXXW_NAMESPACE_END
