/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholed_item_containerobj_h
#define x_w_peepholed_item_containerobj_h

#include "itemlayoutmanager/peepholed_item_containerfwd.H"
#include "peephole/peepholed_elementfwd.H"
#include "x/w/containerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see peepholed_item_container

class peepholed_item_containerObj : public peepholed_elementObj<containerObj> {

	//! Alias

	typedef peepholed_elementObj<containerObj> superclass_t;

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	peepholed_item_containerObj(const ref<implObj> &impl,
				    const layout_impl &l);

	//! Destructor
	~peepholed_item_containerObj();

	//! Implement horizontal_increment()
	dim_t horizontal_increment(ONLY IN_THREAD) const override;

	//! Implement horizontal_increment()
	dim_t vertical_increment(ONLY IN_THREAD) const override;

	//! Implement peepholed_rows()
	size_t peepholed_rows(ONLY IN_THREAD) const override;
};

LIBCXXW_NAMESPACE_END

#endif
