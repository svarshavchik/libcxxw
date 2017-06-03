/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editorobj_h
#define editorobj_h

#include "x/w/elementobj.H"
#include "peephole/peepholed_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! An editor window.

//! \see editor

class LIBCXX_HIDDEN editorObj : public peepholed_elementObj<elementObj> {

	//! Alias for my mixin superclass.

	typedef peepholed_elementObj<elementObj> superclass_t;

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	editorObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~editorObj() LIBCXX_HIDDEN;

	//! Return the preferred horizontal increment
	dim_t horizontal_increment(IN_THREAD_ONLY) const override;

	//! Return the preferred vertical increment
	dim_t vertical_increment(IN_THREAD_ONLY) const override;
};

LIBCXXW_NAMESPACE_END

#endif
