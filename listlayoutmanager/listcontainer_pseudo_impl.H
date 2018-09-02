/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainer_pseudo_impl_h
#define listcontainer_pseudo_impl_h

#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/containerobj.H"
#include "x/w/impl/themedimfwd.H"

LIBCXXW_NAMESPACE_START

//! The pseudo-container with a \ref listlayoutmanager "list layout manager".

//! Base class for a container that uses the list layout manager.
//!
//! The container has one element, the \ref list_element "list element", that
//! draws the items in the list. This container makes it possible to use
//! the layout manager framework for managing list items, even though they
//! are not, themselves, discrete display elements.
//!
//! This base class defines virtual method that the list element uses to
//! obtain the padding for the list items. The subclass is responsible for
//! implementing the virtual methods, using the
//! \ref listcontainer_dim_elementObj "list padding dimension mixin".

class LIBCXX_HIDDEN listcontainer_pseudo_implObj :
	public container_elementObj<child_elementObj> {

	//! Number of rows currently in the list

	//! Advertized number of rows, used in update_peephole_metrics().

	size_t rows_thread_only=0;
 public:

	THREAD_DATA_ONLY(rows);

	//! Alias
	typedef container_elementObj<child_elementObj> superclass_t;

	//! Constructor
	listcontainer_pseudo_implObj(const container_impl &parent);

	//! Destructor
	~listcontainer_pseudo_implObj();

	//! Implemented in the subclass.
	virtual dim_t list_v_padding(ONLY IN_THREAD) const=0;

	//! Implemented in the subclass.
	virtual std::tuple<dim_t, dim_t> get_paddings(ONLY IN_THREAD,
						      size_t) const=0;

	//! Override do_draw().

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectangle_set &areas) override;
 private:

	//! do_draw() saves the visible height of the list contanier.
	dim_t most_recent_visible_height_thread_only;
 public:
	THREAD_DATA_ONLY(most_recent_visible_height);
};

LIBCXXW_NAMESPACE_END

#endif
