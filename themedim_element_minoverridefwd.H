/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedim_element_minoverridefwd_H
#define themedim_element_minoverridefwd_H

#include "x/w/impl/themedim_elementfwd.H"
#include "x/w/dim_arg.H"

LIBCXXW_NAMESPACE_START

struct themedim_element_override_width_tag;
struct themedim_element_override_height_tag;

/*!
Template for a mixin that installs a minimum width and height overrides
into an element.

The template parameter is a subclass of elementObj. The resulting template
inherits from this subclass.

The first two parameters to the constructor are dim_args. The remaining
parameters get forwarded to the superclass's constructor.

The resulting template class uses set_minimum_override() to set the minimum
width or height of this display element.

*/

template<typename element_impl>
class LIBCXX_HIDDEN themedim_element_minoverrideObj
	: public themedim_elementObj<element_impl,
				     themedim_element_override_width_tag,
				     themedim_element_override_height_tag> {

	//! Alias

	typedef themedim_elementObj<element_impl,
				    themedim_element_override_width_tag,
				    themedim_element_override_height_tag
				    > superclass_t;

 public:
	template<typename ...Args>
		themedim_element_minoverrideObj(const dim_arg &width_override,
						const dim_arg &height_override,
						Args && ...args);

	~themedim_element_minoverrideObj();

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override;
 private:
	void compute_minimum_override(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
