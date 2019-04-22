/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uigeneratorsobj_h
#define x_w_uigeneratorsobj_h

#include <x/w/uigeneratorsfwd.H>
#include <x/w/rgb.H>
#include <x/w/border_infomm.H>
#include <x/functionalrefptrfwd.H>
#include <x/obj.H>
#include <x/vectorfwd.H>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see uigenerators

class uigeneratorsObj: virtual public obj {

public:
	//! Constructor
	uigeneratorsObj() LIBCXX_HIDDEN;

	//! Destructor
	~uigeneratorsObj() LIBCXX_HIDDEN;

	//! Custom colors loaded from this layout.

	std::unordered_map<std::string, theme_color_t> colors;

	//! Borders

	std::unordered_map<std::string, border_infomm> borders;

	//! Generators used by the grid layout manager

	std::unordered_map<std::string, vector<gridlayoutmanager_generator>
			   > gridlayoutmanager_generators;

	//! Generators used by the grid factory
	std::unordered_map<std::string, vector<gridfactory_generator>
			   > gridfactory_generators;

	//! Generators used by the book layout manager
	std::unordered_map<std::string, vector<booklayoutmanager_generator>
			   > booklayoutmanager_generators;

	//! Generators used by the book page factory
	std::unordered_map<std::string, vector<bookpagefactory_generator>
			   > bookpagefactory_generators;
};

LIBCXXW_NAMESPACE_END

#endif
