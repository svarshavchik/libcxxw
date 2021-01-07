/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_selectorobj_h
#define x_w_color_picker_selectorobj_h

#include "color_picker/color_picker_selectorfwd.H"
#include "peephole/peepholed_attachedto_containerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see color_picker_selector

class LIBCXX_HIDDEN color_picker_selectorObj
	: public peepholed_attachedto_containerObj {

	//! Alias

	typedef peepholed_attachedto_containerObj superclass_t;

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	color_picker_selectorObj(const popup_attachedto_info &,
				 const ref<implObj> &,
				 const layout_impl &);

	//! Destructor
	~color_picker_selectorObj();
};

LIBCXXW_NAMESPACE_END

#endif
