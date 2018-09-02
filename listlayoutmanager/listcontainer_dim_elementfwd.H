/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainer_dim_elementfwd_h
#define listcontainer_dim_elementfwd_h

#include "x/w/impl/themedim_elementfwd.H"
#include "x/w/listlayoutmanager.H"

LIBCXXW_NAMESPACE_START

// Tags for padding between list items: bottom/bottom botting, and the default
// horizontal padding, plus explicit paddings.

struct listcontainer_dim_v;
struct listcontainer_dim_h;

//! We need to also keep track of the individual cell paddings.

class listcontainer_cell_paddingsObj : virtual public obj {

	const std::unordered_map<size_t, std::tuple<dim_arg, dim_arg>
				 > lr_paddings;

	std::unordered_map<size_t, std::tuple<dim_t, dim_t>
			   > lr_padding_pixels_thread_only;


public:
	THREAD_DATA_ONLY(lr_padding_pixels);

	//! Constructor
	listcontainer_cell_paddingsObj(const new_listlayoutmanager &);

	//! Destructor
	~listcontainer_cell_paddingsObj();

	//! Calculate the size in pixels.
	void initialize(ONLY IN_THREAD,
			elementObj::implObj &element_impl);

	//! Recalculate the size in pixels.
	void theme_updated(ONLY IN_THREAD,
			   const defaulttheme &new_theme);

	std::tuple<dim_t, dim_t> get_paddings(ONLY IN_THREAD,
					      size_t,
					      dim_t) const;
};

//! List padding dimension mixin.

//! The template parameter is a subclass of listcontainer_pseudo_implObj, that
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
				   listcontainer_dim_h>,
	public listcontainer_cell_paddingsObj
{

	//! The superclass
	typedef themedim_elementObj<element_impl,
				     listcontainer_dim_v,
				     listcontainer_dim_h
				    > superclass_t;

 public:
	template<typename ...Args>
		inline listcontainer_dim_elementObj(const new_listlayoutmanager &,
						    Args && ...args);

	~listcontainer_dim_elementObj();

	void initialize(ONLY IN_THREAD)
		override;

	void theme_updated(ONLY IN_THREAD,
			   const defaulttheme &new_theme)
		override;

	dim_t list_v_padding(ONLY IN_THREAD) const override;

	std::tuple<dim_t, dim_t> get_paddings(ONLY IN_THREAD, size_t)
		const override;
};

LIBCXXW_NAMESPACE_END

#endif
