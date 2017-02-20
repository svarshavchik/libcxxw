/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef corner_border_h
#define corner_border_h

#include "child_elementobj.H"
#include "x/w/grid_elementfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "straight_borderfwd.H"
#include "scratch_and_mask_buffer_drawfwd.H"
#include "corner_borderfwd.H"
#include "corner_borderobj.H"
#include "border_implobj.H"
#include "generic_window_handler.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN corner_borderObj::implObj
	: public scratch_and_mask_buffer_draw<child_elementObj> {

 public:
	//! Metadata: the elements and the straight borders that surround me.

	struct surrounding_elements_info {

		surrounding_elements_info();
		~surrounding_elements_info();

		//! The top/left element, maybe null if this is the grid edge.
		grid_elementptr topleft;

		//! The bottom/right element, maybe null if this is the grid edge.
		grid_elementptr topright;

		//! The bottom/left element, maybe null if this is the grid edge.
		grid_elementptr bottomleft;

		//! The bottom/right element, maybe null if this is the grid edge.
		grid_elementptr bottomright;

		//! Border that enters this corner from the top.

		straight_borderptr fromtop_border;

		//! Border that enters this corner from the bottom.

		straight_borderptr frombottom_border;

		//! Border that enters this corner from the left.

		straight_borderptr fromleft_border;

		//! Border that enters this corner from the right.

		straight_borderptr fromright_border;

		//! Pointer to one of the borders.
		typedef straight_borderptr surrounding_elements_info::*from_b;

		//! Find same border that can be drawn as a corner border.

		//! If border1 and border2 are the same, we can invoke its
		//! draw_corner(), for which_corners, and logically-and
		//! mask into flags.
		//!
		//! The border, and which_corners, gets added to the borders
		//! vector.

		void get_same_border(IN_THREAD_ONLY,
				     from_b border1,
				     from_b border2,
				     int which_corners,
				     int &flags,
				     int mask,
				     std::vector<std::tuple<const_border_impl,
				     int>> &borders);

		//! Pick a border for drawing.

		//! If which_border is defined, add it, together with an
		//! opaque "flag" to the borders vector.

		void pick_border(IN_THREAD_ONLY,
				 from_b which_border,
				 int flag,
				 std::vector<std::tuple<const_border_impl,
				 int>> &borders);

		//! Default comparison operator

		bool operator==(const surrounding_elements_info &) const;

		//! Default copy constructor

		surrounding_elements_info(const surrounding_elements_info &);
	};

 private:
	//! The elements and the straight borders that surround me.

	surrounding_elements_info surrounding_elements_thread_only;

	//! When rebuilding the grid, the updated surrounding_elements metadata.

	//! After the grid gets rebuilt, this is compared with
	//! surrounding_elements in order to determine if the corner is
	//! unchanged.

	surrounding_elements_info old_surrounding_elements_thread_only;

 public:

	THREAD_DATA_ONLY(surrounding_elements);
	THREAD_DATA_ONLY(old_surrounding_elements);

	//! The grid layout manager has updated the surrounding_elements()

	void updated(IN_THREAD_ONLY);

	//! Constructor
	implObj(const ref<containerObj::implObj> &container);

 private:
	//! Internal constructor
	implObj(const ref<containerObj::implObj> &container,
		generic_windowObj::handlerObj &h);

	//! Destructor
	~implObj();

	//! Initialize this element.
	void initialize(IN_THREAD_ONLY) override;

	//! The current theme has changed.
	void theme_updated(IN_THREAD_ONLY) override;

	//! Recalculate my metrics.
	void compute_metrics(IN_THREAD_ONLY);

	//! Draw the border.
	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const picture &area_picture,
		     const pixmap &area_pixmap,
		     const gc &area_gc,
		     const picture &mask_picture,
		     const pixmap &mask_pixmap,
		     const gc &mask_gc,
		     const rectangle &area_entire_rect) override;
};

LIBCXXW_NAMESPACE_END

#endif
