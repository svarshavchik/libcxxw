/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainerobj_h
#define peepholed_listcontainerobj_h

#include "peepholed_listcontainerfwd.H"
#include "listlayoutmanager/listcontainerobj.H"
#include "peephole/peepholedobj.H"
#include "connection_threadfwd.H"
#include "x/w/listlayoutmanagerobj.H"
#include "x/w/elementfwd.H"
#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

//! Peepholed listcontainer object.

//! \see peepholed_listcontainer

class LIBCXX_HIDDEN peepholed_listcontainerObj : public listcontainerObj,
						 public peepholedObj {


 public:

	class implObj;

	//! The implementation object.
	const ref<implObj> impl;

	//! Constructor
	peepholed_listcontainerObj(const ref<implObj> &impl,
				   const ref<listlayoutmanagerObj::implObj>
				   &list_impl);

	//! Destructor
	~peepholed_listcontainerObj();

	//! Inherited from peepholedObj
	element get_element() override;

	//! Inherited from peepholedObj
	dim_t horizontal_increment(IN_THREAD_ONLY) const override;

	//! Inherited from peepholedObj
	dim_t vertical_increment(IN_THREAD_ONLY) const override;
};

LIBCXXW_NAMESPACE_END

#endif
