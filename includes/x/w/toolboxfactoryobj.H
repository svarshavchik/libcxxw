/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_toolboxfactoryobj_h
#define x_w_toolboxfactoryobj_h

#include <x/w/toolboxfactoryfwd.H>
#include <x/w/factoryobj.H>
#include <x/w/uielementsfwd.H>
#include <x/w/uigeneratorsfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see toolboxfactory

class toolboxfactoryObj: public factoryObj {

public:
	//! Constructor
	toolboxfactoryObj() LIBCXX_HIDDEN;

	//! Destructor
	~toolboxfactoryObj() LIBCXX_HIDDEN;

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements);
};

LIBCXXW_NAMESPACE_END

#endif
