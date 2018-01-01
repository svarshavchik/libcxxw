/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/pane_peephole_container_impl.H"
#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "focus/focusable.H"
#include "x/w/scrollbar.H"
#include <x/exception.H>
#include <vector>

LIBCXXW_NAMESPACE_START

pane_peephole_containerObj
::pane_peephole_containerObj(const ref<implObj> &impl,
			     const ref<layoutmanagerObj::implObj> &layout_impl)
	: focusable_containerObj{impl, layout_impl},
	  impl{impl}
{
}

pane_peephole_containerObj::~pane_peephole_containerObj()=default;

ref<focusableImplObj> pane_peephole_containerObj::get_impl() const
{
	// The only reason we end up here is to do internal sanity checks.
	// We don't care which focusable we return here.

	focusableptr fp;

	impl->invoke_layoutmanager
		([&]
		 (const ref<peephole_gridlayoutmanagerObj> &lm_impl)
		 {
			 fp=lm_impl->get_horizontal_scrollbar();
		 });
	return fp->get_impl();
}

void pane_peephole_containerObj
::do_get_impl(const function<internal_focusable_cb> &cb) const
{
	// Combine the scrollbars' into a single focusable list.

	std::vector<ref<focusableImplObj>> all_focusable_impls;

	impl->invoke_layoutmanager
		([&]
		 (const ref<peephole_gridlayoutmanagerObj> &lm_impl)
		 {
			 lm_impl->get_vertical_scrollbar()->get_impl
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

			 lm_impl->get_horizontal_scrollbar()->get_impl
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

container pane_peephole_containerObj::get_peephole()
{
	containerptr c;

	impl->invoke_layoutmanager
		([&]
		 (const ref<peephole_gridlayoutmanagerObj> &lm)
		 {
			 c=lm->get_peephole_container();
		 });

	return c;
}

LIBCXXW_NAMESPACE_END
