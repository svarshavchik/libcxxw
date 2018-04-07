/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_h
#define image_h

#include "x/w/impl/child_element.H"
#include "x/w/image.H"
#include "connection_thread.H"
#include "iconfwd.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN imageObj::implObj : public child_elementObj {

	//! The currently shown image.

	icon current_icon_thread_only;

public:
	THREAD_DATA_ONLY(current_icon);

	//! Constructor
	implObj(const ref<containerObj::implObj> &container,
		const icon &initial_icon);

private:
	//! Internal constructor
	implObj(const ref<containerObj::implObj> &container,
		const icon &initial_icon,
		dim_t icon_width,
		dim_t icon_height);
public:
	//! Constructor
	implObj(const ref<containerObj::implObj> &container,
		const icon &initial_icon,
		const metrics::axis &horiz_metrics,
		const metrics::axis &vert_metrics);

	//! Inherited from elementObj::implObj

	void initialize(ONLY IN_THREAD) override;

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectangle_set &areas) override;

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;

	///////////////////////////////////////////////////////

	//! Set new icon.

	void set_icon(ONLY IN_THREAD, const icon &new_icon);

 protected:

	//! Compute new metrics for this image element.

	//! The default implementation sets the metrics from the
	//! icon's width and height.

	virtual void update_image_metrics(ONLY IN_THREAD);

};

LIBCXXW_NAMESPACE_END

#endif
