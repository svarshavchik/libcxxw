/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/busy.H"
#include "image_button_internal_impl.H"
#include "focus/focusable_element.H"
#include "hotspot_element.H"
#include "icon_images_vector_element.H"
#include "icon.H"
#include "radio_group.H"
#include "busy.H"
#include "catch_exceptions.H"

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
	: superclass_t(icon_images, container,
		       get_first_icon_image(icon_images)),
	  current_image(0),
	  current_callback_thread_only([](size_t, const auto &,
					  const auto &) {})
{
}

image_button_internalObj::implObj
::implObj(const ref<containerObj::implObj> &container,
	  const std::vector<icon> &icon_images,
	  const metrics::axis &horiz_metrics,
	  const metrics::axis &vert_metrics)
	: superclass_t(icon_images, container,
		       get_first_icon_image(icon_images),
		       horiz_metrics, vert_metrics),
	  current_image(0),
	  current_callback_thread_only([](size_t, const auto &,
					  const auto &) {})
{
}

image_button_internalObj::implObj::~implObj()=default;

void image_button_internalObj::implObj::set_image_number(ONLY IN_THREAD,
							 const callback_trigger_t &trigger,
							 size_t next_icon)
{
	auto p=get_image_number();

	do_set_image_number(IN_THREAD, next_icon);

	auto n=get_image_number();

	if (p != n)
		try {
			current_callback(IN_THREAD)(n, trigger,
						    busy_impl{*this});
		} CATCH_EXCEPTIONS;
}

void image_button_internalObj::implObj::do_set_image_number(ONLY IN_THREAD,
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
// A regular image_button_internalObj::implObj instance correctly handles
// checkbox semantics.

class LIBCXX_HIDDEN checkbox_image_buttonObj :
	public image_button_internalObj::implObj {

 public:
	using image_button_internalObj::implObj::implObj;

	~checkbox_image_buttonObj()=default;

		//! Overridden from hotspotObj::implObj

	//! We do not use hotspot callbacks. Invoke set_image_number();

	void activated(ONLY IN_THREAD, const callback_trigger_t &trigger)
		override
	{
		set_image_number(IN_THREAD, trigger, get_image_number() ? 0:1);
	}
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
	public checkbox_image_buttonObj {
 public:

	//! Constructor
	radio_image_buttonObj(const radio_group &group,
			      const ref<containerObj::implObj> &container,
			      const std::vector<icon> &icon_images)
		: checkbox_image_buttonObj(container, icon_images),
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

	void set_image_number(ONLY IN_THREAD,
			      const callback_trigger_t &trigger,
			      size_t) override;
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

void radio_image_buttonObj::set_image_number(ONLY IN_THREAD,
					     const callback_trigger_t &trigger,
					     size_t n)
{
	n %= icon_images(IN_THREAD).size();

	if (n == 0)
		n=(n+1) % icon_images(IN_THREAD).size();

	// Invoke all callbacks after updating all image buttons' state.

	std::vector<image_button_callback_t> callbacks;

	callbacks.reserve(group->impl->button_list->size());

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

		if (button->get_image_number())
			callbacks.push_back(button->current_callback(IN_THREAD));

		button->do_set_image_number(IN_THREAD, 0);
	}

	// And turn on myself.

	auto p=get_image_number();
	if (n != p)
		do_set_image_number(IN_THREAD, n);

	busy_impl i_am_busy{*this};

	for (const auto &cb:callbacks)
		try {
			cb(0, trigger, i_am_busy);
		} CATCH_EXCEPTIONS;

	if (n != p)
		try {
			current_callback(IN_THREAD)(n, trigger,
						    i_am_busy);
		} CATCH_EXCEPTIONS;
}

LIBCXXW_NAMESPACE_END
