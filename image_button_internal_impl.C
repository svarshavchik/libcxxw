/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/busy.H"
#include "image_button_internal_impl.H"
#include "x/w/impl/focus/focusable_element.H"
#include "x/w/impl/always_visible_element.H"
#include "hotspot_element.H"
#include "icon_images_vector_element.H"
#include "x/w/impl/icon.H"
#include "radio_group.H"
#include "radio_buttonobj.H"
#include "busy.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

static auto get_first_icon_image(const std::vector<icon> &v)
{
	if (v.empty())
		throw EXCEPTION("Internal error: empty icon list");

	return v.at(0);
}


image_button_internal_impl_init_params
::image_button_internal_impl_init_params(const container_impl &container,
					 const std::vector<icon> &icon_images)
	: image_impl_init_params{container, get_first_icon_image(icon_images)},
	  icon_images{icon_images}
{
}

image_button_internal_impl_init_params
::image_button_internal_impl_init_params(const container_impl &container,
					 const std::vector<icon> &icon_images,
					 const metrics::axis &horiz_metrics,
					 const metrics::axis &vert_metrics)
	: image_impl_init_params{container, get_first_icon_image(icon_images),
				 horiz_metrics, vert_metrics},
	  icon_images{icon_images}
{
}

image_button_internal_impl_init_params
::~image_button_internal_impl_init_params()=default;

image_button_internalObj::implObj
::implObj(const image_button_internal_impl_init_params &init_params)
	: superclass_t{init_params.icon_images, init_params},
	  current_image{0}
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
			auto &cb=current_callback(IN_THREAD);

			if (cb)
				cb(IN_THREAD, n, trigger,
				   busy_impl{*this});
		} REPORT_EXCEPTIONS(this);
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
create_checkbox_impl(const container_impl &container,
		     const std::vector<icon> &icon_images)
{
	return ref<checkbox_image_buttonObj>::create
		(image_button_internal_impl_init_params{container,
							icon_images});
}

/////////////////////////////////////////////////////////////////////////
//
// Subclass of the image button implementation button that implements
// radio button semantics. When activated, all other radio buttons in the
// group cycle to image #0, and this radio button cycles to the next non-0
// image.

class LIBCXX_HIDDEN radio_image_buttonObj :
	public checkbox_image_buttonObj,
	public radio_buttonObj {
 public:

	//! Constructor
	radio_image_buttonObj(const radio_group &group,
			      const image_button_internal_impl_init_params
			      &init_params)
		: checkbox_image_buttonObj{init_params},
		group{group}
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

	//! Implement radio button turn off.

	void turn_off(ONLY IN_THREAD,
		      const container_impl &parent_container,
		      busy_impl &i_am_busy,
		      const callback_trigger_t &trigger) override;
};

void radio_image_buttonObj::turn_off(ONLY IN_THREAD,
				     const container_impl &parent_container,
				     busy_impl &i_am_busy,
				     const callback_trigger_t &trigger)
{
	if (get_image_number() == 0)
		return;

	do_set_image_number(IN_THREAD, 0);

	auto &cb=current_callback(IN_THREAD);

	if (cb)
		try {
			cb(IN_THREAD, 0, trigger, i_am_busy);
		} REPORT_EXCEPTIONS(this);
}

ref<image_button_internalObj::implObj>
create_radio_impl(const radio_group &group,
		  const container_impl &container,
		  const std::vector<icon> &icon_images)
{
	auto r=ref<radio_image_buttonObj>::create
		(group,
		 image_button_internal_impl_init_params{
			container, icon_images});

	r->group->button_list->push_back(r);

	return r;
}

void radio_image_buttonObj::set_image_number(ONLY IN_THREAD,
					     const callback_trigger_t &trigger,
					     size_t n)
{
	busy_impl i_am_busy{*this};

	group->turn_off(IN_THREAD, radio_button{this}, child_container,
			i_am_busy, trigger);

	n %= icon_images(IN_THREAD).size();

	if (n == 0)
		n=(n+1) % icon_images(IN_THREAD).size();

	// Turn on myself.

	auto p=get_image_number();
	if (n != p)
	{
		do_set_image_number(IN_THREAD, n);
		try {
			auto &cb=current_callback(IN_THREAD);

			if (cb)
				cb(IN_THREAD, n, trigger, i_am_busy);
		} REPORT_EXCEPTIONS(this);
	}
}

LIBCXXW_NAMESPACE_END
