/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef progressbar_impl_h
#define progressbar_impl_h

#include "progressbar_handler.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_elementobj.H"
#include "progressbar_sliderfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! Private implementation of progressbarObj, holds refs to the
//! content element, and the slider element.
class LIBCXX_HIDDEN progressbarObj::implObj : virtual public obj {

 public:

	//! The actual internal implementation

	//! Container implementation object, with the grid layout manager.

	const ref<handlerObj> handler;

	//! The progressbar contents.

	const container contents;

	//! The slider

	const progressbar_slider slider;

	//! Constructor
	implObj(const ref<handlerObj> &handler,
		const container &contents,
		const progressbar_slider &slider);


	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
