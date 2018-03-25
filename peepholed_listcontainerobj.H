/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainerobj_h
#define peepholed_listcontainerobj_h

#include "peepholed_listcontainerfwd.H"
#include "peephole/peepholed_elementfwd.H"
#include "listlayoutmanager/listcontainerobj.H"
#include "peephole/peepholedobj.H"
#include "connection_threadfwd.H"
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

	class implObj;

	//! The implementation object.
	const ref<implObj> impl;

	//! Constructor
	peepholed_listcontainerObj(const ref<implObj> &impl,
				   const ref<focusableImplObj> &focusable_impl,
				   const ref<containerObj::implObj> &container_impl,
				   const ref<layoutmanagerObj::implObj>
				   &list_impl);

	//! Destructor
	~peepholed_listcontainerObj();

	//! Inherited from peepholedObj
	dim_t horizontal_increment(ONLY IN_THREAD) const override;

	//! Inherited from peepholedObj
	dim_t vertical_increment(ONLY IN_THREAD) const override;
};

LIBCXXW_NAMESPACE_END

#endif
