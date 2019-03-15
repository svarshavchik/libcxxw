/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "button.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/background_color.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "x/w/border_arg.H"
#include "x/w/text_param_literals.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "hotspot_bgcolor_element.H"
#include "capturefactory.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/focus/focusframecontainer_element.H"
#include "x/w/impl/focus/standard_focusframecontainer_element.H"
#include "generic_window_handler.H"
#include "xid_t.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

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

class LIBCXX_HIDDEN button_focusframeObj;

class button_focusframeObj : public ff_impl_t {

public:
	template<typename ...Args>
	button_focusframeObj(const font_arg &button_theme_font,
			     Args && ...args)
		: ff_impl_t{std::forward<Args>(args)...},
		  button_theme_font{button_theme_font}
	{
	}

	~button_focusframeObj()=default;

	const font_arg button_theme_font;

	// create_label() in this container will use an action_button font.

	font_arg label_theme_font() const override
	{
		return button_theme_font;
	}
};
#if 0
{
#endif
}

struct buttonObj::internal_construction_info {

	ref<borderlayoutmanagerObj::implObj> blmi;

	ref<ff_impl_t> ff_impl;
};

namespace {
#if 0
}
#endif

typedef ref<button_focusframeObj> button_focusframe;

typedef factoryObj::factory_creator_t factory_creator_t;

// We get:
//
// A created implementation object.
//
// button colors.
//
// Factory for creating the contents of the button.

static inline buttonObj::internal_construction_info
create_button_focusframe(const ref<buttonObj::implObj> &impl,
			 const font_arg &button_theme_font,
			 const color_arg &normal_color,
			 const color_arg &selected_color,
			 const color_arg &active_color,
			 const border_arg &inputfocusoff_border,
			 const border_arg &inputfocuson_border,
			 const function<factory_creator_t> &creator)
{
	// Now, create the focusframecontainer.
	//
	// Obtain the factory for its contents, via set_focusable(), and
	// pass it to the factory that's supposed to populate the
	// focusframecontainer. Ergo, the factory user that thinks will
	// be populating the contents of the button ends up populating
	// the contents of the focusframecontainer.

	auto ffi=button_focusframe
		::create(button_theme_font,
			 normal_color,
			 selected_color,
			 active_color,
			 inputfocusoff_border,
			 inputfocuson_border,
			 0,
			 0,
			 impl, impl,
			 child_element_init_params{"focusframe@libcxx.com"});

	// Call the application-provided creator to populate the contents
	// of the button.

	auto cf=capturefactory::create(ffi);

	creator(cf);

	auto ff=create_focusframe_container_owner(ffi, ffi, cf->get(), ffi);

	// Create the border layout manager for the border
	// for its real contents: the focusframecontainer.

	auto blmi=ref<borderlayoutmanagerObj::implObj>
		::create(impl, impl, ff,
			 halign::fill, valign::fill);

	return {blmi, ffi};
}
#if 0
{
#endif
}

buttonObj::buttonObj(const ref<implObj> &impl,
		     const internal_construction_info &ici)
	: containerObj{impl, ici.blmi},
	  hotspot_bgcolorObj{ici.ff_impl},
	  impl{impl}
{
}

buttonObj::~buttonObj()=default;

focusable_impl buttonObj::get_impl() const
{
	focusableptr ffc;

	containerObj::impl->invoke_layoutmanager
		([&]
		 (const ref<singletonlayoutmanagerObj::implObj> &impl)
		 {
			 ffc=impl->get();
		 });

	return ffc->get_impl();
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
	auto impl=ref<buttonObj::implObj>::create(left_border, right_border,
						  top_border,
						  bottom_border,
						  f.get_container_impl(),
						  init_params);

	auto ab=button::create(impl,
			       create_button_focusframe
			       (impl,
				"button"_theme_font,
				normal_color, selected_color, active_color,
				"inputfocusoff_border",
				"inputfocuson_border",
				creator));

	// Left to its own devices, the real focusable element is the internal
	// container inside the focus frame. Pointer clicks just outside of it,
	// on the focus frame, but inside the exterior borders, won't register.
	//
	// Simple solution. The whole button is the label for the focus frame.

	ab->label_for(ab);

	ab->hotspotObj::impl->set_shortcut(shortcut_key);

	f.created_internally(ab);
	return ab;
}

button
factoryObj::create_normal_button_with_label(const text_param &text)
{
	return create_normal_button_with_label(text, label_config{});
}

button
factoryObj::create_normal_button_with_label(const text_param &text,
					    const label_config &config)
{
	return create_normal_button_with_label(text,
					       {},
					       config);
}

button
factoryObj::create_normal_button_with_label(const text_param &text,
					    const shortcut &shortcut_key)
{
	return create_normal_button_with_label(text, shortcut_key, {});
}

button
factoryObj::create_normal_button_with_label(const text_param &text,
					    const shortcut &shortcut_key,
					    const label_config &config)
{
	return create_button_with_label("normal_button_border",
					text, shortcut_key, config);
}

button
factoryObj::create_special_button_with_label(const text_param &text)
{
	return create_special_button_with_label(text, label_config{});
}

button
factoryObj::create_special_button_with_label(const text_param &text,
					     const label_config &config)
{
	return create_special_button_with_label(text,
						{},
						config);
}

button
factoryObj::create_special_button_with_label(const text_param &text,
					     const shortcut &shortcut_key)
{
	return create_special_button_with_label(text, shortcut_key, {});
}

button
factoryObj::create_special_button_with_label(const text_param &text,
					     const shortcut &shortcut_key,
					     const label_config &config)
{
	return create_button_with_label("special_button_border",
					text, shortcut_key, config);
}

button
factoryObj::create_button_with_label(const border_arg &theme_border,
				     const text_param &text,
				     const shortcut &shortcut_key)
{
	return create_button_with_label(theme_border, text, shortcut_key, {});
}

button
factoryObj::create_button_with_label(const border_arg &theme_border,
				     const text_param &text,
				     const shortcut &shortcut_key,
				     const label_config &config)
{
	return do_create_button
		(theme_border,
		 make_function<factory_creator_t>
		 ([&]
		  (const auto &f)
		  {
			  f->create_label(text, config)->show();
		  }),
		 shortcut_key);
}

layout_impl buttonObj::get_layout_impl() const
{
	layout_implptr l;

	containerObj::impl->invoke_layoutmanager
		([&]
		 (const ref<singletonlayoutmanagerObj::implObj> &border)
		 {
			 container focusframe=border->get();

			 l=focusframe->get_layout_impl();
		 });

	return l;
}

singletonlayoutmanager buttonObj::get_layoutmanager()
{
	return containerObj::get_layoutmanager();
}

const_singletonlayoutmanager buttonObj::get_layoutmanager() const
{
	return containerObj::get_layoutmanager();
}

LIBCXXW_NAMESPACE_END
