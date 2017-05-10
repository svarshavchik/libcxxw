/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef defaultthemefwd_h
#define defaultthemefwd_h

#include <x/ptrfwd.H>
#include <x/mpobj.H>
#include "x/w/namespace.H"
#include <string>

LIBCXXW_NAMESPACE_START

//! Location of the directory with all the themes

std::string themedirroot();

class defaultthemeObj;
class defaultthemeBase;

//! A loaded application theme.

typedef ref<defaultthemeObj, defaultthemeBase> defaulttheme;

//! Current theme, mutex protected.

//! The current theme must be consulted when constructing new
//! display elements, and it is also accessed by the connection thread
//! as well.


typedef mpobj<defaulttheme> current_theme_t;

#if 0
class current_theme_t : public mpobj<defaulttheme> {

public:
	using mpobj<defaulttheme>::mpobj;

	class lock : public mpobj<defaulttheme>::lock {

	public:
		static int counter;

		lock(mpobj<defaulttheme> &l)
			: mpobj<defaulttheme>::lock(((std::cout << "LOCK "
						      << ++counter
						      << std::endl), l))
		{
			if (counter == 144)
				abort();
		}

		~lock()
		{
			std::cout << "UNLOCK" << std::endl;
		}
	};
};
#endif

LIBCXXW_NAMESPACE_END

#endif
