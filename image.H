/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_h
#define image_h

#include "child_element.H"
#include "x/w/image.H"
#include "connection_thread.H"
#include "iconfwd.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN imageObj::implObj : public child_elementObj {

	//! The currently shown image.

	icon current_icon;

public:

	//! Constructor
	implObj(const ref<containerObj::implObj> &container,
		const icon &initial_icon);

private:
	//! Internal constructor
	implObj(const ref<containerObj::implObj> &container,
		const icon &initial_icon,
		dim_t icon_width,
		dim_t icon_height,
		const std::string &scratch_bufer_id);
public:
	//! Inherited from elementObj::implObj

	void initialize(IN_THREAD_ONLY) override;

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas) override;

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;

	///////////////////////////////////////////////////////

	//! Set new icon.

	void set_icon(IN_THREAD_ONLY, const icon &new_icon);
};

LIBCXXW_NAMESPACE_END

#endif
