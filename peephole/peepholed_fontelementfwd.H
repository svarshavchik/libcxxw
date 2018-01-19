/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_fontelementfwd_h
#define peepholed_fontelementfwd_h

#include "peephole/peepholed_elementfwd.H"
#include "reference_fontfwd.H"

LIBCXXW_NAMESPACE_START

//! A mixin for an element in a peephole.

//! The first template parameter must be a subclass of peepholed_elementObj
//! or pepeholed_toplevel_elementObj.
//!
//! Implements horizontal_increment() and
//! vertical_increment().
//!
//! The second template parameter is a tag. The first parameter to the
//! constructor must be a ref<reference_font_tag<tag>>, the remaining
//! template parameters get forwarded to the superclass.
//!
//! Implements horizontal_increment() and vertical_increment() based on
//! the passed in font.

template<typename element_impl, typename font_tag=void>
class LIBCXX_HIDDEN peepholed_fontelementObj : public element_impl {

	//! Alias.

	typedef ref<reference_font_tag<font_tag>> font_impl_t;

	//! The reference font.

	const font_impl_t font_impl;
public:

	//! Constructor
	template<typename ...Args>
		peepholed_fontelementObj(const font_impl_t &font_impl,
					 Args && ...args);

	//! Destructor
	~peepholed_fontelementObj();

	//! Implement horizontal_increment().
	dim_t horizontal_increment(IN_THREAD_ONLY) const override;

	//! Implement vertical_increment().
	dim_t vertical_increment(IN_THREAD_ONLY) const override;

};

LIBCXXW_NAMESPACE_END

#endif
