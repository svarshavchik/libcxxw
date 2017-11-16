/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image_button.H"
#include "image_button_internal.H"
#include "image_button_internal_impl.H"
#include "focus/focusframecontainer.H"
#include "focus/standard_focusframecontainer_element.H"
#include "icon.H"
#include "busy.H"
#include "x/w/factory.H"
#include "x/w/button_event.H"
#include "x/w/key_event.H"
#include "container_element.H"
#include "gridlayoutmanager.H"
#include "container_visible_element.H"
#include "messages.H"
#include "generic_window_handler.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

LOG_FUNC_SCOPE_DECL(INSERT_LIBX_NAMESPACE::w::image_button, image_log);

image_buttonObj::image_buttonObj(const ref<implObj> &impl,
				 const ref<containerObj::implObj>
				 &container_impl,
				 const ref<layoutmanagerObj::implObj> &lm_impl)
	: containerObj(container_impl, lm_impl),
	  impl(impl)
{
}

image_buttonObj::~image_buttonObj()=default;

ref<focusableImplObj> image_buttonObj::get_impl() const
{
	return impl->button->impl;
}

size_t image_buttonObj::get_value() const
{
	return impl->button->impl->get_image_number();
}

void image_buttonObj::set_value(size_t n)
{
	elementimpl e=impl->button->impl;

	e->get_window_handler().IN_THREAD->run_as
		([impl=this->impl, n]
		 (IN_THREAD_ONLY)
		 {
			 impl->button->impl->set_image_number(IN_THREAD,
							      {}, n);
		 });
}

void image_buttonObj::on_activate(const image_button_callback_t &callback)
{
	LOG_FUNC_SCOPE(image_log);

	elementimpl e=impl->button->impl;

	e->get_window_handler().IN_THREAD->run_as
		([&, impl=this->impl, callback]
		 (IN_THREAD_ONLY)
		 {
			 auto i=impl->button->impl;

			 i->current_callback(IN_THREAD)=callback;

			 try {
				 callback(i->get_image_number(),
					  initial{},
					  busy_impl{*i});
			 } CATCH_EXCEPTIONS;
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

///////////////////////////////////////////////////////////////////////////
//
// This is the container implementation button for the image_buttonObj's
// container superclass.
typedef container_visible_elementObj<container_elementObj<child_elementObj>
				     >image_button_container_superclass_t;

class LIBCXX_HIDDEN image_button_containerObj
	: public image_button_container_superclass_t {

 public:

	image_button_containerObj(const ref<containerObj::implObj>
				  &parent_container)
		: image_button_container_superclass_t(parent_container,
						      {"focusframe@libcxx"})
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

	void request_visibility_recursive(IN_THREAD_ONLY, bool flag)
		override
	{
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

// The factory do_create_image_button invokes to contsruct the internal
// implementation object.

typedef ref<image_button_internalObj::implObj>
image_button_factory_t(const ref<containerObj::implObj> &,
		       const std::vector<icon> &);

static image_button
do_create_image_button(const std::vector<std::string_view>
		       &images,
		       const function<image_button_factory_t> &img_impl_factory,
		       factoryObj &f,
		       valign alignment,

		       const function<void (const factory &)> &label_factory)
{
	if (images.empty())
		throw EXCEPTION(_("Attempt to create an image button without any images."));

	auto icons=f.container_impl->get_window_handler()
		.create_icon_vector(images);

	// Create an image_button_containerObj, a container with a grid
	// layout manager.

	auto image_button_outer_container_impl=
		ref<image_button_containerObj>::create(f.container_impl);

	auto image_button_outer_container_layout=
		ref<gridlayoutmanagerObj::implObj>
		::create(image_button_outer_container_impl);

	auto glm=image_button_outer_container_layout->create_gridlayoutmanager();

	glm->row_alignment(0, alignment);

	// If there's going to be a label, it gets all extra space.
	glm->requested_col_width(1, 100);

	// This grid layout manager will contain a single focusframecontainer.

	auto focus_frame_impl=
		create_standard_focusframe_container_element
		(image_button_outer_container_impl);

	// Create an image_button_internal implementation object. Its
	// container is the focusframecontainer.

	auto ibii=img_impl_factory(focus_frame_impl, icons);

	// And create the image_button_internal "public" object.
	auto ibi=image_button_internal::create(ibii);

	// Now, create the focusframecontainer object, pointing it to the
	// image_button_internalObj as its focusable implementation object.

	auto focus_frame=focusframecontainer::create(focus_frame_impl,
						     ibii,
						     "thin_inputfocusoff_border",
						     "thin_inputfocuson_border");

	// And "create" the image_button_internal inside the focusframe.

	// We will show() the focusframe, and the outer container.
	// The outer container implementation inherits from
	// nonrecursive_visibilityObj, so its show_all/hide_all() will not
	// recursively show/hide the internal display elements.

	auto focus_frame_factory=focus_frame->set_focusable();

	focus_frame_factory->created_internally(ibi);
	ibi->show();

	// Now, let's get back to our internal gridlayoutmanager, where we
	// "create" the focusframecontainer.

	auto button_factory=glm->append_row();
	button_factory->padding(0).created_internally(focus_frame);
	focus_frame->show();

	auto impl=ref<image_buttonObj::implObj>::create(ibi);

	auto b=image_button::create(impl, image_button_outer_container_impl,
				    image_button_outer_container_layout);

	f.created_internally(b);

	// The internal grid layout manager does not introduce any of its own
	// padding, but keep the left padding, to separate the image button
	// from its label.

	button_factory->top_padding(0);
	button_factory->bottom_padding(0);
	button_factory->right_padding(0);
	label_factory(button_factory);

	b->label_for(b);
	return b;
}

template<typename functor1>
static inline image_button
create_image_button(const std::vector<std::string_view>
		    &images,
		    functor1 &&creator,
		    factoryObj &f,
		    valign alignment,
		    const function<void (const factory &)> &label_factory)
{
	return do_create_image_button(images,
				      make_function<image_button_factory_t>
				      (std::forward<functor1>(creator)),
				      f, alignment,
				      label_factory);
}

image_button factoryObj::create_checkbox(valign alignment)
{
	return create_checkbox([](const auto &) {}, alignment);
}

image_button factoryObj::do_create_checkbox(const function<factory_creator_t>
					    &label_factory,
					    valign alignment)
{

	return do_create_checkbox(label_factory,
				  {"checkbox1", "checkbox2", "checkbox3"},
				  alignment);
}

// Call create_image_button, using create_checkbox_impl() to create the
// internal image button.

image_button factoryObj::create_checkbox(const std::vector<std::string_view>
					 &images,
					 valign alignment)
{
	return create_checkbox([](const factory &){}, images, alignment);
}

image_button factoryObj::do_create_checkbox(const function<factory_creator_t>
					    &label_factory,
					    const std::vector<std::string_view>
					    &images,
					    valign alignment)
{

	return create_image_button(images, create_checkbox_impl, *this,
				   alignment, label_factory);
}

image_button factoryObj::create_radio(const radio_group &group,
				      valign alignment)
{
	return create_radio(group, [](const auto &){}, alignment);
}

image_button factoryObj::do_create_radio(const radio_group &group,
					 const function<factory_creator_t>
					 &label_creator,
					 valign alignment)
{

	return do_create_radio(group, label_creator,
			       {"radio1", "radio2"}, alignment);
}

// Call create_image_button, using create_radio_impl() to create the
// internal image button.

image_button factoryObj::create_radio(const radio_group &group,
				      const std::vector<std::string_view>
				      &images,
				      valign alignment)
{
	return create_radio(group, [](const auto &) {}, images, alignment);
}

image_button factoryObj::do_create_radio(const radio_group &group,
					 const function<factory_creator_t>
					 &label_creator,
					 const std::vector<std::string_view>
					 &images,
					 valign alignment)
{
	return create_image_button(images,
				   [&]
				   (const auto &container,
				    const auto &images)
				   {
					   return create_radio_impl(group,
								    container,
								    images);
				   }, *this, alignment, label_creator);
}

LIBCXXW_NAMESPACE_END
