/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themeborder_elementfwd_H
#define themeborder_elementfwd_H

#include "themeborder_element_impl.H"

LIBCXXW_NAMESPACE_START

//! Attach a tag to a themeborder_element_implObj

//! \see themeborder_elementObj
template<typename tag=void>
class LIBCXX_HIDDEN themeborder_element_impl
	: public themeborder_element_implObj {

 public:

	using themeborder_element_implObj::themeborder_element_implObj;

	//! Destructor

	~themeborder_element_impl();
};

//! One themeborder_element instance mixin.

//! Helper used by themeborder_elementObj

template<typename element_impl, typename tag=void>
class LIBCXX_HIDDEN themeborder1_elementObj
	: public element_impl,
	  public themeborder_element_impl<tag> {

public:

	template<typename ...Args>
		themeborder1_elementObj(const border_arg &dimname,
					Args && ...args);

	~themeborder1_elementObj();

	//! Override initialize()

	//! Makes sure the border gets updated properly, in the event
	//! of the default theme changing after construction.
	void initialize(IN_THREAD_ONLY) override;

	//! Recalculate the dimension, after a theme change.
	void theme_updated(IN_THREAD_ONLY,
			   const defaulttheme &new_theme) override;

	elementObj::implObj &get_border_element_impl() override;
};

//! Helper template used by themeborder_elementObj

template<typename ...args> struct themeborder_element_types;

//! Helper template used by themeborder_elementObj

template<typename base, typename arg>
struct LIBCXX_HIDDEN themeborder_element_types<base, arg> {

	typedef themeborder1_elementObj<base, arg> type;
};

//! Helper template used by themeborder_elementObj

template<typename base, typename arg1, typename arg2, typename ...args>
struct LIBCXX_HIDDEN themeborder_element_types<base, arg1, arg2, args...> {

	typedef themeborder1_elementObj<typename
					themeborder_element_types<base, arg2,
								  args...>
					::type, arg1> type;
};

//! Template for attaching one or more themeborder_element_impl mixins to a display element.

//! The first template parameter is a subclass of elementObj::implObj,
//! and themeborder_elementObj multiply-inherits from the template parameter
//! and a \ref themeborder_element_implObj via an intermediate
//! themeborder_element_impl template, using the tag specified by the optional
//! second template parameter (which defaults to \c void ).
//!
//! The template parameter class is not used, and serves to differentiate
//! multiple superclasses of themeborder_element_impl.
//!
//! Additional template parameters may follow.
//! This makes it possible to have multiple borders, referring to
//! them as themeborder_element_impl<X>.
//!
//! The template overrides initialize() and theme_updated(), in order to
//! keep the borders updated.
//!
//! Each themeborder_element_impl superclass that gets inherited from
//! consumes a leading parameter to the constructor, which can be either a
//! border_arg or a current_border_impl.
//!
//! That is, if there are two themedim_element_impl superclasses that
//! get inherited from, the constructor takes two border_arg parameters.
//!
//! The remaining parameters get forwarded to the superclass specified by
//! the first template parameter.
//!
//! If optional parameters are not specified, a single void template
//! parameter gets used to mix in a single border.

template<typename base, typename arg1=void, typename ...args>
using themeborder_elementObj=
	typename themeborder_element_types<base, arg1, args...>::type;

LIBCXXW_NAMESPACE_END

#endif
