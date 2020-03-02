/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef toolboxlayoutmanager_impl_h
#define toolboxlayoutmanager_impl_h

#include "x/w/toolboxlayoutmanager.H"
#include "x/w/elementfwd.H"
#include "x/w/impl/layoutmanager.H"
#include "size_hints.H"
#include <vector>

LIBCXXW_NAMESPACE_START

//! Mutex-protected elements in the toolbox

class toolbox_info {

public:
	std::vector<element> elements;

	// Largest width and height
	dim_t w, h;
};

//! Internal implementation object.

class toolboxlayoutmanagerObj::implObj : public layoutmanagerObj::implObj,
					 public size_hintsObj {

	//! Elements per row computed by the most recent recalculate().
	size_t current_elements_per_row=0;

	//! Theme was updated.

	//! Use current_elements_per_row() instead of looking at the current
	//! container's size, and set the metrics accordingly, until our
	//! size matches.
	bool theme_update_in_progress=false;

public:

	//! What's in the toolbox
	toolbox_info_t info;

	//! Default width
	const size_t default_width;

	//! Constructor
	implObj(const container_impl &layout_container_impl,
		const new_toolboxlayoutmanager &ntlm);

	//! Destructor
	~implObj();

	//! Implement do_for_each_child().
	void do_for_each_child(ONLY IN_THREAD,
			       const function<void (const element &e)> &)
		override;

	//! Implement num_children().
	size_t num_children(ONLY IN_THREAD) override;


	//! Implement create_public_object().
	layoutmanager create_public_object() override;

	//! Implement recalculate().
	void recalculate(ONLY IN_THREAD) override;

	//! Implement process_updated_position().
	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position) override;

	//! Override theme_updated().
	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
		override;
private:
	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position,
				      toolbox_info_t::lock &lock);

};

LIBCXXW_NAMESPACE_END

#endif
