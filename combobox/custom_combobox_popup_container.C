/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "textlistlayoutmanager/textlist_impl.H"
#include "peephole/peepholed_toplevel_element.H"
#include "peepholed_toplevel_listcontainer/element.H"
#include "peepholed_toplevel_listcontainer/impl_element.H"

LIBCXXW_NAMESPACE_START

custom_combobox_popup_containerObj
::custom_combobox_popup_containerObj(const ref<implObj> &impl,
				     const ref<textlistlayoutmanagerObj
				     ::implObj> &layout_impl,
				     const popup_attachedto_info
				     &attachedto_info)
	: superclass_t(attachedto_info, impl,
		       layout_impl->textlist_element->impl,
		       impl,
		       layout_impl),
	  impl(impl),
	  layout_impl(layout_impl)
{
}

custom_combobox_popup_containerObj::~custom_combobox_popup_containerObj()
=default;

LIBCXXW_NAMESPACE_END
