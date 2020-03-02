/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/pane_peephole_container_impl.H"
#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "peephole/peephole.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/scrollbar.H"
#include <x/exception.H>
#include <vector>

LIBCXXW_NAMESPACE_START

pane_peephole_containerObj
::pane_peephole_containerObj(const ref<implObj> &impl,
			     const layout_impl &container_layout_impl,
			     dim_t reference_size)
	: focusable_containerObj{impl, container_layout_impl},
	  reference_size_thread_only{reference_size},
	  impl{impl}
{
}

pane_peephole_containerObj::~pane_peephole_containerObj()=default;

focusable_impl pane_peephole_containerObj::get_impl() const
{
	// The only reason we end up here is to do internal sanity checks.
	// We don't care which focusable we return here.

	focusableptr fp;

	impl->invoke_layoutmanager
		([&]
		 (const ref<peephole_gridlayoutmanagerObj> &lm_impl)
		 {
			 fp=lm_impl->my_horizontal_scrollbar;
		 });
	return fp->get_impl();
}

void pane_peephole_containerObj
::do_get_impl(const function<internal_focusable_cb> &cb) const
{
	// Combine the scrollbars' into a single focusable list.

	std::vector<focusable_impl> all_focusable_impls;

	auto optional_focusable_element=focusable_element.get();

	if (optional_focusable_element)
	{
		optional_focusable_element->get_impl
			([&]
			 (const auto &focusable_info)
			 {
				 all_focusable_impls.insert
					 (all_focusable_impls.end(),
					  focusable_info.impls,
					  focusable_info.impls+
					  focusable_info
					  .internal_impl_count);
			 });
	}

	impl->invoke_layoutmanager
		([&]
		 (const ref<peephole_gridlayoutmanagerObj> &lm_impl)
		 {
			 lm_impl->my_vertical_scrollbar->get_impl
				 ([&]
				  (const auto &focusable_info)
				  {
					  all_focusable_impls.insert
						  (all_focusable_impls.end(),
						   focusable_info.impls,
						   focusable_info.impls+
						   focusable_info
						   .internal_impl_count);
				  });

			 lm_impl->my_horizontal_scrollbar->get_impl
				 ([&]
				  (const auto &focusable_info)
				  {
					  all_focusable_impls.insert
						  (all_focusable_impls.end(),
						   focusable_info.impls,
						   focusable_info.impls+
						   focusable_info
						   .internal_impl_count);

				  });
		 });

	cb(internal_focusable_group{all_focusable_impls.size(),
				&all_focusable_impls.at(0)});
}

peephole pane_peephole_containerObj::get_peephole()
{
	peepholeptr c;

	impl->invoke_layoutmanager
		([&]
		 (const ref<peephole_gridlayoutmanagerObj> &lm)
		 {
			 c=lm->my_peephole;
		 });

	return c;
}

LIBCXXW_NAMESPACE_END
