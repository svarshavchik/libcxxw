/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themedim_element_H
#define x_w_impl_themedim_element_H

#include <x/w/impl/themedim_elementfwd.H>
#include <x/w/impl/themedim.H>
#include <x/w/impl/get_window_handler.H>
#include <x/refptr_traits.H>
#include <type_traits>

LIBCXXW_NAMESPACE_START

//! Alternative constructor parameter to \ref #themedim_elementObj "themedim_elementObj"

//! The regular constructor takes a \ref dim_arg "dim_arg" and a
//! \ref themedimaxis "themedimaxis" parameter, forwarding the remaining
//! parameters to the superclass.
//!
//! The computed number of pixels isn't known until
//! \ref themedimObj "themedimObj" get constructed. It would be convenient
//! to have that before construcing everything, in order to figure out the
//! required metrics.
//!
//! Pass this themedim_element_init as an alternative. Construct this
//! themedim_element_init using:
//!
//! - the parent container_impl or element_impl.
//!
//! - the \ref dim_arg "dim_arg" and \ref themedimaxis "themedimaxis".
//!
//! You can now peek at \c pixels, do some calculations, then pass this
//! themedim_element_init to the constructor.

struct themedim_element_init {

	//! Captured dimension name argument.

	dim_arg dimname;

	//! Computed number of pixels.

	dim_t pixels;

	//! Captured axis

	themedimaxis width_or_height;

	//! Constructor

	//! The first parameter to a constructor must be either container_impl
	//! or element_impl. The parameter must be a ref to
	//! get_window_handlerObj.

	template<typename must_be_ref,
		 typename=std::enable_if_t<
			 std::is_same_v<
				 std::remove_cv_t<
					 std::remove_reference_t<must_be_ref>>,
				 typename refptr_traits<std::remove_cv_t
							<std::remove_reference_t
							 <must_be_ref>>>::ref_t
				 >>>
	inline themedim_element_init(must_be_ref &&r,
				     const dim_arg &dimname,
				     themedimaxis width_or_height)
		: themedim_element_init{dimname, width_or_height, *r}
	{
	}

private:

	//! Delegated constructor
	themedim_element_init(const dim_arg &dimname,
			      themedimaxis width_or_height,
			      get_window_handlerObj &gw);
public:
	//! Destructor
	~themedim_element_init();
};

template<typename tag>
inline themedim themedim_element<tag>::getref() { return themedim(this); }

template<typename tag>
inline const_themedim themedim_element<tag>::getref() const
{
	return const_themedim(this);
}

//! Destructor
template<typename tag>
themedim_element<tag>::~themedim_element()=default;

//! Constructor

//! The first parameter specifies the dimension's, the next either
//! get_theme_height/width_dim_t, and the remaining
//! parameters get forwarded to the superclass.

template<typename element_impl, typename tag>
template<typename ...Args>
themedim1_elementObj<element_impl, tag>
::themedim1_elementObj(const dim_arg &dimname,
		       themedimaxis width_or_height,
		       Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  themedim_element<tag>(dimname, this->get_screen()->impl,
				width_or_height)
{
}
//! Constructor

//! The first parameter is a themedim_element_init, and the remaining
//! parameters get forwarded to the superclass.

template<typename element_impl, typename tag>
template<typename ...Args>
themedim1_elementObj<element_impl, tag>
::themedim1_elementObj(const themedim_element_init &init,
		       Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  themedim_element<tag>(init)
{
}

//! Default destructor
template<typename element_impl, typename tag>
themedim1_elementObj<element_impl, tag>
::~themedim1_elementObj()=default;

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the themedimObj::theme_updated().
//!
//! Addresses the potential race condition with the theme changing before
//! initialization.
template<typename element_impl, typename tag>
void themedim1_elementObj<element_impl, tag>
::initialize(ONLY IN_THREAD)
{
	themedim_element<tag>::initialize(IN_THREAD,
					  this->get_screen()->impl);
	element_impl::initialize(IN_THREAD);
}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the themedimObj::theme_updated().

template<typename element_impl, typename tag>
void themedim1_elementObj<element_impl, tag>
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	themedim_element<tag>::theme_updated(IN_THREAD, new_theme);
	element_impl::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
