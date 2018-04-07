/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainer_dim_elementfwd_h
#define listcontainer_dim_elementfwd_h

#include "x/w/impl/themedim_elementfwd.H"
#include "x/w/listlayoutmanager.H"

LIBCXXW_NAMESPACE_START

// Tags for padding between list items: bottom/bottom botting, and horizontal
// padding - left + right borders, and between multi-column list items.

struct listcontainer_dim_v;
struct listcontainer_dim_left;
struct listcontainer_dim_inner;
struct listcontainer_dim_right;

//! List padding dimension mixin.

//! The template parameter is a subclass of list_container_implObj, that
//! defines list_v_padding(), list_left_padding(), list_right_padding(),
//! and list_inner_padding() virtual methods. This mixin implements them.
//!
//! This is used by container that implement the list layout manager interface.
//!
//! The template inherits from its template parameter. The first argument
//! to the constructor is a new_listlayoutmanager object, the remaining
//! arguments get forwarded to the superclass.

template<typename element_impl>
class LIBCXX_HIDDEN listcontainer_dim_elementObj :
	public themedim_elementObj<element_impl,
				   listcontainer_dim_v,
				   listcontainer_dim_left,
				   listcontainer_dim_inner,
				   listcontainer_dim_right>
{

	//! The superclass
	typedef themedim_elementObj<element_impl,
				     listcontainer_dim_v,
				     listcontainer_dim_left,
				     listcontainer_dim_inner,
				     listcontainer_dim_right
				    > superclass_t;

 public:
	template<typename ...Args>
		inline listcontainer_dim_elementObj(const new_listlayoutmanager &,
						    Args && ...args);

	~listcontainer_dim_elementObj();

	const_themedim list_v_padding() const override;

	const_themedim list_left_padding() const override;

	const_themedim list_right_padding() const override;

	const_themedim list_inner_padding() const override;
};

LIBCXXW_NAMESPACE_END

#endif
