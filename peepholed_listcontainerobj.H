/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainerobj_h
#define peepholed_listcontainerobj_h

#include "peepholed_listcontainerfwd.H"
#include "peephole/peepholed_elementfwd.H"
#include "listlayoutmanager/listcontainerobj.H"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"
#include "peephole/peepholedobj.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/layoutmanagerobj.H"
#include "x/w/elementfwd.H"
#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

//! Peepholed listcontainer object.

//! \see peepholed_listcontainer

class LIBCXX_HIDDEN peepholed_listcontainerObj
	: public peepholed_elementObj<listcontainerObj> {

	//! Alias

	typedef peepholed_elementObj<listcontainerObj> superclass_t;
 public:

	//! The implementation object.
	const ref< listcontainer_pseudo_implObj> impl;

	//! Constructor
	peepholed_listcontainerObj(const ref<listcontainer_pseudo_implObj>
				   &impl,
				   const focusable_impl &focusable_impl,
				   const layout_impl &list_impl);

	//! Destructor
	~peepholed_listcontainerObj();

	//! Inherited from peepholedObj
	dim_t horizontal_increment(ONLY IN_THREAD) const override;

	//! Inherited from peepholedObj
	dim_t vertical_increment(ONLY IN_THREAD) const override;

	//! Inherited from peepholedObj
	size_t peepholed_rows(ONLY IN_THREAD) const override;
};

LIBCXXW_NAMESPACE_END

#endif
