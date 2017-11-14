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
#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "focus/focusable.H"
#include "focus/standard_focusframecontainer_element.H"
#include "container_element.H"
#include "container_visible_element.H"
#include "gridlayoutmanager.H"
#include "x/w/factory.H"
#include "x/w/gridfactory.H"
#include "x/w/scrollbar.H"
#include <x/refptr_traits.H>

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
(const create_peepholed_focusable_args_t &args,
 const function<make_peepholed_func_t> &make_peepholed)
{
	auto grid_impl=
		ref<peephole_gridlayoutmanagerObj>
		::create(args.parent_container);

	auto grid=grid_impl->create_gridlayoutmanager();

	auto factory=grid->append_row();

	factory->padding(0);

	ptr<peepholeObj::implObj> impl;
	peepholedptr peepholed_element;
	focusableptr focusable_element;
	focusable_implptr focusable_element_impl;
	factoryptr ff_factory;

	refptr_traits<standard_focusframecontainer_element_t>
		::ptr_t focusframecontainer_impl;

	factory->create_container
		([&]
		 (const auto &new_container)
		 {
			 gridlayoutmanager new_container_glm=
				 new_container->get_layoutmanager();

			 auto new_container_factory=
				 new_container_glm->append_row();

			 new_container_factory->padding(0);
			 new_container_factory->border(args.border);

			 // The focus frame carries the same treatment.
			 // it gets 100% of its space, and is filled.
			 new_container_glm->requested_col_width(0, 100);
			 new_container_glm->requested_row_height(0, 100);
			 new_container_factory->halign(halign::fill);
			 new_container_factory->valign(valign::fill);

			 ff_factory=new_container_factory;

			 // Create the focusframe implementation object, first.

			 auto focusframe_impl_ret=
				 create_standard_focusframe_container_element
				 (new_container->impl,
				  args.focusable_background_color);

			 // Now that the focusframe implementation object
			 // exists we can create the peepholed focusable
			 // element.

			 std::tie(impl, peepholed_element, focusable_element,
				  focusable_element_impl)=
				 make_peepholed(focusframe_impl_ret);

			 focusframecontainer_impl=focusframe_impl_ret;
		 },
		 new_gridlayoutmanager())->show();

	// We can now create our layout manager, and give it the created
	// peepholed_element.

	auto scrollbars=create_peephole_scrollbars(args.parent_container);

	auto layout_impl=ref<peepholeObj::layoutmanager_implObj>
		::create(impl,
			 args.style,
			 peepholed_element,
			 scrollbars,
			 args.horizontal_visibility,
			 args.vertical_visibility);

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
			 args.inputfocusoff_border,
			 args.inputfocuson_border);

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
	l->append_row()->padding(args.focusable_padding)
		.created_internally(peephole_container);
	ff->show();

	ff_factory->created_internally(ff);

	auto factory2=grid->append_row();

	install_peephole_scrollbars(l,
				    scrollbars.vertical_scrollbar,
				    args.vertical_visibility,
				    factory,
				    scrollbars.horizontal_scrollbar,
				    args.horizontal_visibility,
				    factory2);

	return {ref<peepholed_focusableObj::implObj>
			::create(focusable_element,
				 scrollbars.vertical_scrollbar,
				 scrollbars.horizontal_scrollbar),
			grid};
}

LIBCXXW_NAMESPACE_END
