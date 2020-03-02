/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_elementobj_h
#define x_w_list_elementobj_h

#include "listlayoutmanager/list_elementfwd.H"
#include "x/w/element.H"
#include "x/w/focusable.H"

LIBCXXW_NAMESPACE_START

//! List element object.

//! \see list_element

class LIBCXX_HIDDEN list_elementObj : public elementObj,
				      public focusableObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	list_elementObj(const ref<implObj> &impl);

	//! Destructor
	~list_elementObj();

	//! Implement get_impl()
	focusable_impl get_impl() const override;

};

LIBCXXW_NAMESPACE_END

#endif
