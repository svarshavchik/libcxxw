/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peepholed_focusable.H"
#include "x/w/focusable_container_owner.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "peephole/peephole.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/focus/standard_focusframecontainer_element_impl.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/impl/richtext/richtext.H"
#include "gridlayoutmanager.H"
#include "x/w/factory.H"
#include "x/w/gridfactory.H"
#include "x/w/scrollbar.H"
#include <x/refptr_traits.H>

LIBCXXW_NAMESPACE_START

peepholed_focusableObj
::peepholed_focusableObj(const ref<implObj> &impl,
			 const layout_impl &container_layout_impl)
	: focusable_containerObj{container_layout_impl->layout_container_impl,
		container_layout_impl},
	  impl{impl}
{
}

peepholed_focusableObj::~peepholed_focusableObj()=default;

focusable_impl peepholed_focusableObj::get_impl() const
{
	return impl->get_impl();
}

// The input field has three focusable fields inside it.

void peepholed_focusableObj
::do_get_impl(const function<internal_focusable_cb> &cb) const
{
	impl->do_get_impl(cb);
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

focusable_impl peepholed_focusableObj::implObj::get_impl() const
{
	return peepholed_element->get_impl();
}

void peepholed_focusableObj::implObj
::do_get_impl(const function<internal_focusable_cb> &cb) const
{
	// Note that the resulting order corresponds to the
	// order that's initially set by set_peephole_scrollbar_focus_order().

	process_focusable_impls(cb, get_impl(),
				vertical_scrollbar->get_impl(),
				horizontal_scrollbar->get_impl());
}

peepholed_focusable_with_frame_ret_t
create_peepholed_focusable_with_frame_impl
(const create_peepholed_focusable_args_t &args,
 const function<make_peepholed_func_t> &make_peepholed)
{
	auto pfc_impl=ref<bordercontainer_elementObj<container_elementObj
						     <child_elementObj>>>
		::create(args.border,
			 args.border,
			 args.border,
			 args.border,
			 richtextptr{},
			 0, 0, 0,
			 args.parent_container);

	// Create the focusframe implementation object, first.

	auto focusframecontainer_impl=
		create_always_visible_focusframe_impl
		(pfc_impl,
		 args.inputfocusoff_border,
		 args.inputfocuson_border,
		 args.focusable_padding,
		 args.focusable_padding,
		 args.focusable_background_color);

	// Now that the focusframe implementation object
	// exists we can create the peepholed focusable
	// element.

	auto [impl, peepholed_element, focusable_element,
	      focusable_element_impl]=
		make_peepholed(focusframecontainer_impl,
			       args.parent_container);

	const auto &[layout_impl, grid_impl, grid]=
		create_peephole_with_scrollbars
		([&]
		 (const ref<peepholeObj::layoutmanager_implObj> &layout_impl)
		 -> peephole_element_factory_ret_t
		 {
			 // In order to properly initialize the focusable
			 // element, the layout manager needs_recalculation().
			 // Arrange to invoke it indirectly by constructing
			 // the layout manager public object, which will
			 // take care of calling needs_recalculation().

			 auto public_layout=layout_impl->create_public_object();

			 // Ok, we can now create the container.
			 auto peephole_container=peephole::create(impl,
								  layout_impl);

			 // We can now create the focusframecontainer public
			 // object, now that the implementation object, and
			 // the focusable object exist.

			 auto ff=focusable_container_owner::create
				 (focusframecontainer_impl,
				  ref<focusframelayoutimplObj>
				  ::create(focusframecontainer_impl,
					   focusframecontainer_impl,
					   peephole_container),
				  focusable_element_impl);

			 // We still need to:
			 //
			 // Explicitly show() the peephole_container and
			 // the focusframe, since the whole thing has
			 // nonrecursive_visibility.
			 //
			 // Officially place the focusframe inside the grid
			 // layout manager, it was created_internally().

			 peephole_container->show();
			 ff->show();

			 auto blm_impl=ref<borderlayoutmanagerObj::implObj>
				 ::create(pfc_impl, pfc_impl, ff, halign::fill,
					  valign::fill);

			 auto pfc=container::create(pfc_impl, blm_impl);

			 pfc->show();

			 return {
				 peephole_container,
				 pfc,
				 std::nullopt,
				 ff,
			 };
		 },
		 {
		  args.parent_container,
		  std::nullopt,
		  impl,
		  args.style,
		  args.horizontal_visibility,
		  args.vertical_visibility,
		  peepholed_element,
		 });

	return {ref<peepholed_focusableObj::implObj>
			::create(focusable_element,
				 grid_impl->my_vertical_scrollbar,
				 grid_impl->my_horizontal_scrollbar),
			grid};
}

LIBCXXW_NAMESPACE_END
