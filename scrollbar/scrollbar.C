/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "scrollbar_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/always_visible_element.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "x/w/impl/focus/standard_focusframecontainer_element.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/focus/standard_focusframecontainer_element_impl.H"
#include "run_as.H"
#include "x/w/rgb.H"
#include "x/w/factory.H"
#include "x/w/scrollbar_appearance.H"
#include "x/w/scrollbar_images_appearance.H"
#include "x/w/focus_border_appearance.H"

LIBCXXW_NAMESPACE_START

scrollbarObj::scrollbarObj(const ref<implObj> &impl,
			   const container_impl &container_impl,
			   const layout_impl &container_layout_impl)
	: containerObj{container_impl, container_layout_impl},
	  focusableObj::ownerObj{impl},
	impl{impl}
{
}

scrollbarObj::~scrollbarObj()=default;

void scrollbarObj::set(scroll_v_t value)
{
	impl->THREAD->run_as([me=ref{this}, value]
			     (ONLY IN_THREAD)
			     {
				     me->set(IN_THREAD, value);
			     });
}

void scrollbarObj::set(ONLY IN_THREAD,
		       scroll_v_t value)
{
	auto new_state=impl->state(IN_THREAD);

	new_state.value=value;

	reconfigure(IN_THREAD, new_state);
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
	impl->THREAD->run_as([me=ref{this}, new_state]
			     (ONLY IN_THREAD)
			     {
				     me->reconfigure(IN_THREAD, new_state);
			     });
}

void scrollbarObj::reconfigure(ONLY IN_THREAD,
			       const scrollbar_config &new_state)
{
	impl->reconfigure(IN_THREAD, new_state);
}

void scrollbarObj::on_update(const scrollbar_cb_t &callback)
{
	impl->THREAD->run_as([impl=this->impl, callback]
			     (ONLY IN_THREAD)
			     {
				     impl->update_callback(IN_THREAD, callback);
			     });
}

typedef std::tuple<icon, icon, icon, icon, icon> icon_set_t;

// Construct a vertical or a horizontal scrollbar.

static scrollbar create_scrollbar(const container_impl &parent_container,
				  const std::optional<color_arg> &background_color,
				  const scrollbar_config &conf,
				  const scrollbar_orientation &orientation,
				  const icon_set_t &icon_set_1,
				  const icon_set_t &icon_set_2,
				  const const_scrollbar_appearance &appearance,
				  const scrollbar_cb_t &callback)
{
	// Create a container for the focus frame around the scrollbar.
	//
	// This will be the implementation object of the actual child element
	// that we will return to the factory as, supposedly, the newly
	// created element. Hence the implementation object points to the
	// factory's container.

	auto ffcontainer_impl=
		create_nonrecursive_visibility_focusframe_impl
		(parent_container,
		 appearance->focus_border,
		 0, 0, background_color);

	ref<focusframecontainer_implObj> ff_impl{ffcontainer_impl};

	// The focus frame will manage the actual scrollbar element. Create
	// the implementation object. Since the focus-framed element will
	// be installed in the focusframe container, the implementation
	// object's container is ffcontainer_impl.

	auto scrollbar_impl=
		ref<always_visible_elementObj<scrollbarObj::implObj>>
		::create(scrollbar_impl_init_params{ffcontainer_impl,
						    callback,
						    orientation,
						    std::tuple_cat(icon_set_1,
								   icon_set_2),
						    conf,
						    appearance});

	// We need to tell the focu sframe that we, supposedly, created
	// an element for it. Create a plain element that owns the
	// scrollbar implementation object, install it in the focus frame,
	// and make it visible.
	auto e=element::create(scrollbar_impl);

	// And this will be its layout manager.

	auto fflayout=ref<focusframelayoutimplObj>::create(ffcontainer_impl,
							   ffcontainer_impl,
							   e);

	// We are now ready to construct the elements.
	//
	// The constructed scrollbar elemen that gets returned.
	auto sb=scrollbar::create(scrollbar_impl,
				  ffcontainer_impl,
				  fflayout);

	return sb;
}

// Construct one of the two icon sets for the scrollbar to render.

static auto
create_scrollbar_icon_set(drawableObj::implObj &drawable,
			  const const_scrollbar_images_appearance &images)
{
	return std::tuple{
		drawable.create_icon({images->scroll_low}),
			drawable.create_icon({images->scroll_high}),
			drawable.create_icon({images->knob_start}),
			drawable.create_icon({images->knob_handle}),
			drawable.create_icon({images->knob_end}),
			};
}

scrollbar
do_create_h_scrollbar(const container_impl &parent_container,
		      const std::optional<color_arg> &background_color,
		      const scrollbar_config &conf,
		      const const_scrollbar_appearance &appearance,
		      const scrollbar_cb_t &callback)
{
	auto &window_handler=parent_container->get_window_handler();

	return create_scrollbar(parent_container, background_color, conf,
				horizontal_scrollbar,
				create_scrollbar_icon_set
				(window_handler,
				 appearance->horizontal1),
				create_scrollbar_icon_set
				(window_handler,
				 appearance->horizontal2),
				appearance,
				callback);
}

scrollbar
do_create_v_scrollbar(const container_impl &parent_container,
		      const std::optional<color_arg> &background_color,
		      const scrollbar_config &conf,
		      const const_scrollbar_appearance &appearance,
		      const scrollbar_cb_t &callback)
{
	auto &window_handler=parent_container->get_window_handler();

	return create_scrollbar(parent_container, background_color, conf,
				vertical_scrollbar,
				create_scrollbar_icon_set
				(window_handler,
				 appearance->vertical1),
				create_scrollbar_icon_set
				(window_handler,
				 appearance->vertical2),
				appearance,
				callback);
}


scrollbar factoryObj
::do_create_horizontal_scrollbar(const scrollbar_config &config,
				 const scrollbar_args_t &args)
{
	std::optional<scrollbar_cb_t> default_callback;

	auto callback=optional_arg_or<scrollbar_cb_t>(args,
						      default_callback,
						      [](THREAD_CALLBACK,
							 const auto &)
						      {
						      });

	std::optional<const_scrollbar_appearance> default_appearance;

	auto appearance=optional_arg_or<appearance_wrapper<
		const_scrollbar_appearance>>
		(args, default_appearance,
		 scrollbar_appearance::base::theme());

	auto sb=do_create_h_scrollbar(get_container_impl(),
				      std::nullopt,
				      config,
				      appearance,
				      callback);

	created_internally(sb);
	return sb;
}

scrollbar factoryObj
::do_create_vertical_scrollbar(const scrollbar_config &config,
			       const scrollbar_args_t &args)
{
	std::optional<scrollbar_cb_t> default_callback;

	auto callback=optional_arg_or<scrollbar_cb_t>(args,
						      default_callback,
						      [](THREAD_CALLBACK,
							 const auto &)
						      {
						      });

	std::optional<const_scrollbar_appearance> default_appearance;

	auto appearance=optional_arg_or<appearance_wrapper<
		const_scrollbar_appearance>>
		(args, default_appearance,
		 scrollbar_appearance::base::theme());

	auto sb=do_create_v_scrollbar(get_container_impl(),
				      std::nullopt,
				      config,
				      appearance,
				      callback);

	created_internally(sb);
	return sb;
}

LIBCXXW_NAMESPACE_END
