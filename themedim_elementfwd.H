/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedim_elementfwd_H
#define themedim_elementfwd_H

#include "themedimobj.H"
#include "themedimfwd.H"

LIBCXXW_NAMESPACE_START

//! Attach a tag to a themedimObj

//! \see themedim_elementObj
template<typename tag=void>
class LIBCXX_HIDDEN themedim_element : public themedimObj {
public:

	using themedimObj::themedimObj;

	//! Destructor
	~themedim_element();

	//! A helper for obtaining a reference to this dimension

	//! obj.themedim_element<tag>.getref() returns a \c themedim
	//! for that dimension.

	themedim getref();

	//! A helper for obtaining a reference to this dimension

	//! obj.themedim_element<tag>.getref() returns a \c themedim
	//! for that dimension.

	const_themedim getref() const;
};

//! One themedim_element instance mixin.

//! Helper used by themedim_elementObj

template<typename element_impl, typename tag=void>
class LIBCXX_HIDDEN themedim1_elementObj : public element_impl,
					   public themedim_element<tag> {

public:

	template<typename ...Args>
	themedim1_elementObj(const std::string &dimname,
			     theme_width_or_height width_or_height,
			     Args && ...args);

	~themedim1_elementObj();

	//! Override initialize()

	//! Makes sure the dimension gets updated properly, in the event
	//! of the default theme changing after construction.
	void initialize(IN_THREAD_ONLY) override;

	//! Recalculate the dimension, after a theme change.
	void theme_updated(IN_THREAD_ONLY,
			   const defaulttheme &new_theme) override;
};

//! Helper template used by themedim_elementObj

template<typename ...args> struct themedims_element_types;

//! Helper template used by themedim_elementObj

template<typename base, typename arg>
struct LIBCXX_HIDDEN themedims_element_types<base, arg> {

	typedef themedim1_elementObj<base, arg> type;
};

//! Helper template used by themedim_elementObj

template<typename base, typename arg1, typename arg2, typename ...args>
struct LIBCXX_HIDDEN themedims_element_types<base, arg1, arg2, args...> {

	typedef themedim1_elementObj<typename
				     themedims_element_types<base, arg2,
							     args...>
				     ::type, arg1> type;
};

//! Template for attaching one or more themedimObj mixins to a display element.

//! The first template parameter is a subclass of elementObj::implObj,
//! and themedim_elementObj multiply-inherits from the template parameter
//! and a \ref themedimObj via an intermediate
//! themedim_element template, using the tag specified by the optional
//! second template parameter (which defaults to \c void ).
//!
//! The template overrides initialize() and theme_updated(), in order to
//! keep the themedimObj updates.
//!
//! Additional template parameters may follow.
//! This makes it possible to have multiple reference dimensions, referring to
//! them as themedim_element<X>::pixels,
//! themedim_element<Y>::pixels.
//!
//! Each themedimObj takes the leading two parameters to the constructor.
//! That is, if the template results in two themedimObj superclasses, the first
//! four parameters initialize them. In each pair of parameters, the
//! first one is the name of the theme dimension, and the second parameter
//! is either &defaultthemeObj::get_theme_width_dim_t or
//! &defaultthemeObj::get_theme_height_dim_t.
//!
//! If optional parameters are not specified, a single void template
//! parameter gets used to mix in a single dimension.

template<typename base, typename arg1=void, typename ...args>
using themedim_elementObj=typename themedims_element_types<base, arg1, args...
							   >::type;

LIBCXXW_NAMESPACE_END

#endif
