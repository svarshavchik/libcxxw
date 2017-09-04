/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridtemplate_h
#define gridtemplate_h

#include "gridtemplatefwd.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/gridfactoryfwd.H"
#include <x/obj.H>
#include <functional>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

//! Temporary information carried out while generating the contents of a grid.

//! Keeps track of all the element generators.

class LIBCXX_HIDDEN gridtemplate {

public:

	//! Our generators

	std::unordered_map<std::string,
			   std::function<void (const gridfactory &)>
			   > generators;

	//! Execute a generator

	void generate(const gridfactory &f,
		      const std::string &name) const;
};

LIBCXXW_NAMESPACE_END

#endif
