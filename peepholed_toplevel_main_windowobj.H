/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_main_windowobj_h
#define peepholed_toplevel_main_windowobj_h

#include "peephole/peepholed_toplevel_elementfwd.H"
#include "x/w/containerobj.H"
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
					const new_layoutmanager &factory);

	//! Destructor
	~peepholed_toplevel_main_windowObj();

 private:

	//! Implement recalculate_metrics().

	void recalculate_metrics(IN_THREAD_ONLY) override;

	//! Implement max_width()
	dim_t max_width(IN_THREAD_ONLY) const override;

	//! Implement max_height()
	dim_t max_height(IN_THREAD_ONLY) const override;

	//! Implement horizontal_increment()
	dim_t horizontal_increment(IN_THREAD_ONLY) const override;

	//! Implement vertical_increment()
	dim_t vertical_increment(IN_THREAD_ONLY) const override;
};


LIBCXXW_NAMESPACE_END

#endif