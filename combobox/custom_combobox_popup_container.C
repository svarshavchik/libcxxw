/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include "peephole/peepholed_toplevel_element.H"
#include "peepholed_toplevel_listcontainer/element.H"
#include "peepholed_toplevel_listcontainer/impl_element.H"

LIBCXXW_NAMESPACE_START

custom_combobox_popup_containerObj
::custom_combobox_popup_containerObj(const ref<implObj> &impl,
				     const ref<listlayoutmanagerObj
				     ::implObj> &listlayout_impl,
				     const popup_attachedto_info
				     &attachedto_info)
	: superclass_t{attachedto_info, impl,
		listlayout_impl->list_element_singleton->impl,
		impl,
		listlayout_impl},
	  impl{impl},
	  listlayout_impl{listlayout_impl}
{
}

custom_combobox_popup_containerObj::~custom_combobox_popup_containerObj()
=default;

LIBCXXW_NAMESPACE_END
