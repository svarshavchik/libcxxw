/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_color_elementfwd_H
#define background_color_elementfwd_H

#include "background_colorfwd.H"
#include "connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Attach a tag to a background_colorObj

//! \see background_color_elementObj

class LIBCXX_HIDDEN background_color_element_implObj : virtual public obj {

	//! Wrapped background color
	background_color color;

public:

	//! Constructor
	background_color_element_implObj(const background_color &color);

	//! Set new background color
	void update(IN_THREAD_ONLY, const background_color &new_color);

	// Retrieve the current color.

	background_color get(IN_THREAD_ONLY);

	//! Update the background color
	void theme_updated(IN_THREAD_ONLY);

	//! Destructor
	~background_color_element_implObj();
};

//! Helper class for using tags to attach multiple background color objects to elements

template<typename tag=void>
class LIBCXX_HIDDEN background_color_element
	: public background_color_element_implObj {

 public:

	using background_color_element_implObj::background_color_element_implObj;

	~background_color_element();
};

//! Attach one background color mixin.

//! The constructor forwards its first argument, the initial background color
//! to the background_color_element_implObj.

template<typename element_impl, typename tag=void>
class LIBCXX_HIDDEN background_color_1elementObj
	: public background_color_element<tag>,
	  public element_impl {

 public:
	//! Constructor

	template<typename ...Args>
		background_color_1elementObj(const background_color &color,
					     Args && ...args);

	//! Destructor
	~background_color_1elementObj();

	//! Override initialize()

	//! Makes sure the background color gets updated properly, in the event
	//! of the default theme changing after construction.
	void initialize(IN_THREAD_ONLY) override;

	//! Recalculate the background color, after a theme change.
	void theme_updated(IN_THREAD_ONLY) override;
};

template<typename ...args> struct background_colors_element_types;

//! Helper template used by background_color_elementObj

template<typename base, typename arg>
struct LIBCXX_HIDDEN background_colors_element_types<base, arg> {

	typedef background_color_1elementObj<base, arg> type;
};

//! Helper template used by background_color_elementObj

template<typename base, typename arg1, typename arg2, typename ...args>
struct LIBCXX_HIDDEN background_colors_element_types<base, arg1, arg2, args...>
{

	typedef background_color_1elementObj
		<typename
		 background_colors_element_types<base, arg2, args...>
		 ::type, arg1> type;
};

//! Template for attaching one or more background_colors, as mixins, to a display element.

//! The first template parameter is a subclass of elementObj::implObj.
//! background_color_elementObj multiply-inherits from the template
//! parameter and a \ref background_color_element_implObj via an intermediate
//! background_color_element template, using the tag specified by the optional
//! second template parameter (which defaults to \c void ).
//!
//! The tag is used to create a distinct type only, and is not otherwise used.
//! Additional template parameters may follow, specifying additional tags.
//! This result in multiple inheritances, each one is referenced as
//! background_color_element<tag>.
//!
//! The end result is a multiple inheritance from the first template parameter
//! and background_color_element<T>, with T specified by the template
//! parameters to background_color_elementObj. If none are specified, beyond
//! the parent class, a single inheritance from background_color_element<void>
//! results.
//!
//! Each background_color_element<T> inherits from
//! \ref background_color_element_implObj "background_color_element_implObj",
//! which implements methods for each mixed-in background color.
//!
//! \par Constructors
//!
//! The initial set of parameters to the constructor are the initial
//! \ref background_color "background_color" objects, one for each
//! background_color_element<T> mixin. The remaining constructor parameters
//! get forwarded to the parent elementObj::implObj superclass.

template<typename base, typename arg1=void, typename ...args>
using background_color_elementObj=
	typename background_colors_element_types<base, arg1, args...>::type;

LIBCXXW_NAMESPACE_END

#endif
