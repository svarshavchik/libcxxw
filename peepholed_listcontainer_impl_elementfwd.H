/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainer_impl_elementfwd_h
#define peepholed_listcontainer_impl_elementfwd_h

#include "peepholed_listcontainer_impl.H"
#include "listlayoutmanager/listcontainer_dim_elementfwd.H"
#include "x/w/impl/theme_font_elementfwd.H"
#include "x/w/impl/themedimfwd.H"
#include "x/w/impl/themedim_axis_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Combine the peepholed_listcontainerObj::implObj with an element.

//! For internal use by peepholed_listcontainer_impl_elementObj, which
//! uses this template, pulls in the font and dimension mixins that
//! peepholed_listcontainerObj::implObj uses; and pulls in the
//! themedim_axis_elementObj mixin to handle the list height when it is
//! specified as a dim_axis_arg. This makes these mixins to
//! be the subclass of both peepholed_listcontainer_impl_elementObj and
//! the template parameter, which must be a subclass of child_elementObj
//! and the container_element mixin.
//!
//! One of the combining elements is listcontainerObj::implObj which also
//! uses the same fond and dimension mixin.
//!
//! The first constructor parameter is a new_listlayoutmanager that gets
//! passed to peepholed_listcontainer_impl_elementObj's constructor.
//! The remaining constructor parameters get passed to the superclass
//! template parameter.

template<typename element_impl>
class LIBCXX_HIDDEN peepholed_listcontainer_impl_element_baseObj
	: public element_impl, public peepholed_listcontainerObj::implObj {

 public:
	template<typename ...Args>
	peepholed_listcontainer_impl_element_baseObj(const new_listlayoutmanager
						     &style,
						     Args && ...args);

	~peepholed_listcontainer_impl_element_baseObj();

	void initialize(ONLY IN_THREAD) override;

	void horizvert_updated(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
		override;

};

//! Combine the peepholed_listcontainerObj::implObj with an element.

//! The template parameter is a superclass. This template multiply-inherits
//! from the template parameter and peepholed_listcontainerObj::implObj.
//!
//! The first template parameter must be \c new_listlayoutmanager, the
//! remaining template parameters get passed to the superclass specified
//! by the template parameter.

template<typename element_impl>
class LIBCXX_HIDDEN peepholed_listcontainer_impl_elementObj :
	public themedim_axis_heightObj<
	theme_font_elementObj<
		listcontainer_dim_elementObj
		<peepholed_listcontainer_impl_element_baseObj<element_impl>>>> {

public:
	typedef themedim_axis_heightObj<theme_font_elementObj<
		listcontainer_dim_elementObj<
			peepholed_listcontainer_impl_element_baseObj<
				element_impl>>>
				       > superclass_t;

	template<typename ...Args>
		peepholed_listcontainer_impl_elementObj
		(const new_listlayoutmanager &style,
		 Args && ...args);

	~peepholed_listcontainer_impl_elementObj();

	const theme_fontObj &list_reference_font()
		const override;

	child_elementObj &get_child_elementObj() override;
	const child_elementObj &get_child_elementObj() const override;

	virtual metrics::axis get_height_metrics(ONLY IN_THREAD) const override;
};

LIBCXXW_NAMESPACE_END

#endif
