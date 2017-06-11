/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef input_field_h
#define input_field_h

#include "x/w/input_fieldobj.H"
#include "container_elementfwd.H"
#include "child_elementobj.H"
#include "editorfwd.H"
#include "nonrecursive_visibilityfwd.H"
#include "focus/focusframecontainerfwd.H"
#include "scrollbar/scrollbarfwd.H"

LIBCXXW_NAMESPACE_START

//! \ref input_field "input_field" implementation class.

class LIBCXX_HIDDEN input_fieldObj::implObj : virtual public obj {

 public:

	//! The actual implementation object is this mixin.

	typedef ref<nonrecursive_visibilityObj<container_elementObj<
		child_elementObj>>> impl_mixin;

	//! The real internal implementation object.

	const impl_mixin impl;

	//! Direct access to the editor element.

	const editor editor_element;

	//! Constructor
	implObj(const impl_mixin &impl,
		const editor &editor_element);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
