/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef panefactory_impl_h
#define panefactory_impl_h

#include "x/w/panefactoryobj.H"
#include "grid_map_info.H"
#include "panelayoutmanager/create_pane_info.H"
#include "panelayoutmanager/pane_peephole_containerfwd.H"
#include <optional>

LIBCXXW_NAMESPACE_START

//! Common code shared by all pane factories.

//! Implements most virtual methods defined in panefactoryObj, except for
//! created().

class LIBCXX_HIDDEN panefactory_implObj : public panefactoryObj {

 public:

	//! Temporary info while creating a new pane element.

	typedef mpobj<std::optional<create_pane_info_t>> new_pane_info_t;

	//! New pane element's containers
	new_pane_info_t new_pane_info;

	//! The new pane that was just created.
	mpobj<pane_peephole_containerptr> created_pane_peephole;

	//! Constructor
	panefactory_implObj(const panelayoutmanager &layout);

	//! Destructor
	~panefactory_implObj();

	//! Begin creating a new element.

	container_impl get_container_impl() override;

	//! Implement get_element_impl().
	elementObj::implObj &get_element_impl() override;

	//! Implement last_container_impl().
	container_impl last_container_impl() override;

	//! End of the road. New element has been created.

	//! The append/insert factory subclass implements created(), then
	//! tells us which position the new element goes into, and we
	//! take care of it.
	void created_at(const element &e, size_t position);

	//! Override do_create_focusable_container()

	//! Insert the focusable container into the peephole container's
	//! tabbing order.

	focusable_container
	do_create_focusable_container(const function<void
				      (const focusable_container
				       &)> &creator,
				      const new_focusable_layoutmanager
				      &layout_manager) override;

};

LIBCXXW_NAMESPACE_END

#endif
