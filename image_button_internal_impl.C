/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image_button_internal_impl.H"
#include "focus/focusable_element.H"
#include "hotspot_element.H"
#include "icon.H"
#include "radio_group.H"
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
	  current_image(0)
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

void image_button_internalObj::implObj::activated(IN_THREAD_ONLY)
{
	set_image_number(IN_THREAD, get_image_number()+1);
}

void image_button_internalObj::implObj::set_image_number(IN_THREAD_ONLY,
							 size_t next_icon)
{
	do_set_image_number(IN_THREAD, next_icon);
}

void image_button_internalObj::implObj::do_set_image_number(IN_THREAD_ONLY,
							    size_t next_icon)
{
	next_icon %= icon_images(IN_THREAD).size();

	*current_image_t::lock(current_image)=next_icon;

	set_icon(IN_THREAD, icon_images(IN_THREAD).at(next_icon));
}

//! Return the current icon number.
size_t image_button_internalObj::implObj::get_image_number()
{
	return *current_image_t::lock(current_image);
}

/////////////////////////////////////////////////////////////////////////
//
// Subclass of the image button implementation button that implements
// checkbox semantics: each activation cycles to the next image.

class LIBCXX_HIDDEN checkbox_image_buttonObj :
	public image_button_internalObj::implObj {
 public:

	using image_button_internalObj::implObj::implObj;

	~checkbox_image_buttonObj()=default;
};

ref<image_button_internalObj::implObj>
create_checkbox_impl(const ref<containerObj::implObj> &container,
		     const std::vector<icon> &icon_images)
{
	return ref<checkbox_image_buttonObj>::create(container, icon_images);
}

/////////////////////////////////////////////////////////////////////////
//
// Subclass of the image button implementation button that implements
// radio button semantics. When activated, all other radio buttons in the
// group cycle to image #0, and this radio button cycles to the next non-0
// image.

class LIBCXX_HIDDEN radio_image_buttonObj :
	public image_button_internalObj::implObj {
 public:

	//! Constructor
	radio_image_buttonObj(const radio_group &group,
			      const ref<containerObj::implObj> &container,
			      const std::vector<icon> &icon_images)
		: image_button_internalObj::implObj(container, icon_images),
		group(group)
		{
		}

	//! Destructor
	~radio_image_buttonObj()=default;

	//! My radio group

	const radio_group group;

	//! Override set_image_number().

	//! activated() calls it, as well as the public object's set_value().
	//! This way, the same radio button logic gets used for both.

	void set_image_number(IN_THREAD_ONLY, size_t) override;
};

ref<image_button_internalObj::implObj>
create_radio_impl(const radio_group &group,
		  const ref<containerObj::implObj> &container,
		  const std::vector<icon> &icon_images)
{
	auto r=ref<radio_image_buttonObj>::create(group,
						  container, icon_images);

	r->group->impl->button_list->push_back(r);

	return r;
}

void radio_image_buttonObj::set_image_number(IN_THREAD_ONLY, size_t n)
{
	n %= icon_images(IN_THREAD).size();

	if (n == 0)
		if (n == 0) n=(n+1) % icon_images(IN_THREAD).size();

	ref<image_button_internalObj::implObj> me(this);

	// Turn off all other radio buttons...

	for (const auto &buttonptr : *group->impl->button_list)
	{
		auto buttonp=buttonptr.getptr();

		if (!buttonp)
			continue;

		ref<image_button_internalObj::implObj> button=buttonp;

		// ... except me.

		if (button == me)
			continue;

		if (button->get_image_number() == 0)
			continue;

		button->do_set_image_number(IN_THREAD, 0);
	}

	// And turn on myself.

	if (n != get_image_number())
		do_set_image_number(IN_THREAD, n);
}

LIBCXXW_NAMESPACE_END
