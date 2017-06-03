/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_containerobj_h
#define editor_containerobj_h

#include "x/w/containerobj.H"
#include "peephole/peepholeobj.H"
#include "editorfwd.H"
#include "editor_containerfwd.H"

LIBCXXW_NAMESPACE_START

//! \ref editor_container "editor_container" object.

class LIBCXX_HIDDEN editor_containerObj : public peepholeObj {

 public:

	class implObj;

	//! The editor element in this container.

	const editor editor_element;

	//! Constructor
	editor_containerObj(const editor &editor_element,
			    const ref<implObj> &impl,
			    const ref<layoutmanager_implObj> &layout_impl);

	//! Destructor
	~editor_containerObj();
};

LIBCXXW_NAMESPACE_END

#endif
