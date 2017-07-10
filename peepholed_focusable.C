/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peepholed_focusable.H"
#include "nonrecursive_visibility.H"
#include "peephole/peephole.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "focus/focusable.H"
#include "focus/standard_focusframecontainer_element.H"
#include "scrollbar/scrollbar.H"
#include "container_element.H"
#include "container_visible_element.H"
#include "gridlayoutmanager.H"
#include "x/w/factory.H"
#include "x/w/gridfactory.H"

LIBCXXW_NAMESPACE_START

peepholed_focusableObj
::peepholed_focusableObj(const ref<implObj> &impl,
			 const ref<layoutmanagerObj::implObj> &layout_impl)
	: focusable_containerObj(layout_impl->container_impl, layout_impl),
	  impl(impl)
{
}

peepholed_focusableObj::~peepholed_focusableObj()=default;

ref<focusableImplObj> peepholed_focusableObj::get_impl() const
{
	return impl->get_impl();
}

// The input field has three focusable fields inside it.

size_t peepholed_focusableObj::internal_impl_count() const
{
	return impl->internal_impl_count();
}

ref<focusableImplObj> peepholed_focusableObj::get_impl(size_t n) const
{
	return impl->get_impl();
}


peepholed_focusableObj::implObj
::implObj(const focusable &peepholed_element,
	  const scrollbar &vertical_scrollbar,
	  const scrollbar &horizontal_scrollbar)
	: peepholed_element(peepholed_element),
	  vertical_scrollbar(vertical_scrollbar),
	  horizontal_scrollbar(horizontal_scrollbar)
{
}

peepholed_focusableObj::implObj::~implObj()=default;

ref<focusableImplObj> peepholed_focusableObj::implObj::get_impl() const
{
	return peepholed_element->get_impl();
}

size_t peepholed_focusableObj::implObj::internal_impl_count() const
{
	return 3;
}

focusable_impl peepholed_focusableObj::implObj::get_impl(size_t n) const
{
	// Note that the resulting order corresponds to the
	// order that's initially set by set_peephole_scrollbar_focus_order().

	if (n == 1)
		return vertical_scrollbar->get_impl();

	if (n == 2)
		return horizontal_scrollbar->get_impl();

	return get_impl();
}

std::tuple<ref<peepholed_focusableObj::implObj>, gridlayoutmanager>
create_peepholed_focusable_with_frame_impl
(const char *border,
 const char *inputfocusoff_border,
 const char *inputfocuson_border,
 double focusable_padding,
 const background_color &focusable_background_color,
 const ref<containerObj::implObj> &parent_container,
 peephole_style style,
 const function<make_peepholed_func_t> &make_peepholed,
 scrollbar_visibility horizontal_visibility,
 scrollbar_visibility vertical_visibility)
{
	auto grid_impl=
		ref<gridlayoutmanagerObj::implObj>
		::create(parent_container);

	auto grid=grid_impl->create_gridlayoutmanager();

	auto factory=grid->append_row();

	factory->padding(0).border(border);

	// Create the focusframe implementation object, first.

	auto focusframecontainer_impl=
		create_standard_focusframe_container_element
		(factory->container_impl,
		 focusable_background_color);

	// Now that the focusframe implementation object exists we can
	// create the peepholed focusable element.

	auto ret=make_peepholed(focusframecontainer_impl);

	// TODO: structured bindings
	auto &impl=std::get<0>(ret);
	auto &editor=std::get<1>(ret);
	auto &focusable_element=std::get<2>(ret);
	auto &focusable_element_impl=std::get<3>(ret);

	// We can now create our layout manager, and give it the created
	// editor.

	auto scrollbars=create_peephole_scrollbars(parent_container);

	auto layout_impl=ref<peepholeObj::layoutmanager_implObj>
		::create(impl,
			 style,
			 editor,
			 scrollbars,
			 horizontal_visibility,
			 vertical_visibility);

	layout_impl->initialize_scrollbars();

	// In order to properly initialize the focusable element, the layout
	// manager needs_recalculation(). Arrange to invoke it indirectly
	// by constructing the layout manager public object, which will
	// take care of calling needs_recalculation().

	auto public_layout=layout_impl->create_public_object();

	// Ok, we can now create the container.
	auto peephole_container=peephole::create(impl, layout_impl);

	// We can now create the focusframecontainer public object, now that
	// the implementation object, and the focusable object exist.

	auto ff=focusframecontainer
		::create(focusframecontainer_impl,
			 focusable_element_impl,
			 inputfocusoff_border,
			 inputfocuson_border);

	// Make sure that the the focusframe and the scrollbars use the
	// correct tabbing order.
	set_peephole_scrollbar_focus_order(ff,
					   scrollbars.horizontal_scrollbar,
					   scrollbars.vertical_scrollbar);

	// We still need to:
	//
	// Explicitly show() the peephole_container and the focusframe, since
	// the whole thing has nonrecursive_visibility.
	//
	// Officially place the peephole_container in focusframe's layout
	// manager, it was created_internally().
	//
	// Officially place the focusframe inside the grid
	// layout manager, it was created_internally().

	gridlayoutmanager l=ff->get_layoutmanager();

	peephole_container->show();
	l->append_row()->padding(focusable_padding)
		.created_internally(peephole_container);
	ff->show();

	factory->padding(0).created_internally(ff);

	// Before letting install_peephole_scrollbars() finish the job,
	// let's prime the cells with the right border.

	factory->border(border);

	auto factory2=grid->append_row();
	factory2->border(border);

	install_peephole_scrollbars(l,
				    scrollbars.vertical_scrollbar,
				    vertical_visibility,
				    factory,
				    scrollbars.horizontal_scrollbar,
				    horizontal_visibility,
				    factory2);

	return {ref<peepholed_focusableObj::implObj>
			::create(focusable_element,
				 scrollbars.vertical_scrollbar,
				 scrollbars.horizontal_scrollbar),
			grid};
}

LIBCXXW_NAMESPACE_END
