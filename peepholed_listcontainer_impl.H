/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainer_impl_h
#define peepholed_listcontainer_impl_h

#include "peepholed_listcontainerobj.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/containerobj.H"
#include "reference_font_elementfwd.H"
#include "themedimfwd.H"
#include "metrics_horizvertfwd.H"
#include "child_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! The internal-internal implementation object for a peepholed listcontainer.

//! \see peepholed_listcontainer
//!
//! This is a mixin that gets combined with the actual list container element
//! by peepholed_listcontainer_impl_elementObj template, which provides
//! the implementations for the abstract methods.

class LIBCXX_HIDDEN peepholed_listcontainerObj::implObj : virtual public obj {

 public:

	//! The size peephole to this many rows, based on the reference font.
	const size_t rows;

	//! The default font for labels in the list container.
	static const char default_list_font[];

	//! Constructor
	implObj(const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Return the list reference font.

	//! Implemented by the mixin template.
	virtual const reference_fontObj &list_reference_font() const=0;

	//! Return padding amount.

	//! Implemented by the mixin template.
	virtual const_themedim list_v_padding() const=0;

	//! Return the element is mixin is a part of.

	//! Implemented by the mixin template.
	virtual child_elementObj &get_child_elementObj()=0;

	//! Return the element is mixin is a part of.

	//! Implemented by the mixin template.
	virtual const child_elementObj &get_child_elementObj() const=0;

	//! Update_peephole_metrics().
	void initialize(IN_THREAD_ONLY);

	//! Call update_peephole_metrics().
	void horizvert_updated(IN_THREAD_ONLY);

	//! Compute the height of the single row.

	//! Adds the vertical padding, specified in the list style,
	//! to the font's height.
	dim_t rowsize(IN_THREAD_ONLY) const;

	//! Stub, does nothing.
	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme);

 private:

	//! Recalculate and update my peephole's metrics.
	void update_peephole_metrics(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
