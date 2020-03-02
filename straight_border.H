/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef straight_border_h
#define straight_border_h

#include "x/w/impl/child_elementobj.H"
#include "x/w/impl/scratch_and_mask_buffer_drawfwd.H"
#include "x/w/impl/current_border_implfwd.H"
#include "x/w/impl/border_implobj.H"
#include "x/w/scratch_bufferfwd.H"
#include "x/w/generic_windowobj.H"
#include "x/w/containerobj.H"
#include "straight_borderfwd.H"
#include "straight_borderobj.H"
#include "corner_borderfwd.H"
#include "x/w/connection_threadfwd.H"
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Base class for \ref straight_border "straight borders".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::straight_border::base

class LIBCXX_HIDDEN straight_borderBase {

public:

	//! Create a horizontal border.

	static straight_border_factory_t create_horizontal_border;

	//! Create a vertical border

	static straight_border_factory_t create_vertical_border;

	//! Update a horizontal border

	//! When updating a grid's borders, we can optimize things a bit,
	//! and check if the existing border is unchanged.
	//!
	//! This returns existing_border if it's the same one as what
	//! create_horizontal_border() would create, otherwise this calls
	//! create_horizontal_border.

	static straight_border_update_t update_horizontal_border;

	//! Update a vertical border

	//! When updating a grid's borders, we can optimize things a bit,
	//! and check if the existing border is unchanged.
	//!
	//! This returns existing_border if it's the same one as what
	//! create_vertical_border() would create, otherwise this calls
	//! create_vertical_border.

	static straight_border_update_t update_vertical_border;
};

//! A horizontal or a vertical border element implementation object.

//! This is an abstract class. Logic that's specific to a horizontal
//! or a vertical border is defined by an internal subclass.

class LIBCXX_HIDDEN straight_borderObj::implObj
	: public scratch_and_mask_buffer_draw<child_elementObj> {

	struct possible_borders {

		grid_elementptr element_1;
		current_border_implptr border_1;
		grid_elementptr element_2;
		current_border_implptr border_2;
		current_border_implptr border_default;
	};

	//! One of these borders gets picked by best_border().

	possible_borders borders_thread_only;

	//! Implemented in the border_element_implObj subclass.
	virtual void compute_metrics(ONLY IN_THREAD)=0;

 public:

	THREAD_DATA_ONLY(borders);

	implObj(const container_impl &container,
		const metrics::horizvert_axi &initial_metrics,
		const char *scratch_buffer_label,
		const grid_elementptr &element_1,
		const current_border_implptr &border1,
		const grid_elementptr &element_2,
		const current_border_implptr &border2,
		const current_border_implptr &border_default);
 private:
	implObj(const container_impl &container,
		const metrics::horizvert_axi &initial_metrics,
		const char *scratch_buffer_label,
		generic_windowObj::handlerObj &h,
		const grid_elementptr &element_1,
		const current_border_implptr &border1,
		const grid_elementptr &element_2,
		const current_border_implptr &border2,
		const current_border_implptr &border_default);
 public:
	//! Destructor
	~implObj();

	friend class corner_borderObj;

 protected:

	//! Return the best border for us to draw by this element.

	//! There's more than meets the eye to the index of an inherited
	//! border in the borders vector.
	//!
	//! Typically, two sources compete for the same border, such as
	//! the cell above and the cell below the border.
	//!
	//! One source uses an even \c int index, the other source uses
	//! the odd \c int index. That is [0] and [1] are the two competing
	//! sources.
	//!
	//! The best border is picked by looking at the first set of keys in
	//! best_borders. If there's only an odd or an even one, that one
	//! is picked, otherwise we choose the best border between the two.
	//! The remaining borders are ignored.

	const current_border_implptr &best_border(ONLY IN_THREAD) const;

 private:

	//! Subclass implementation invokes the appropriate draw method.
	virtual void draw_horizvert(ONLY IN_THREAD,
				    const border_implObj::draw_info &di)
		const=0;

 public:
	//! Fill background colors of elements above/below this horiz border.
	void background_horizontal(ONLY IN_THREAD,
				   const border_implObj::draw_info &bg) const;

	//! Fill background colors of elements to the left/right of this vert border.
	void background_vertical(ONLY IN_THREAD,
				 const border_implObj::draw_info &bg) const;


	//! Draw the border.
	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const picture &area_picture,
		     const pixmap &area_pixmap,
		     const gc &area_gc,
		     const picture &mask_picture,
		     const pixmap &mask_pixmap,
		     const gc &mask_gc,
		     const clip_region_set &clipped,
		     const rectangle &area_entire_rect) override;
};

LIBCXXW_NAMESPACE_END

#endif
