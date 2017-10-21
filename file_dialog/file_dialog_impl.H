/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef file_dialog_impl_h
#define file_dialog_impl_h

#include "x/w/file_dialog.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN file_dialogObj::implObj : virtual public obj {

 public:
	class elementsObj;

	const ref<elementsObj> elements;

	const dialog the_dialog;

	//! Constructor
	implObj(const ref<elementsObj> &elements,
		const dialog &the_dialog);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
