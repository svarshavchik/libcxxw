/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image_button_internal_impl.H"
#include "focus/focusable_element.H"
#include "hotspot_element.H"
#include "icon.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

static auto get_first_icon_image(const auto &v)
{
	if (v.empty())
		throw EXCEPTION("Internal error: empty icon list");

	return v.at(0);
}

image_button_internalObj::implObj
::implObj(const ref<containerObj::implObj> &container,
	  const std::vector<icon> &icon_images)
	: superclass_t(true, container, get_first_icon_image(icon_images)),
	  icon_images_thread_only(icon_images),
	  current_image_thread_only(0)
{
}

image_button_internalObj::implObj::~implObj()=default;

void image_button_internalObj::implObj::initialize(IN_THREAD_ONLY)
{
	for (auto &i:icon_images(IN_THREAD))
	{
		i=i->initialize(IN_THREAD);
	}
	superclass_t::initialize(IN_THREAD);
}

void image_button_internalObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	for (auto &i:icon_images(IN_THREAD))
	{
		i=i->theme_updated(IN_THREAD);
	}
	superclass_t::theme_updated(IN_THREAD);
}

// Subclass of the image button implementation button that implements
// checkbox semantics: each activation cycles the images.

class LIBCXX_HIDDEN checkbox_image_buttonObj :
	public image_button_internalObj::implObj {
 public:

	using image_button_internalObj::implObj::implObj;

	~checkbox_image_buttonObj()=default;

	void activated(IN_THREAD_ONLY) override
	{
		current_image(IN_THREAD)=(current_image(IN_THREAD)+1)
			% icon_images(IN_THREAD).size();

		set_icon(IN_THREAD, icon_images(IN_THREAD)
			 .at(current_image(IN_THREAD)));
		image_button_internalObj::implObj::activated(IN_THREAD);
	}
};

ref<image_button_internalObj::implObj>
create_checkbox_impl(const ref<containerObj::implObj> &container,
		     const std::vector<icon> &icon_images)
{
	return ref<checkbox_image_buttonObj>::create(container, icon_images);
}

LIBCXXW_NAMESPACE_END
