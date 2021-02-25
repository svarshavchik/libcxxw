/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themedim_elementfwd_H
#define x_w_impl_themedim_elementfwd_H

#include <x/w/impl/themedimobj.H>
#include <x/w/impl/themedimfwd.H>
#include <x/w/screenobj.H>
#include <x/ptrfwd.H>
#include <tuple>
#include <functional>

LIBCXXW_NAMESPACE_START

template<typename tag=void>
class LIBCXX_PUBLIC themedim_element;
template<typename base, typename ...args>
class themedims_elementObj;
template<typename ...args> struct themedims_elements;

//! Template for attaching one or more themedimObj mixins to a widget.

//! The first template parameter is a subclass of elementObj::implObj,
//! and \c themedim_elementObj multiply-inherits from the template parameter
//! and a \ref themedimObj "themedimObj" via an intermediate
//! themedim_element template, using the tag specified by the optional
//! second template parameter (which defaults to \c void ).
//!
//! The template overrides initialize() and theme_updated(), in order to
//! keep the themedimObj updated.
//!
//! Additional template parameters may follow. Each parameter assign
//! a label to a themdimObj. The actual classes are not used and do not need
//! to be defined. Two or more labels result in multiple reference dimensions,
//! and multiply-inherited themedimObj. Each one's value gets referenced as
//! them as themedim_element<X>::pixels(IN_THREAD),
//! themedim_element<Y>::pixels(IN_THREAD).
//!
//! If optional template parameters are not specified, a single void template
//! parameter gets used to mix in a single dimension.
//!
//! The first parameter to the constructor is a std::tuple with the same number
//! number of values as the number of dimensions. It will be more efficient
//! construct this parameter with \c std::forward_as_tuple, to avoid
//! copying.
//!
//! Each value in the std::tuple is itself a two-value tuple that gives the
//! parameters to \ref themedimObj "themedimObj"'s constructor: a
//! \ref dim_arg "dim_arg" and a \ref themedimaxis "themedimaxis". The
//! \ref dim_arg "dim_arg" parameter is also forwardable as a reference
//! (both of them are, but not much to be gained from forwarding a simple
//! \ref themedimaxis "themedimaxis" value.
//!
//! The remaining parameters get forwarded to the elementObj::implObj
//! superclass.
//!
//! \c themedim_elementObj is an alias that supplies the default \c void
//! dimension tag, if none are supplied.

template<typename base, typename arg1=void, typename ...args>
using themedim_elementObj=themedims_elementObj<base, arg1, args...>;

//! Multiply inherit from the element and all themedims_elements

//! Used by \ref themedim_elementObj "themedim_elementObj" to multiply
//! inherit from its first template parameter and all remaining
//! parameters constructing an appropriate subclass of
//! \ref themedims_elements "themedims_elements".

template<typename base, typename ...args>
class themedims_elementObj : public base,
			     public themedims_elements<args...> {

 public:

	template<typename ...TupleTypes, typename ...ExtraArgs>
		themedims_elementObj(const std::tuple<TupleTypes...> &dimArgs,
				     ExtraArgs && ...Args);

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
		override;

	~themedims_elementObj();
};

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


template<typename ...args> struct themedims_elements
	: themedim_element<args>... {


 public:

	template<typename ...TupleTypes, size_t ...indexes>
		themedims_elements(std::index_sequence<indexes...>,
				   const ref<screenObj::implObj> &screen_impl,
				   const std::tuple<TupleTypes...> &dimArgs);
};


LIBCXXW_NAMESPACE_END

#endif
