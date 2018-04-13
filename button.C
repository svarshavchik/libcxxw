/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "button.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/background_color.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "x/w/border_arg.H"
#include "x/w/impl/container_element.H"
#include "container_visible_element.H"
#include "hotspot_bgcolor_element.H"
#include "x/w/impl/always_visible.H"
#include "focus/focusframecontainer_element.H"
#include "generic_window_handler.H"
#include "xid_t.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

// The implementation of the button is a mixin that combines
//
// A hotspot with background colors.
//
// The hotspot is inside a focusframecontainer
//
// Which is a container, derived from child_elementObj, since we must be
// somebody's child element.

typedef hotspot_bgcolor_elementObj<always_visibleObj<
	focusframecontainer_elementObj<
		container_elementObj
		<child_elementObj>>>> ff_impl_t;

class LIBCXX_HIDDEN button_focusframeObj : public ff_impl_t {

 public:
	using ff_impl_t::ff_impl_t;

	~button_focusframeObj()=default;

	// create_label() in this container will use an action_button font.

	const char *label_theme_font() const override
	{
		return "button";
	}
};

struct LIBCXX_HIDDEN buttonObj::internal_construction_info {

	ref<gridlayoutmanagerObj::implObj> glmi;

	ref<ff_impl_t> ff_impl;
};

typedef ref<button_focusframeObj> button_focusframe;

typedef factoryObj::factory_creator_t factory_creator_t;

// We get:
//
// A created implementation object.
//
// button border: normal or special.
//
// Factory for creating the contents of the button.

static buttonObj::internal_construction_info
create_button_focusframe(const ref<buttonObj::implObj> &impl,
			 const border_arg &left_border,
			 const border_arg &right_border,
			 const border_arg &top_border,
			 const border_arg &bottom_border,
			 const color_arg &normal_color,
			 const color_arg &selected_color,
			 const color_arg &active_color,
			 const function<factory_creator_t> &creator)
{
	// Create the grid layout manager that the button uses
	// for its real contents: the focusframecontainer.

	auto glmi=ref<gridlayoutmanagerObj::implObj>::create(impl);

	auto glm=glmi->create_gridlayoutmanager();

	// Now, create the focusframecontainer.
	//
	// Obtain the factory for its contents, via set_focusable(), and
	// pass it to the factory that's supposed to populate the
	// focusframecontainer. Ergo, the factory user that thinks will
	// be populating the contents of the button ends up populating
	// the contents of the focusframecontainer.

	auto ffi=button_focusframe
		::create(normal_color,
			 selected_color,
			 active_color,
			 "inputfocusoff_border",
			 "inputfocuson_border",
			 glmi->layout_container_impl,
			 child_element_init_params{"focusframe@libcxx.com"});

	auto ff=focusframecontainer::create(ffi, ffi);

	// Call the application-provided creator to populate the contents
	// of the button.

	creator(ff->set_focusable());

	// Now, it's time to go back to the new button's grid
	// layout manager, and insert the fully-cooked focusframecontainer.

	auto factory=glm->append_row();

	factory->padding(0);	// No padding for the focus frame.
	factory->left_border(left_border); // And set its border, too.
	factory->right_border(right_border);
	factory->top_border(top_border);
	factory->bottom_border(bottom_border);

	// If the button is placed in a grid cell that fill()ed, it will
	// stretch the button element. Stretch the focusframe too, in that
	// case.
	factory->halign(halign::fill);
	factory->valign(valign::fill);

	factory->created_internally(ff);

	return {glmi, ffi};
}

buttonObj::buttonObj(const ref<implObj> &impl,
		     const internal_construction_info &ici)
	: containerObj(impl, ici.glmi),
	  hotspot_bgcolorObj(ici.ff_impl),
	  impl(impl)
{
}

buttonObj::~buttonObj()=default;

focusable_impl buttonObj::get_impl() const
{
	const_gridlayoutmanager m=get_layoutmanager();

	focusframecontainer ffc=m->get(0, 0);

	return ffc->impl;
}

button factoryObj::do_create_normal_button(const function<factory_creator_t> &f)
{
	return do_create_normal_button(f, {});
}

button factoryObj::do_create_normal_button(const function<factory_creator_t> &f,
					   const shortcut &sk)
{
	return do_create_button("normal_button_border", f, sk);
}

button factoryObj::do_create_special_button(const function<factory_creator_t>&f)
{
	return do_create_special_button(f, {});
}

button factoryObj::do_create_special_button(const function<factory_creator_t>&f,
					    const shortcut &sk)
{
	return do_create_button("special_button_border", f, sk);
}

button factoryObj::do_create_button(const border_arg &theme_border,
				    const function<factory_creator_t> &f)
{
	return do_create_button(theme_border, f, {});
}

button factoryObj::do_create_button(const border_arg &theme_border,
				    const function<factory_creator_t> &f,
				    const shortcut &shortcut_key)
{
	return do_create_button_with_explicit_borders
		(*this, theme_border, theme_border, theme_border, theme_border,
		 "button_normal_color",
		 "button_selected_color",
		 "button_active_color",
		 f, shortcut_key, {});
}

button do_create_button_with_explicit_borders
(factoryObj &f,
 const border_arg &left_border,
 const border_arg &right_border,
 const border_arg &top_border,
 const border_arg &bottom_border,
 const color_arg &normal_color,
 const color_arg &selected_color,
 const color_arg &active_color,
 const function<factory_creator_t> &creator,
 const shortcut &shortcut_key,
 const child_element_init_params &init_params)
{
	auto impl=ref<buttonObj::implObj>::create(f.get_container_impl(),
						  init_params);

	auto ab=button::create(impl,
			       create_button_focusframe
			       (impl, left_border, right_border,
				top_border, bottom_border,
				normal_color, selected_color, active_color,
				creator));

	// Left to its own devices, the real focusable element is the internal
	// container inside the focus frame. Pointer clicks just outside of it,
	// on the focus frame, but inside the exterior borders, won't register.
	//
	// Simple solution. The whole button is the label for the focus frame.

	ab->label_for(ab);

	static_cast<elementObj::implObj &>(*impl)
		.get_window_handler().thread()->run_as
		([shortcut_key,
		  hotspot_impl=ab->hotspotObj::impl]
		 (ONLY IN_THREAD)
		 {
			 hotspot_impl->set_shortcut(IN_THREAD, shortcut_key);
		 });

	f.created_internally(ab);
	return ab;
}

button
factoryObj::create_normal_button_with_label(const text_param &text,
					    halign alignment)
{
	return create_normal_button_with_label(text,
					       {},
					       alignment);
}

button
factoryObj::create_normal_button_with_label(const text_param &text,
					    const shortcut &shortcut_key,
					    halign alignment)
{
	return create_button_with_label("normal_button_border",
					text, shortcut_key, alignment);
}

button
factoryObj::create_special_button_with_label(const text_param &text,
					     halign alignment)
{
	return create_special_button_with_label(text,
						{},
						alignment);
}

button
factoryObj::create_special_button_with_label(const text_param &text,
					     const shortcut &shortcut_key,
					     halign alignment)
{
	return create_button_with_label("special_button_border",
					text, shortcut_key, alignment);
}

button
factoryObj::create_button_with_label(const border_arg &theme_border,
				     const text_param &text,
				     const shortcut &shortcut_key,
				     halign alignment)
{
	return do_create_button
		(theme_border,
		 make_function<factory_creator_t>
		 ([&text, alignment]
		  (const auto &f)
		  {
			  f->create_label(text, alignment)->show();
		  }),
		 shortcut_key);
}

LIBCXXW_NAMESPACE_END
