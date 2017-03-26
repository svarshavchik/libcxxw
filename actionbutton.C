/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "actionbutton.H"
#include "gridlayoutmanager.H"
#include "background_color.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "container_element.H"
#include "hotspot_bgcolor_element.H"
#include "focus/focusframecontainer_element.H"

LIBCXXW_NAMESPACE_START

// The implementation of the action button is a mixin that combines
//
// A hotspot with background colors.
//
// The hotspot is inside a focusframecontainer
//
// Which is a container, derived from child_elementObj, since we must be
// somebody's child element.

typedef hotspot_bgcolor_elementObj<focusframecontainer_elementObj<
					   container_elementObj
					   <child_elementObj>>
				   > ff_impl_t;

struct LIBCXX_HIDDEN actionbuttonObj::internal_construction_info {

	ref<gridlayoutmanagerObj::implObj> glmi;

	ref<ff_impl_t> ff_impl;
};

// We get:
//
// A created implementation object.
//
// Action button border: normal or special.
//
// Factory for creating the contents of the action button.

static actionbuttonObj::internal_construction_info
create_actionbutton_focusframe(const ref<actionbuttonObj::implObj> &impl,
			       const char *border,
			       const function<void (const factory &)> &f)
{
	// Create the grid layout manager that the action button uses
	// for its real contents: the focusframecontainer.

	auto glmi=ref<gridlayoutmanagerObj::implObj>::create(impl);

	auto glm=glmi->create_gridlayoutmanager();

	// Now, create the focusframecontainer.
	//
	// Obtain the factory for its contents, via set_focusable(), and
	// pass it to the factory that's supposed to populate the
	// focusframecontainer. Ergo, the factory user that thinks will
	// be populating the contents of the action button ends up populating
	// the contents of the focusframecontainer.

	auto ffi=ref<ff_impl_t>
		::create(impl->create_background_color("action_normal_color",
						       rgb(rgb::maximum * .7,
							   rgb::maximum * .7,
							   rgb::maximum * .7)),
			 impl->create_background_color("action_selected_color",
						       rgb(rgb::maximum * .8,
							   rgb::maximum * .8,
							   rgb::maximum * .8)),
			 impl->create_background_color("action_active_color",
						       rgb(rgb::maximum,
							   rgb::maximum,
							   rgb::maximum)),
			 true,
			 glmi->container_impl,
			 metrics::horizvert_axi(),
			 "focusframe@libcxx");

	auto ff=focusframecontainer::create(ffi);

	// Call the application-provided creator to populate the contents
	// of the action button.

	f(ff->set_focusable());

	// Now, it's time to go back to the new action button's grid
	// layout manager, and insert the fully-cooked focusframecontainer.

	auto factory=glm->append_row();

	factory->padding(0);	// No padding for the focus frame.
	factory->border(border); // And set its border, too.

	factory->created_internally(ff);

	return {glmi, ffi};
}

actionbuttonObj::actionbuttonObj(const ref<implObj> &impl,
				 const internal_construction_info &ici)
	: containerObj(impl, ici.glmi),
	  hotspot_bgcolorObj(ici.ff_impl),
	  impl(impl)
{
}

actionbuttonObj::~actionbuttonObj()=default;

ref<focusableImplObj> actionbuttonObj::get_impl() const
{
	const_gridlayoutmanager m=get_layoutmanager();

	focusframecontainer ffc=m->get(0, 0);

	return ffc->impl;
}

actionbutton factoryObj::do_create_normal_actionbutton(const function<
						       void (const factory &)>
						       &f)
{
	return do_create_actionbutton("normal_action_border", f);
}

actionbutton factoryObj::do_create_special_actionbutton(const function<
							void (const factory &)>
							&f)
{
	return do_create_actionbutton("special_action_border", f);
}

actionbutton factoryObj::do_create_actionbutton(const char *theme_border,
						const function<
						void (const factory &)>
						&f)
{
	auto impl=ref<actionbuttonObj::implObj>::create(container_impl);

	auto ab=actionbutton::create(impl,
				     create_actionbutton_focusframe
				     (impl, theme_border, f));

	created(ab);
	return ab;
}

actionbutton
factoryObj::create_normal_actionbutton_with_label(const text_param &text,
						  halign alignment)
{
	return create_actionbutton_with_label("normal_action_border",
					      text, alignment);
}

actionbutton
factoryObj::create_special_actionbutton_with_label(const text_param &text,
						   halign alignment)
{
	return create_actionbutton_with_label("special_action_border",
					      text, alignment);
}

actionbutton
factoryObj::create_actionbutton_with_label(const char *theme_border,
					   const text_param &text,
					   halign alignment)
{
	return do_create_actionbutton
		(theme_border,
		 make_function<void (const factory &)>
		 ([&]
		  (const auto &f)
		  {
			  f->create_label(text);
		  }));
}

LIBCXXW_NAMESPACE_END
