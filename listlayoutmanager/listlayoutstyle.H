/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listlayoutstyle_h
#define listlayoutstyle_h

#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/listlayoutmanagerobj.H"
#include "x/w/elementfwd.H"
#include "x/w/batch_queuefwd.H"
#include "x/w/gridfactoryfwd.H"
#include "peepholed_listcontainerfwd.H"
#include "layout_style.H"
#include <vector>
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Implements a particular list layout manager style.

class listlayoutstyle : public layout_style_t {

public:

	//! A temporary container for new list items.

	//! The list layout manager creates a new factory for the new row in
	//! the underlying list layout manager, and gives us a vector with
	//! each element for each list item column, and the status change
	//! callback.

	struct new_list_items_t {

		std::vector<element> elements;

		std::function<list_item_status_change_callback_t
			      > status_change_callback;
	};

	//! Creating a new item.

	//! The list layout manager took care of collecting the raw
	//! elements that make up the new row in the list layout manager.
	//!
	//! We are expected to construct a container for each item that uses a
	//! listitemlayoutmanager.

	virtual void create_item(//! The list item layout manager
				 const ref<listlayoutmanagerObj::implObj> &lilm,

				 //! The grid factory, for creating the new item
				 const gridfactory &underlying_factory,

				 //! Queue for queueing recalculations
				 const batch_queue &queue,

				 //! The new elements for the list item
				 const new_list_items_t &new_list_items)
		const=0;

	//! Create a visual separator.

	//! This counts as one item in the list.
	virtual void create_separator(//! The list item layout manager
				      const ref<listlayoutmanagerObj::implObj> &lilm,

				      //! The grid factory, for creating the new item
				      const gridfactory &underlying_factory,

				      //! Queue for queueing recalculations
				      const batch_queue &queue)
		const=0;


	//! Highlight the given list item.

	virtual void highlight(IN_THREAD_ONLY,
			       listlayoutmanagerObj::implObj &layout_manager,
			       grid_map_t::lock &lock,
			       size_t row_number) const=0;

	//! Unhighlight the given list item.

	virtual void unhighlight(IN_THREAD_ONLY,
				 listlayoutmanagerObj::implObj &layout_manager,
				 grid_map_t::lock &lock,
				 size_t row_number) const=0;

	//! Refresh the visual representation of a list item.

	//! The list item has been selected or unselected. Update its
	//! visual appearance accordingly.

	virtual void refresh(IN_THREAD_ONLY,
			     listlayoutmanagerObj::implObj &layout_manager,
			     grid_map_t::lock &lock,
			     size_t i,
			     bool is_highlighted) const=0;

	//! Map a logical column into a physical column.

	//! For the highlighted style they're one and the same.
	//! The bulleted style reserves column 0 for the bullet, so the
	//! logical columns get shifted over by one.

	virtual size_t physical_column(size_t logical_column) const=0;
};

extern const listlayoutstyle &highlighted_list_style;
extern const listlayoutstyle &bulleted_list_style;

//! List layout style
LIBCXXW_NAMESPACE_END

#endif
