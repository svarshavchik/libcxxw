/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_selector_impl.H"

LIBCXXW_NAMESPACE_START

color_picker_selectorObj
::color_picker_selectorObj(const popup_attachedto_info &attachedto_info,
			   const ref<implObj> &impl,
			   const layout_impl &lm_impl)
	: superclass_t{attachedto_info, impl, lm_impl},
	  impl{impl}
{
}

color_picker_selectorObj::~color_picker_selectorObj()=default;

LIBCXXW_NAMESPACE_END
