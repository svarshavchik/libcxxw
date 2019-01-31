/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridtemplate_h
#define gridtemplate_h

#include "gridtemplatefwd.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/factoryfwd.H"
#include "x/w/shortcut.H"
#include "x/w/containerfwd.H"
#include <x/obj.H>
#include <x/functionalrefptr.H>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

//! Temporary information carried out while generating the contents of a grid.

//! Gets passed to gridlayoutmanagerObj::create(). Keeps track of all the
//! element generators.

class LIBCXX_HIDDEN gridtemplate {

public:

	//! Our generators

	std::unordered_map<std::string,
			   functionref<void (const factory &)>> generators;

	//! Our standalone shortcuts, currently used for book pages.

	std::unordered_map<std::string, shortcut> shortcuts;

	//! The internal containers that get created.
	std::unordered_map<std::string, container> new_layouts;

	//! Execute a generator

	void generate(const factory &f,
		      const std::string &name) const;
};

LIBCXXW_NAMESPACE_END

#endif
