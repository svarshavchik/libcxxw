/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_popup.H"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "combobox/custom_combobox_popup_layoutmanager.H"
#include "combobox/combobox_button_impl.H"
#include "popup/popup_attachedto_info.H"
#include "popup/popup_impl.H"

#include "focus/focusframecontainer_element.H"
#include "focus/focusable_element.H"
#include "peephole/peephole_toplevel.H"

#include "x/w/focusable_container.H"
#include "image_button_internal.H"
#include "icon_images_vector_element.H"
#include "hotspot_element.H"
#include "icon.H"
#include "generic_window_handler.H"
#include "capturefactory.H"
#include "run_as.H"

#include <x/weakcapture.H>

LIBCXXW_NAMESPACE_START

custom_comboboxlayoutmanagerObj
::custom_comboboxlayoutmanagerObj(const ref<implObj> &impl,
				  const ref<listlayoutmanagerObj::implObj>
				  &list_layout_impl)
	// Surprise! Our implementation object is attached to the combobox
	// container, but our superclass is a listlayoutmanager whose
	// implementation object is the popup's listlayoutmanager implementation
	// object.
	//
	// It is expected that most of the activity here will impact the
	// combobox popup, so naturally when we go out of scope and get
	// destroyed it'll be the popup's container that'll get tickled for
	// recalculation.
	: listlayoutmanagerObj(list_layout_impl),
	  impl(impl)
{
}

custom_comboboxlayoutmanagerObj::~custom_comboboxlayoutmanagerObj()=default;

/////////////////////////////////////////////////////////////////////////////
//
// Combobox button.

typedef focusframecontainer_elementObj<container_elementObj<child_elementObj>
				       > combobox_button_focusframe_container_t;


// Create the combobox button.

// The factory where the combobox button gets created gets passed in.

static inline auto
create_combobox_button(const ref<containerObj::implObj> &parent_container,
		       const custom_combobox_popup &popup_handler)
{
	// The actual element that will go into this factory will be the
	// focus frame, with the button inside it. First, construct
	// the focus frame implementation object.

	auto cbfc=ref<combobox_button_focusframe_container_t>
		::create(parent_container,
			 child_element_init_params{"focusframe@libcxx"});

	// The focus frame implementation object is the parent of the
	// combobox button. Create its implementatio n button.

	auto &d=cbfc->get_element_impl().get_window_handler();

	auto icon1=d.create_icon("scroll-down1", render_repeat::none, 0, 0,
				 icon_scale::nomore);
	auto icon2=d.create_icon("scroll-down2", render_repeat::none, 0, 0,
				 icon_scale::nomore);

	auto image_button_internal_impl=
		ref<combobox_button_implObj>
		::create(cbfc,
			 std::vector<icon>{ icon1, icon2 },
			 popup_handler);

	// We can now create the focusframe public object.

	auto ff=focusframecontainer::create(cbfc, image_button_internal_impl,
					    "inputfocusoff_border",
					    "inputfocuson_border");

	// The focus frame's factory, where the focusable element, the
	// image button, gets created.
	auto focusframe_factory=ff->set_focusable();

	// Create the "public" object, show() it, and tell the focus frame:
	// here's what you hafe inside you.
	auto combobox_button=image_button_internal
		::create(image_button_internal_impl);

	combobox_button->show();

	focusframe_factory->created_internally(combobox_button);

	ff->show();

	ff->label_for(ff); // Make clicks on the focusframe work.

	return std::make_tuple(ff, combobox_button);
}

/////////////////////////////////////////////////////////////////////////////

new_custom_comboboxlayoutmanager
::new_custom_comboboxlayoutmanager(const custom_combobox_selection_factory_t
				   &selection_factory,
				   const combobox_selection_changed_t
				   &selection_changed)
	: selection_factory(selection_factory),
	  selection_changed(selection_changed)
{
}

new_custom_comboboxlayoutmanager::~new_custom_comboboxlayoutmanager()=default;

focusable_container new_custom_comboboxlayoutmanager
::create(const ref<containerObj::implObj> &parent) const
{
	// Start by creating the popup first.
	//
	// First, the handler.

	auto parent_handler=ref<generic_windowObj::handlerObj>
		(&parent->get_window_handler());

	auto attachedto_info=popup_attachedto_info::create
		(rectangle{},
		 attached_to::combobox_above_or_below);

	auto popup_handler=custom_combobox_popup
		::create(parent_handler, attachedto_info,
			 // We're about to create the combobox container,
			 // with nesting_level of parent+1
			 //
			 // The current selection element in the combox
			 // container will be parent+2.
			 //
			 // Need to set the popup's nesting level to
			 // parent+3, so that it gets recalculated after
			 // the popup gets recalculated.
			 parent->get_element_impl().nesting_level+3);

	popup_handler->set_window_type("combo,popup_menu,dropdown_menu");
	popup_handler->elementObj::implObj
		::set_background_color("combobox_background_color");

	new_listlayoutmanager style;

	custom_combobox_popup_containerptr popup_containerptr;
	ptr<custom_combobox_popup_layoutmanagerObj> popup_listlayoutmanagerptr;

	peephole_style combobox_peephole_style;

	combobox_peephole_style.h_alignment=halign::fill;

	// Create the popup's peephole, in case the popup is too big for the
	// screen.
	auto popup_toplevel_layoutmanager=create_peephole_toplevel
		(popup_handler,
		 "combobox_border",
		 combobox_peephole_style,
		[&]
		 (const auto &peephole_container)
		 {
			 auto impl=ref<custom_combobox_popup_containerObj
			 ::implObj>::create(peephole_container, style);

			 auto popup_listlayoutmanager=
			 ref<custom_combobox_popup_layoutmanagerObj>
			 ::create(impl, style);

			 auto popup=custom_combobox_popup_container
			 ::create(impl,
				  popup_listlayoutmanager,
				  attachedto_info);

			 popup->show();
			 popup_containerptr=popup;
			 popup_listlayoutmanagerptr=popup_listlayoutmanager;

			 return popup;
		 });

	// Now, finish creating the popup's implementation object, and the
	// popup "public" object itself.
	custom_combobox_popup_container popup_container=popup_containerptr;
	ref<custom_combobox_popup_layoutmanagerObj> popup_listlayoutmanager=
		popup_listlayoutmanagerptr;

	auto popup_impl=ref<popupObj::implObj>::create(popup_handler,
						       parent_handler);

	auto combobox_popup=popup::create(popup_impl,
					  popup_toplevel_layoutmanager->impl);

	// We can now start creating the combobox display element, starting
	// with the container where everything goes.
	auto combobox_container_impl=
		ref<custom_combobox_containerObj::implObj>
		::create(parent, popup_container,
			 popup_handler);

	combobox_container_impl->elementObj::implObj
		::set_background_color("combobox_background_color");

	// And the layout manager.
	auto lm=ref<custom_comboboxlayoutmanagerObj::implObj>
		::create(combobox_container_impl, *this);

	auto glm=lm->create_gridlayoutmanager();

	// The combo-box's internal grid layout manager will have one row
	// with the current selection element, and the button element.
	auto f=glm->append_row();

	f->padding(0);
	f->border("combobox_border");
	f->valign(valign::middle);

	// Invoke the callback to construct the current selection element,
	// which is automatically show()n.

	auto capture_current_selection=
		capturefactory::create(combobox_container_impl);

	selection_factory(capture_current_selection);

	auto current_selection=capture_current_selection->get();

	current_selection->show_all();

	f->padding(0);
	f->border("combobox_border");
	f->valign(valign::middle);

	f->created_internally(current_selection);

	// Now for the combo-button.
	f->padding(0);
	f->border("combobox_border");
	f->halign(halign::fill);
	f->valign(valign::fill);

	auto ret=create_combobox_button(f->container_impl,
					popup_handler);

	// TODO: structured bindings

	auto &ff=std::get<0>(ret);
	auto &combobox_button=std::get<1>(ret);

	f->created_internally(ff);

	// Point the popup container to the current selection element and
	// the combo-box button element, so both can be sized based on the
	// size of the combo-box's items.
	popup_container->impl
		->set_current_combobox_selection_element_and_button
		(current_selection, combobox_button);

	auto c=custom_combobox_container::create(combobox_container_impl,
						 lm,
						 combobox_button,
						 combobox_popup);

	c->elementObj::impl->THREAD->run_as
		([=, selection_changed=this->selection_changed]
		 (IN_THREAD_ONLY)
		 {
			 // Install:

			 // 1. The popup handler as the combobox container's
			 // attached_popup.
			 //
			 // 2. The combo-box popup's list layout manager
			 // selection_changed callback, to forward to the
			 // combo-box selection_changed callback (must be
			 // weakly-captured to avoid a circular reference).

			 combobox_container_impl->data(IN_THREAD)
				 .attached_popup=popup_handler;

			 popup_listlayoutmanager->selection_changed(IN_THREAD)=
				 [=, current_selection=make_weak_capture
				  (current_selection, combobox_popup)]
				 (list_lock &lock,
				  const listlayoutmanager &llm,
				  size_t i,
				  bool flag,
				  const busy &mcguffin)
				 {
					 current_selection.get
					 ([&]
					  (const auto &e,
					   const auto &combobox_popup) {

						 selection_changed(lock, llm,
								   i, flag,
								   e,
								   combobox_popup,
								   mcguffin);
					 });
				 };

			 popup_handler->parent_element_window(IN_THREAD)=
				 parent_handler;
		 });

	return c;
}

LIBCXXW_NAMESPACE_END