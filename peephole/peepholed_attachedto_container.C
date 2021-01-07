/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peepholed_attachedto_container_impl.H"
#include "peephole/peepholed_fontelement.H"
#include "peephole/peepholed_toplevel_element.H"
#include "popup/popup_attachedto_info.H"

LIBCXXW_NAMESPACE_START

peepholed_attachedto_containerObj::
peepholed_attachedto_containerObj(const popup_attachedto_info &attachedto_info,
				  const ref<implObj> &impl,
				  const layout_impl &lm_impl)
	: superclass_t{impl, impl, lm_impl},
	  impl{impl},
	  attachedto_info{attachedto_info}
{
}

void peepholed_attachedto_containerObj
::recalculate_peepholed_metrics(ONLY IN_THREAD,	const screen &s)
{
	max_width_value=attachedto_info->max_peephole_width(IN_THREAD, s);
	max_height_value=attachedto_info->max_peephole_height(IN_THREAD, s);
}

dim_t peepholed_attachedto_containerObj::max_width(ONLY IN_THREAD) const
{
	return max_width_value;
}

dim_t peepholed_attachedto_containerObj::max_height(ONLY IN_THREAD) const
{
	return max_height_value;
}

peepholed_attachedto_containerObj::~peepholed_attachedto_containerObj()=default;

LIBCXXW_NAMESPACE_END
