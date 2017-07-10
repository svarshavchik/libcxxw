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
#include "x/w/rgb.H"

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

// Construct a vertical or a horizontal scrollbar.

static scrollbar create_scrollbar(const ref<containerObj::implObj> &parent_container,
				  const scrollbar_config &conf,
				  const scrollbar_orientation &orientation,
				  const scrollbar_icon_set &icon_set_1,
				  const scrollbar_icon_set &icon_set_2,
				  const function<scrollbar_impl_constructor>
				  &create_impl)
{
	// Create a container for the focus frame around the scrollbar.
	//
	// This will be the implementation object of the actual child element
	// that we will return to the factory as, supposedly, the newly
	// created element. Hence the implementation object points to the
	// factory's container.

	auto ffcontainer_impl=
		create_standard_focusframe_container_element(parent_container);

	// And this will be its layout manager.

	auto fflayout=ref<focusframelayoutimplObj>
		::create(ffcontainer_impl,
			 "scrollbarfocusoff_border",
			 "inputfocuson_border");

	// The focus frame will manage the actual scrollbar element. Create
	// the implementation object. Since the focus-framed element will
	// be installed in the focusframe container, the implementation
	// object's container is ffcontainer_impl.

	auto scrollbar_impl=
		create_impl(scrollbar_impl_init_params{ffcontainer_impl,
					orientation,
					icon_set_1,
					icon_set_2,
					conf});

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

static scrollbar_icon_set
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

	return {
		drawable.create_icon_mm(scroll + scroll_low + suffix,
					render_repeat::none, 0, 0),
		drawable.create_icon_mm(scroll + scroll_high + suffix,
					render_repeat::none, 0, 0),
		drawable.create_icon_mm(knob + start + suffix,
					render_repeat::none, 0, 0),
		drawable.create_icon_mm(scrollbar + handle + suffix,
					render_repeat::none, 0, 0),
		drawable.create_icon_mm(knob + end + suffix,
					render_repeat::none, 0, 0),
	};
}

scrollbar do_create_h_scrollbar(const ref<containerObj::implObj> &parent_container,
				const scrollbar_config &conf,
				const function<scrollbar_impl_constructor>
				&create_impl)
{
	auto &window_handler=parent_container->get_window_handler();

	return create_scrollbar(parent_container, conf,
				horizontal_scrollbar,
				create_scrollbar_icon_set
				(window_handler,
				 "left", "right",
				 "left", "horiz", "right", "1"),
				create_scrollbar_icon_set
				(window_handler,
				 "left", "right",
				 "left", "horiz", "right", "2"),
				create_impl);
}

scrollbar do_create_v_scrollbar(const ref<containerObj::implObj> &parent_container,
				const scrollbar_config &conf,
				const function<scrollbar_impl_constructor>
				&create_impl)
{
	auto &window_handler=parent_container->get_window_handler();

	return create_scrollbar(parent_container, conf,
				vertical_scrollbar,
				create_scrollbar_icon_set
				(window_handler,
				 "up", "down",
				 "top", "vert", "bottom", "1"),
				create_scrollbar_icon_set
				(window_handler,
				 "up", "down",
				 "top", "vert", "bottom", "2"),
				create_impl);
}

LIBCXXW_NAMESPACE_END
