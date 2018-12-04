/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image_button.H"
#include "image_button_internal.H"
#include "image_button_internal_impl.H"
#include "x/w/impl/focus/standard_focusframecontainer_element.H"
#include "x/w/impl/focus/standard_focusframecontainer_element_impl.H"
#include "icon.H"
#include "busy.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "x/w/factory.H"
#include "x/w/button_event.H"
#include "x/w/key_event.H"
#include "x/w/impl/container_element.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/always_visible_element.H"
#include "messages.H"
#include "generic_window_handler.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

image_button_config::image_button_config()
	: alignment{valign::middle},
	  focusoff_border{"thin_inputfocusoff_border"},
	  focuson_border{"thin_inputfocuson_border"}

{
}

image_button_config::~image_button_config()=default;

void image_button_config::visible_focusoff_border()
{
	focusoff_border="thin_inputfocusoff_border_color2";
}

checkbox_config::checkbox_config()
	: images{"checkbox1", "checkbox2", "checkbox3"}
{
}

checkbox_config::~checkbox_config()=default;

radio_config::radio_config()
	: images{"radio1", "radio2"}
{
}

radio_config::~radio_config()=default;

create_image_button_info::~create_image_button_info()=default;

image_buttonObj::image_buttonObj(const ref<implObj> &impl,
				 const container_impl
				 &container_impl,
				 const layout_impl &lm_impl)
	: containerObj(container_impl, lm_impl),
	  impl(impl)
{
}

image_buttonObj::~image_buttonObj()=default;

focusable_impl image_buttonObj::get_impl() const
{
	return impl->button->impl;
}

size_t image_buttonObj::get_value() const
{
	return impl->button->impl->get_image_number();
}

void image_buttonObj::set_value(size_t n)
{
	element_impl e=impl->button->impl;

	e->get_window_handler().thread()->run_as
		([impl=this->impl, n]
		 (ONLY IN_THREAD)
		 {
			 impl->button->impl->set_image_number(IN_THREAD,
							      {}, n);
		 });
}

void image_buttonObj::on_activate(const image_button_callback_t &callback)
{
	element_impl e=impl->button->impl;

	e->get_window_handler().thread()->run_as
		([&, impl=this->impl, callback]
		 (ONLY IN_THREAD)
		 {
			 auto i=impl->button->impl;

			 i->current_callback(IN_THREAD)=callback;

			 try {
				 callback(IN_THREAD,
					  i->get_image_number(),
					  initial{},
					  busy_impl{*i});
			 } REPORT_EXCEPTIONS(impl->button->impl);
		 });
}

void image_buttonObj::do_update_label(const function<void (const factory &)> &f)
{
	gridlayoutmanager glm=get_layoutmanager();

	glm->remove(0, 1);
	auto factory=glm->append_columns(0);

	factory->top_padding(0);
	factory->bottom_padding(0);
	factory->right_padding(0);

	f(factory);
}

//////////////////////////////////////////////////////////////////////////////

namespace {
#if 0
}
#endif

class LIBCXX_HIDDEN scroll_imagebuttonObj
	: public image_button_internalObj::implObj {

 public:

	using image_button_internalObj::implObj::implObj;

	~scroll_imagebuttonObj()=default;

	//! Override temperature_changed()

	//! Flash the scroll icons when clicking on them.

	void temperature_changed(ONLY IN_THREAD,
				 const callback_trigger_t &trigger) override
	{
		image_button_internalObj::implObj::temperature_changed
			(IN_THREAD, trigger);

		set_image_number(IN_THREAD,
				 trigger,
				 hotspot_temperature(IN_THREAD)
				 == temperature::hot ? 1:0);
	}
};

#if 0
{
#endif
}

ref<image_button_internalObj::implObj>
scroll_imagebutton_specific_height(const container_impl
				   &parent_container,
				   const std::string &image1,
				   const std::string &image2,
				   const dim_arg &height_arg)
{
	auto &wh=parent_container->get_window_handler();

	image_button_internal_impl_init_params init_params
		{
		 parent_container,
		 {
		  wh.create_icon({image1, render_repeat::none, 0, height_arg}),
		  wh.create_icon({image2, render_repeat::none, 0, height_arg}),
		 }
		};

	return ref<scroll_imagebuttonObj>::create(init_params);
}

namespace {
#if 0
}
#endif

///////////////////////////////////////////////////////////////////////////
//
// This is the container implementation button for the image_buttonObj's
// container superclass.
typedef container_visible_elementObj<container_elementObj<child_elementObj>
				     >image_button_container_superclass_t;

class LIBCXX_HIDDEN image_button_containerObj
	: public image_button_container_superclass_t {

 public:

	const bool disable_recursive_visibility;

	image_button_containerObj(bool disable_recursive_visibility,
				  const container_impl
				  &parent_container,
				  const child_element_init_params &init_params)
		: image_button_container_superclass_t(parent_container,
						      init_params),
		disable_recursive_visibility{disable_recursive_visibility}
	{
	}

	~image_button_containerObj()=default;

	//! Override request_visibility_recursive

	//! Forward recursive visibility to the label element, if there is one.
	//!
	//! We're going to show() the internal image button focus frame
	//! and the internal image button element, so it's always technically
	//! visible, so a mere show() on the image button will reveal
	//! everything.

	void request_visibility_recursive(ONLY IN_THREAD, bool flag)
		override
	{
		if (disable_recursive_visibility)
			return;

		request_visibility(IN_THREAD, flag);

		invoke_layoutmanager
			([&]
			 (const ref<gridlayoutmanagerObj::implObj> &glm)
			 {
				 auto e=glm->get(0, 1);

				 if (!e)
					 return;

				 e->impl->request_visibility_recursive
					 (IN_THREAD, flag);
			 });
	}

};

#if 0
{
#endif
}

// The factory do_create_image_button invokes to contsruct the internal
// implementation object.

image_button
do_create_image_button(const create_image_button_info &info,
		       const function<image_button_internal_factory_t>
		       &img_impl_factory,
		       const function<void (const factory &)> &label_factory)
{
	// Create an image_button_containerObj, a container with a grid
	// layout manager.
	child_element_init_params init_params{"focusframe@libcxx.com"};

	init_params.background_color=info.button_background_color;

	auto image_button_outer_container_impl=
		info.always_visible ?
		ref<image_button_containerObj>
		{
		 ref<always_visible_elementObj<image_button_containerObj>>
		 ::create(info.disable_recursive_visibility,
			  info.parent_container_impl, init_params)
		} : ref<image_button_containerObj>
			    ::create(info.disable_recursive_visibility,
				     info.parent_container_impl, init_params);

	ref<gridlayoutmanagerObj::implObj> image_button_outer_container_layout=
		new_gridlayoutmanager{}
	.create(image_button_outer_container_impl);

	auto glm=image_button_outer_container_layout->create_gridlayoutmanager();

	glm->row_alignment(0, info.config.alignment);

	// If there's going to be a label, it gets all extra space.
	glm->requested_col_width(1, 100);

	// This grid layout manager will contain a single focusframecontainer.

	auto focus_frame_impl=
		create_always_visible_focusframe_impl
		(image_button_outer_container_impl,
		 info.config.focusoff_border,
		 info.config.focuson_border, 0, 0);

	// Create an image_button_internal implementation object. Its
	// container is the focusframecontainer.

	auto ibii=img_impl_factory(focus_frame_impl);

	// And create the image_button_internal "public" object.
	auto ibi=image_button_internal::create(ibii);

	// Now, create the focusframecontainer object, pointing it to the
	// image_button_internalObj as its focusable implementation object,
	// with the image_button_internal inside the focusframe.
	//
	// We will show() the focusframe, and the outer container.
	// The outer container implementation inherits from
	// nonrecursive_visibilityObj, so its show_all/hide_all() will not
	// recursively show/hide the internal display elements.

	auto focus_frame=create_focusframe_container_owner
		(focus_frame_impl,
		 focus_frame_impl,
		 ibi, ibii);

	// Now, let's get back to our internal gridlayoutmanager, where we
	// "create" the focusframecontainer.

	auto button_factory=glm->append_row();
	button_factory->padding(0).created_internally(focus_frame);

	auto impl=ref<image_buttonObj::implObj>::create(ibi);

	auto b=image_button::create(impl, image_button_outer_container_impl,
				    image_button_outer_container_layout);

	// The internal grid layout manager does not introduce any of its own
	// padding, but keep the left padding, to separate the image button
	// from its label.

	button_factory->top_padding(0);
	button_factory->bottom_padding(0);
	button_factory->right_padding(0);
	label_factory(button_factory);

	if (info.click_anywhere)
		b->label_for(b);
	else
		focus_frame->label_for(ibi);
	return b;
}

image_button factoryObj::create_checkbox(const checkbox_config &config)
{
	return create_checkbox([](const auto &) {}, config);
}

image_button factoryObj::do_create_checkbox(const function<factory_creator_t>
					    &label_factory,
					    const checkbox_config &config)
{
	auto icons=get_element_impl().get_window_handler()
		.create_icon_vector(config.images);

	if (icons.empty())
		throw EXCEPTION(_("Attempt to create a checkbox without any images."));

	auto im=create_image_button_with_label_factory
		({get_container_impl(), false, config},
		 [&]
		 (const auto &container)
		 {
			 return create_checkbox_impl(container, icons);
		 }, label_factory);
	created_internally(im);
	return im;
}

image_button factoryObj::create_radio(const radio_group &group,
				      const radio_config &config)
{
	return create_radio(group, [](const auto &){}, config);
}

image_button factoryObj::do_create_radio(const radio_group &group,
					 const function<factory_creator_t>
					 &label_creator,
					 const radio_config &config)
{
	auto icons=get_element_impl().get_window_handler()
		.create_icon_vector(config.images);

	if (icons.empty())
		throw EXCEPTION(_("Attempt to create a radio button without any images."));


	auto b=create_image_button_with_label_factory
		({get_container_impl(), false, config},
		 [&]
		 (const auto &container)
		 {
			 return create_radio_impl(group,
						  container,
						  icons);
		 }, label_creator);
	created_internally(b);
	return b;
}

LIBCXXW_NAMESPACE_END
