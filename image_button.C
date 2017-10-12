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
#include "container_element.H"
#include "gridlayoutmanager.H"
#include "nonrecursive_visibility.H"
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
			 impl->button->impl->set_image_number(IN_THREAD, n);
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
				 callback(true, i->get_image_number(),
					  busy_impl{*i});
			 } CATCH_EXCEPTIONS;
		 });
}
///////////////////////////////////////////////////////////////////////////
//
// This is the container implementation button for the image_buttonObj's
// container superclass.
typedef container_visible_elementObj<nonrecursive_visibilityObj
				     <container_elementObj<child_elementObj>>
				     > image_button_container_superclass_t;

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
		       factoryObj &f)
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
	return b;
}

template<typename functor>
static inline image_button
create_image_button(const std::vector<std::string_view>
		    &images,
		    functor &&creator,
		    factoryObj &f)
{
	return do_create_image_button(images,
				      make_function<image_button_factory_t>
				      (std::forward<functor>(creator)),
				      f);
}

image_button factoryObj::create_checkbox()
{
	return create_checkbox({"checkbox1", "checkbox2", "checkbox3"});
}

// Call create_image_button, using create_checkbox_impl() to create the
// internal image button.

image_button factoryObj::create_checkbox(const std::vector<std
					 ::string_view> &images)
{
	return create_image_button(images, create_checkbox_impl, *this);
}

image_button factoryObj::create_radio(const radio_group &group)
{
	return create_radio(group, {"radio1", "radio2"});
}

// Call create_image_button, using create_radio_impl() to create the
// internal image button.

image_button factoryObj::create_radio(const radio_group &group,
				      const std::vector<std::string_view> &images)
{
	return create_image_button(images,
				   [&]
				   (const auto &container,
				    const auto &images)
				   {
					   return create_radio_impl(group,
								    container,
								    images);
				   }, *this);
}

LIBCXXW_NAMESPACE_END
