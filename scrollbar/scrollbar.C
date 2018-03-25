/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "scrollbar_impl.H"
#include "container_element.H"
#include "container_visible_element.H"
#include "nonrecursive_visibility.H"
#include "focus/focusframefactory.H"
#include "focus/focusframelayoutimpl.H"
#include "focus/standard_focusframecontainer_element.H"
#include "run_as.H"
#include "x/w/rgb.H"
#include "x/w/factory.H"

LIBCXXW_NAMESPACE_START

scrollbarObj::scrollbarObj(const ref<implObj> &impl,
			   const ref<containerObj::implObj> &container_impl,
			   const ref<layoutmanagerObj::implObj> &layout_impl)
	: containerObj(container_impl, layout_impl),
	  focusableObj::ownerObj(impl),
	impl(impl)
{
}

scrollbarObj::~scrollbarObj()=default;

void scrollbarObj::set(scroll_v_t value)
{
	impl->THREAD->run_as([impl=this->impl, value]
			     (ONLY IN_THREAD)
			     {
				     auto new_state=impl->state(IN_THREAD);

				     new_state.value=value;

				     impl->reconfigure(IN_THREAD, new_state);
			     });
}

scroll_v_t::value_type scrollbarObj::get_value() const
{
	auto v=impl->current_value.get();

	return (scroll_v_t::value_type)std::get<0>(v);
}

scroll_v_t::value_type scrollbarObj::get_dragged_value() const
{
	auto v=impl->current_value.get();

	return (scroll_v_t::value_type)std::get<0>(v);
}

void scrollbarObj::reconfigure(const scrollbar_config &new_state)
{
	impl->THREAD->run_as([impl=this->impl, new_state]
			     (ONLY IN_THREAD)
			     {
				     impl->reconfigure(IN_THREAD, new_state);
			     });
}

void scrollbarObj::on_update(const scrollbar_cb_t &callback)
{
	impl->THREAD->run_as([impl=this->impl, callback]
			     (ONLY IN_THREAD)
			     {
				     impl->update_callback(IN_THREAD, callback);
			     });
}

// Construct a vertical or a horizontal scrollbar.

static scrollbar create_scrollbar(const ref<containerObj::implObj> &parent_container,
				  const std::optional<color_arg> &background_color,
				  const scrollbar_config &conf,
				  const scrollbar_orientation &orientation,
				  const auto &icon_set_1,
				  const auto &icon_set_2,
				  const dim_arg &minimum_size,
				  const scrollbar_cb_t &callback)
{
	// Create a container for the focus frame around the scrollbar.
	//
	// This will be the implementation object of the actual child element
	// that we will return to the factory as, supposedly, the newly
	// created element. Hence the implementation object points to the
	// factory's container.

	auto ffcontainer_impl=
		create_nonrecursive_visibility_focusframe
		(parent_container,
		 "scrollbarfocusoff_border",
		 "inputfocuson_border", background_color);

	// And this will be its layout manager.

	auto fflayout=ref<focusframelayoutimplObj>
		::create(ffcontainer_impl);

	// The focus frame will manage the actual scrollbar element. Create
	// the implementation object. Since the focus-framed element will
	// be installed in the focusframe container, the implementation
	// object's container is ffcontainer_impl.

	auto scrollbar_impl=
		ref<scrollbarObj::implObj>
		::create(scrollbar_impl_init_params{ffcontainer_impl,
					callback ? callback
					: scrollbar_cb_t{[](const auto &){}},
					orientation,
					std::tuple_cat(icon_set_1,
						       icon_set_2),
					conf,
					minimum_size});

	// We are now ready to construct the elements.
	//
	// The constructed scrollbar elemen that gets returned.
	auto sb=scrollbar::create(scrollbar_impl,
				  ffcontainer_impl,
				  fflayout);

	// We need to tell the focus frame that we, supposedly, created
	// an element for it. Create a plain element that owns the
	// scrollbar implementation object, install it in the focus frame,
	// and make it visible.
	auto e=element::create(scrollbar_impl);

	focusframefactory::create(sb)->created(e);

	e->set_background_color("scrollbar_background_color");
	e->show();

	return sb;
}

// Construct one of the two icon sets for the scrollbar to render.

static auto
create_scrollbar_icon_set(drawableObj::implObj &drawable,
			  const char *scroll_low,
			  const char *scroll_high,
			  const char *start,
			  const char *handle,
			  const char *end,
			  const char *suffix)
{
	std::string scroll{"scroll-"};
	std::string scrollbar{"scrollbar-"};
	std::string knob{"scrollbar-knob-"};

	return std::tuple{
		drawable.create_icon({scroll + scroll_low + suffix}),
			drawable.create_icon({scroll + scroll_high + suffix}),
			drawable.create_icon({knob + start + suffix}),
			drawable.create_icon({scrollbar + handle + suffix}),
			drawable.create_icon({knob + end + suffix}),
			};
}

scrollbar
do_create_h_scrollbar(const ref<containerObj::implObj> &parent_container,
		      const std::optional<color_arg> &background_color,
		      const scrollbar_config &conf,
		      const dim_arg &minimum_size,
		      const scrollbar_cb_t &callback)
{
	auto &window_handler=parent_container->get_window_handler();

	return create_scrollbar(parent_container, background_color, conf,
				horizontal_scrollbar,
				create_scrollbar_icon_set
				(window_handler,
				 "left", "right",
				 "left", "horiz", "right", "1"),
				create_scrollbar_icon_set
				(window_handler,
				 "left", "right",
				 "left", "horiz", "right", "2"),
				minimum_size,
				callback);
}

scrollbar
do_create_v_scrollbar(const ref<containerObj::implObj> &parent_container,
		      const std::optional<color_arg> &background_color,
		      const scrollbar_config &conf,
		      const dim_arg &minimum_size,
		      const scrollbar_cb_t &callback)
{
	auto &window_handler=parent_container->get_window_handler();

	return create_scrollbar(parent_container, background_color, conf,
				vertical_scrollbar,
				create_scrollbar_icon_set
				(window_handler,
				 "up", "down",
				 "top", "vert", "bottom", "1"),
				create_scrollbar_icon_set
				(window_handler,
				 "up", "down",
				 "top", "vert", "bottom", "2"),
				minimum_size,
				callback);
}

scrollbar factoryObj
::create_horizontal_scrollbar(const scrollbar_config &config,
			      dim_arg minimum_size)
{
	return create_horizontal_scrollbar(config, nullptr,
					   minimum_size);
}

scrollbar factoryObj
::create_horizontal_scrollbar(const scrollbar_config &config,
			      const scrollbar_cb_t &callback,
			      dim_arg minimum_size)
{
	auto sb=do_create_h_scrollbar(get_container_impl(),
				      std::nullopt,
				      config,
				      minimum_size,
				      callback);

	created_internally(sb);
	return sb;
}


scrollbar factoryObj
::create_vertical_scrollbar(const scrollbar_config &config,
			    dim_arg minimum_size)
{
	return create_vertical_scrollbar(config, nullptr,
					 minimum_size);
}

scrollbar factoryObj
::create_vertical_scrollbar(const scrollbar_config &config,
			    const scrollbar_cb_t &callback,
			    dim_arg minimum_size)
{
	auto sb=do_create_v_scrollbar(get_container_impl(),
				      std::nullopt,
				      config,
				      minimum_size,
				      callback);

	created_internally(sb);
	return sb;
}

LIBCXXW_NAMESPACE_END
