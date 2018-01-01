/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef panefactory_impl_h
#define panefactory_impl_h

#include "x/w/panefactoryobj.H"
#include "grid_map_info.H"
#include "panelayoutmanager/create_pane_info.H"
#include "panelayoutmanager/create_pane_properties.H"
#include <optional>

LIBCXXW_NAMESPACE_START

//! Common code shared by all pane factories.

//! Implements most virtual methods defined in panefactoryObj, except for
//! created().

class LIBCXX_HIDDEN panefactory_implObj : public panefactoryObj {

 public:

	//! Lock on the underlying grid.

	grid_map_t::lock lock;

	//! Temporary info while creating a new pane element.

	typedef mpobj<std::optional<create_pane_info_t>> new_pane_info_t;

	//! New pane element's containers
	new_pane_info_t new_pane_info;

	//! Properties for the next pane.
	typedef mpobj<create_pane_properties_t> new_pane_properties_t;

	//! New pane's properties
	new_pane_properties_t new_pane_properties;

	//! Constructor
	panefactory_implObj(const panelayoutmanager &layout);

	//! Destructor
	~panefactory_implObj();

	//! Implement set_initial_size_set();
	void set_initial_size_set(const dim_arg &arg) override;

	//! Implement set_background_color_set().
	void set_background_color_set(const color_arg &arg) override;

	//! Implement left_padding_set().
	void left_padding_set(const dim_arg &) override;

	//! Implement right_padding_set().
	void right_padding_set(const dim_arg &) override;

	//! Implement top_padding_set().
	void top_padding_set(const dim_arg &) override;

	//! Implement bottom_padding_set().
	void bottom_padding_set(const dim_arg &) override;

	//! Implement padding_set().
	void padding_set(const dim_arg &) override;

	//! Implement halign_set().
	void halign_set(LIBCXXW_NAMESPACE::halign) override;

	//! Implement valign_set().
	void valign_set(LIBCXXW_NAMESPACE::valign) override;

	//! Begin creating a new element.

	ref<containerObj::implObj> get_container_impl() override;

	//! Implement get_element_impl().
	elementObj::implObj &get_element_impl() override;

	//! Implement last_container_impl().
	ref<containerObj::implObj> last_container_impl() override;

	//! End of the road. New element has been created.

	//! The append/insert factory subclass implements created(), then
	//! tells us which position the new element goes into, and we
	//! take care of it.
	void created_at(const element &e, size_t position);
};

LIBCXXW_NAMESPACE_END

#endif
