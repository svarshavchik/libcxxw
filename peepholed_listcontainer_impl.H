/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainer_impl_h
#define peepholed_listcontainer_impl_h

#include "peepholed_listcontainerobj.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/containerobj.H"
#include "x/w/dim_axis_arg.H"
#include "x/w/metrics/axis.H"
#include "reference_font_elementfwd.H"
#include "themedimfwd.H"
#include "metrics_horizvertfwd.H"
#include "child_elementfwd.H"
#include <variant>

LIBCXXW_NAMESPACE_START

//! The internal-internal implementation object for a peepholed listcontainer.

//! \see peepholed_listcontainer
//!
//! This is a mixin that gets combined with the actual list container element
//! by peepholed_listcontainer_impl_elementObj template, which provides
//! the implementations for the abstract methods.

class LIBCXX_HIDDEN peepholed_listcontainerObj::implObj : virtual public obj {

 public:

	//! Size peephole to this many rows, based on the reference font.
	const std::variant<size_t, dim_axis_arg> height;

	//! Constructor
	implObj(const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Return the height metrics.

	//! If the height was specified as dim_axis_arg, return the computed
	//! dimensions based on the current theme.

	virtual metrics::axis get_height_metrics(ONLY IN_THREAD) const=0;

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
	void initialize(ONLY IN_THREAD);

	//! Call update_peephole_metrics().
	void horizvert_updated(ONLY IN_THREAD);

	//! Compute the height of the single row.

	//! Adds the vertical padding, specified in the list style,
	//! to the font's height.
	dim_t rowsize(ONLY IN_THREAD) const;

	//! Stub, does nothing.
	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme);

 private:

	//! Recalculate and update my peephole's metrics.
	void update_peephole_metrics(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
