/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themedim_elementfwd_H
#define x_w_impl_themedim_elementfwd_H

#include <x/w/impl/themedimobj.H>
#include <x/w/impl/themedimfwd.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_PUBLIC themedim_element_init;
template<typename tag=void>
class LIBCXX_PUBLIC themedim_element;
template<typename element_impl, typename tag=void>
class LIBCXX_PUBLIC themedim1_elementObj;

//! Attach a tag to a themedimObj

//! \see themedim_elementObj

template<typename tag>
class themedim_element : public themedimObj {
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

template<typename element_impl, typename tag>
class themedim1_elementObj : public element_impl,
			     public themedim_element<tag> {

public:

	template<typename ...Args>
	themedim1_elementObj(const dim_arg &dimname,
			     themedimaxis width_or_height,
			     Args && ...args);

	template<typename ...Args>
	themedim1_elementObj(const themedim_element_init &init_param,
			     Args && ...args);

	~themedim1_elementObj();

	//! Override initialize()

	//! Makes sure the dimension gets updated properly, in the event
	//! of the default theme changing after construction.
	void initialize(ONLY IN_THREAD) override;

	//! Recalculate the dimension, after a theme change.
	void theme_updated(ONLY IN_THREAD,
			   const defaulttheme &new_theme) override;
};

//! Helper template used by themedim_elementObj

template<typename ...args> struct LIBCXX_PUBLIC themedims_element_types;

//! Helper template used by themedim_elementObj

template<typename base, typename arg>
struct themedims_element_types<base, arg> {

	typedef themedim1_elementObj<base, arg> type;
};

//! Helper template used by themedim_elementObj

template<typename base, typename arg1, typename arg2, typename ...args>
struct themedims_element_types<base, arg1, arg2, args...> {

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
//! keep the themedimObj updated.
//!
//! Additional template parameters may follow.
//! This makes it possible to have multiple reference dimensions, referring to
//! them as themedim_element<X>::pixels(IN_THREAD),
//! themedim_element<Y>::pixels(IN_THREAD).
//!
//! Each themedimObj takes the leading two parameters to the constructor.
//! That is, if the template results in two themedimObj superclasses, the first
//! four parameters initialize them. In each pair of parameters, the
//! first one is the name of the theme dimension, and the second parameter
//! is either themedimaxis::width or themedimaxis::height. Alternatively,
//! the constructor may also take a themedim_element_init parameter.
//!
//! If optional template parameters are not specified, a single void template
//! parameter gets used to mix in a single dimension.

template<typename base, typename arg1=void, typename ...args>
using themedim_elementObj=typename themedims_element_types<base, arg1, args...
							   >::type;

LIBCXXW_NAMESPACE_END

#endif
