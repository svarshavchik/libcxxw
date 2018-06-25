/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_borderlayoutmanager_h
#define x_w_impl_borderlayoutmanager_h

#include "x/w/borderlayoutmanager.H"
#include "x/w/impl/singletonlayoutmanager.H"
#include "x/w/impl/bordercontainer_impl.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN borderlayoutmanagerObj::implObj
	: public singletonlayoutmanagerObj::implObj {

	// Alias
	typedef singletonlayoutmanagerObj::implObj superclass_t;

 public:
	//! Our container
	const ref<bordercontainer_implObj> bordercontainer_impl;

	//! Constructor
	implObj(const container_impl &container_impl,
		const ref<bordercontainer_implObj> &bordercontainer_impl,
		const elementptr &initial_element);

	//! Destructor
	~implObj();

	//! Implement create_public_object().

	layoutmanager create_public_object() override;

	//! Implement get_left_padding()

	dim_t get_left_padding(ONLY IN_THREAD) override;

	//! Implement get_right_padding()

	dim_t get_right_padding(ONLY IN_THREAD) override;

	//! Implement get_top_padding()

	dim_t get_top_padding(ONLY IN_THREAD) override;

	//! Implement get_bottom_padding()

	dim_t get_bottom_padding(ONLY IN_THREAD) override;

	//! Override padded_position()

	rectangle padded_position(ONLY IN_THREAD, const element_impl &e)
		override;

	//! Override child_background_color_changed().
	void child_background_color_changed(ONLY IN_THREAD,
					    const element_impl &child) override;
	//! Override do_draw()

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     clip_region_set &clip,
		     rectangle_set &drawn_areas) override;
};

LIBCXXW_NAMESPACE_END

#endif
