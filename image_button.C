/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image_button.H"
#include "image_button_internal.H"
#include "image_button_internal_impl.H"
#include "focus/focusframecontainer.H"
#include "focus/focusframecontainer_element.H"
#include "icon.H"
#include "x/w/factory.H"
#include "container_element.H"
#include "gridlayoutmanager.H"
#include "nonrecursive_visibility.H"
#include "messages.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

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

///////////////////////////////////////////////////////////////////////////
//
// This is the container implementation button for the image_buttonObj's
// container superclass.

class LIBCXX_HIDDEN image_button_containerObj
	: public nonrecursive_visibilityObj<
	container_elementObj<child_elementObj>> {

	typedef nonrecursive_visibilityObj<
		container_elementObj<child_elementObj>> superclass_t;

 public:

	image_button_containerObj(const ref<containerObj::implObj>
				  &parent_container)
		: superclass_t(parent_container, metrics::horizvert_axi(),
			       "focusframe@libcxx")
	{
	}

	~image_button_containerObj()=default;
};

// The implementation object for the focusframecontainer.

typedef ref<focusframecontainer_elementObj<
		    container_elementObj<child_elementObj>>> ff_impl_t;

static image_button
create_image_button(const std::vector<std::experimental::string_view> &images,
		    auto img_impl_factory,
		    factoryObj &f)
{
	if (images.empty())
		throw EXCEPTION(_("Attempt to create an image button without any images."));

	std::vector<icon> icons;

	icons.reserve(images.size());

	for (const auto &name:images)
		icons.push_back(f.container_impl->get_window_handler()
				.create_icon_mm(name, render_repeat::none,
						0, 0));

	// Create an image_button_containerObj, a container with a grid
	// layout manager.

	auto image_button_outer_container_impl=
		ref<image_button_containerObj>::create(f.container_impl);

	auto image_button_outer_container_layout=
		ref<gridlayoutmanagerObj::implObj>
		::create(image_button_outer_container_impl);

	auto glm=image_button_outer_container_layout->create_gridlayoutmanager();

	// This grid layout manager will contain a single focusframecontainer.

	auto focus_frame_impl=ff_impl_t::create(image_button_outer_container_impl, metrics::horizvert_axi(),
				       "focusframe@libcxx");

	// Create an image_button_internal implementation object. Its
	// container is the focusframecontainer.

	auto ibii=img_impl_factory(focus_frame_impl, icons);

	// And create the image_button_internal "public" object.
	auto ibi=image_button_internal::create(ibii);

	// Now, create the focusframecontainer object, pointing it to the
	// image_button_internalObj as its focusable implementation object.

	auto focus_frame=focusframecontainer::create(focus_frame_impl, ibii);

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

image_button factoryObj::create_checkbox()
{
	return create_checkbox({"checkbox1", "checkbox2"});
}

image_button factoryObj::create_checkbox(const std::vector<std::experimental
					 ::string_view> &images)
{
	return create_image_button(images, create_checkbox_impl, *this);
}

LIBCXXW_NAMESPACE_END
