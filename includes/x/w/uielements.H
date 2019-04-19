/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uielements_h
#define x_w_uielements_h

#include <x/w/uielementsfwd.H>
#include <x/w/factory.H>
#include <x/w/shortcut.H>
#include <x/w/container.H>
#include <x/functionalrefptr.H>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

//! UI factory

//! Passed to a supported layout manager's generate(), to generate
//! the contents of the container from a template.

struct uielements {

	//! Generators for new display elements.

	//! Referenced by name, from the template.

	std::unordered_map<std::string,
			   functionref<void (const factory &)>> factories;

	//! Standalone shortcuts, currently used for book pages.

	std::unordered_map<std::string, shortcut> shortcuts;

	//! The internal containers that get created.

	//! Generate puts new containers that get created in here.

	std::unordered_map<std::string, container> new_layouts;
};

LIBCXXW_NAMESPACE_END

#endif
