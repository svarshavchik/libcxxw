/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editorobj_h
#define editorobj_h

#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! An editor window.

//! \see editor

class LIBCXX_HIDDEN editorObj : public elementObj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	editorObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~editorObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
