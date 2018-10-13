/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "itembutton_impl.H"
#include "x/w/image_button.H"

LIBCXXW_NAMESPACE_START

itembuttonObj::itembuttonObj(const ref<implObj> &impl,
			     const layout_impl &container_layout_impl,
			     const image_button &deletebutton)
	: containerObj{impl, container_layout_impl},
	  impl{impl},
	  deletebutton{deletebutton}
{
}

itembuttonObj::~itembuttonObj()=default;

LIBCXXW_NAMESPACE_END
