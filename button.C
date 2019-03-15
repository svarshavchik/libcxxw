/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "button.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/background_color.H"
#include "x/w/factory.H"
#include "x/w/button.H"
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

const button_config &normal_button()
{
	static const button_config config={
		"button"_theme_font,
		"button_normal_color",
		"button_selected_color",
		"button_active_color",

		"normal_button_border",
		"normal_button_border",
		"normal_button_border",
		"normal_button_border",

		"inputfocusoff_border",
		"inputfocuson_border",
	};

	return config;
}

static button_config modify_normal_to_default_button()
{
	auto c=normal_button();

	c.left_border=c.right_border=c.top_border=c.bottom_border=
		"default_button_border";
	return c;
}

const button_config &default_button()
{
	static const button_config config=modify_normal_to_default_button();

	return config;
}

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
			 const button_config &config,
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
		::create(config.button_font,
			 config.normal_color,
			 config.selected_color,
			 config.active_color,
			 config.inputfocusoff_border,
			 config.inputfocuson_border,
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

button do_create_button_with_explicit_borders
(factoryObj &f,
 const button_config &config,
 const function<factory_creator_t> &creator,
 const shortcut &shortcut_key,
 const child_element_init_params &init_params)
{
	auto impl=ref<buttonObj::implObj>::create(config,
						  f.get_container_impl(),
						  init_params);

	auto ab=button::create(impl,
			       create_button_focusframe
			       (impl,
				config,
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

button factoryObj::create_button(const text_param &text)
{
	return create_button(text, {});
}

button factoryObj::create_button(const text_param &text,
				 const create_button_with_label_args_t &args)
{
	std::optional<label_config> default_label_config;

	const auto &opt_label_config=
		optional_arg_or<label_config>(args, default_label_config);

	std::optional<shortcut> default_shortcut;

	const auto &opt_shortcut=
		optional_arg_or<shortcut>(args, default_shortcut);

	const auto &specified_button_config=optional_arg<button_config>(args);

	const button_config &opt_button_config=
		specified_button_config
		? static_cast<const button_config &>(*specified_button_config)
		: normal_button();

	return do_create_button
		(make_function<factory_creator_t>
		 ([&]
		  (const auto &f)
		  {
			  f->create_label(text, opt_label_config)->show();
		  }),
		 {opt_button_config, opt_shortcut});
}

button factoryObj::do_create_button(const function<factory_creator_t> &creator)
{
	return do_create_button(creator, {});
}

button factoryObj::do_create_button(const function<factory_creator_t> &creator,
				    const create_button_args_t &args)
{
	std::optional<shortcut> default_shortcut;

	const auto &opt_shortcut=
		optional_arg_or<shortcut>(args, default_shortcut);

	const auto &specified_button_config=optional_arg<button_config>(args);

	const button_config &opt_button_config=
		specified_button_config
		? static_cast<const button_config &>(*specified_button_config)
		: normal_button();

	return do_create_button_with_explicit_borders
		(*this, opt_button_config,
		 creator, opt_shortcut, {});
}

LIBCXXW_NAMESPACE_END
