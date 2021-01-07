/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_main_windowobj_h
#define peepholed_toplevel_main_windowobj_h

#include "peephole/peepholed_toplevel_elementfwd.H"
#include "x/w/containerobj.H"
#include "x/w/new_layoutmanagerfwd.H"
#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

//! Parent clas of a \ref peepholed_toplevel_main_windowObj "top level peepholed element".

typedef peepholed_toplevel_elementObj<containerObj
				      > toplevel_container_superclass_t;

//! Container with the real contents of the top level display element.

//! \see peepholed_toplevel_main_window

class LIBCXX_HIDDEN peepholed_toplevel_main_windowObj
	: public toplevel_container_superclass_t {

 public:

	class implObj;

	//! "Internal" implementation object.
	const ref<implObj> impl;

	//! Constructor
	peepholed_toplevel_main_windowObj(const ref<implObj> &impl,
					  const layout_impl
					  &container_layout_impl);

	//! Destructor
	~peepholed_toplevel_main_windowObj();

 private:

	//! Implement recalculate_metrics().

	void recalculate_peepholed_metrics(ONLY IN_THREAD,
					   const screen &my_screen) override;

	//! Implement max_width()
	dim_t max_width(ONLY IN_THREAD) const override;

	//! Implement max_height()
	dim_t max_height(ONLY IN_THREAD) const override;

	//! Implement horizontal_increment()
	dim_t horizontal_increment(ONLY IN_THREAD) const override;

	//! Implement vertical_increment()
	dim_t vertical_increment(ONLY IN_THREAD) const override;

	//! Implement peepholed_rows()
	size_t peepholed_rows(ONLY IN_THREAD) const override;
};


LIBCXXW_NAMESPACE_END

#endif
