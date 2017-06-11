/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainer_impl_h
#define peepholed_listcontainer_impl_h

#include "peepholed_listcontainerobj.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "reference_font_elementfwd.H"
#include "x/w/containerobj.H"
#include "x/w/listlayoutmanagerfwd.H"

LIBCXXW_NAMESPACE_START

//! The internal-internal implementation object for a peepholed listcontainer.

//! \see peepholed_listcontainer

class LIBCXX_HIDDEN peepholed_listcontainerObj::implObj
	: public reference_font_elementObj<listcontainerObj::implObj> {

	//! The superclass
	typedef reference_font_elementObj<listcontainerObj::implObj
					  > superclass_t;

 public:

	//! The size peephole to this many rows, based on the reference font.
	const size_t rows;

	//! Set the default font for labels in the list container.
	const char *label_theme_font() const override;

	//! Constructor
	implObj(const ref<containerObj::implObj> &parent,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Override initialize() to update_peephole_metrics().
	void initialize(IN_THREAD_ONLY) override;

	//! Override horizvert_updated() to update_peephole_metrics().
	void horizvert_updated(IN_THREAD_ONLY) override;

	//! Compute the height of the single row.

	//! Adds the vertical padding, specified in the list style,
	//! to the font's height.
	dim_t rowsize(IN_THREAD_ONLY) const;

 private:

	//! Recalculate and update my peephole's metrics.
	void update_peephole_metrics(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
