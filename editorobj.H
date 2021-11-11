/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editorobj_h
#define editorobj_h

#include "editorfwd.H"
#include "x/w/elementobj.H"
#include "x/w/focusableobj.H"
#include "x/w/impl/focus/focusablefwd.H"
#include "x/w/input_field.H"
#include "peephole/peepholed_fontelementfwd.H"

LIBCXXW_NAMESPACE_START

//! An editor window.

//! \see editor

class LIBCXX_HIDDEN editorObj
	: public peepholed_fontelementObj<peepholed_elementObj<elementObj>>,
	  public focusableObj {

	//! Alias for my mixin superclass.

	typedef peepholed_fontelementObj<peepholed_elementObj<
		elementObj>> superclass_t;

public:
	const ref<editor_implObj> impl;

	//! Constructor
	editorObj(const ref<editor_implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~editorObj() LIBCXX_HIDDEN;

	//! Inherited from focusableObj
	focusable_impl get_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
