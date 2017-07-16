/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_popup.H"

LIBCXXW_NAMESPACE_START

custom_combobox_popupObj
::custom_combobox_popupObj(const ref<generic_windowObj::handlerObj> &parent,
			   const popup_attachedto_info &attachedto_info,
			   size_t nesting_level)
	: superclass_t(parent, attachedto_info, nesting_level)
{
}

custom_combobox_popupObj::~custom_combobox_popupObj()=default;

LIBCXXW_NAMESPACE_END
