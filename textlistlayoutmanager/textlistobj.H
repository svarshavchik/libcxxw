/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistobj_h
#define x_w_textlistobj_h

#include "textlistlayoutmanager/textlistfwd.H"
#include "x/w/element.H"
#include "x/w/focusable.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see textlist

class LIBCXX_HIDDEN textlistObj : public elementObj,
				  public focusableObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	textlistObj(const ref<implObj> &impl);

	//! Destructor
	~textlistObj();

	//! Implement get_impl()
	ref<focusableImplObj> get_impl() const override;

};

LIBCXXW_NAMESPACE_END

#endif
