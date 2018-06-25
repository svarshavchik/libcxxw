/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef corner_border_h
#define corner_border_h

#include "x/w/impl/child_elementobj.H"
#include "x/w/impl/scratch_and_mask_buffer_drawfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "straight_borderfwd.H"
#include "corner_borderfwd.H"
#include "corner_borderobj.H"
#include "x/w/impl/border_implobj.H"
#include "generic_window_handler.H"
#include "x/w/connection_threadfwd.H"

#include <vector>
#include <tuple>
#include <optional>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN corner_borderObj::implObj
	: public scratch_and_mask_buffer_draw<child_elementObj> {

	//! Alias

	typedef scratch_and_mask_buffer_draw<child_elementObj> superclass_t;

 public:
	//! Metadata: the elements and the straight borders that surround me.

	struct surrounding_elements_and_borders :
		public surrounding_elements_info {

		surrounding_elements_and_borders();
		~surrounding_elements_and_borders();

		//! Border that enters this corner from the top.

		straight_borderptr fromtop_border;

		//! Border that enters this corner from the bottom.

		straight_borderptr frombottom_border;

		//! Border that enters this corner from the left.

		straight_borderptr fromleft_border;

		//! Border that enters this corner from the right.

		straight_borderptr fromright_border;

		//! Pointer to one of the borders.
		typedef straight_borderptr surrounding_elements_and_borders::*from_b;

		//! Find same border that can be drawn as a corner border.

		//! If border1 and border2 are the same, we can invoke its
		//! draw_corner(), for which_corners, and logically-and
		//! mask into flags.
		//!
		//! The border, and which_corners, gets added to the borders
		//! vector.

		void get_same_border(ONLY IN_THREAD,
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

		void pick_border(ONLY IN_THREAD,
				 from_b which_border,
				 int flag,
				 std::vector<std::tuple<const_border_impl,
				 int>> &borders);

		//! Default comparison operator

		bool operator==(const surrounding_elements_and_borders &) const;

		//! Default copy constructor

		surrounding_elements_and_borders(const surrounding_elements_and_borders &);
	};

 private:
	//! The elements and the straight borders that surround me.

	surrounding_elements_and_borders surrounding_elements_thread_only;

	//! When rebuilding the grid, the updated surrounding_elements metadata.

	//! After the grid gets rebuilt, this is compared with
	//! surrounding_elements in order to determine if the corner is
	//! unchanged.

	surrounding_elements_and_borders old_surrounding_elements_thread_only;

	//! Information that needs to be computed for drawing the corner border.

	//! Smartly cache this data, so the logic runs only when needed.
	struct cached_draw_info_s {
		bool same_border_everywhere=false;
		int all_corners=0;
		std::vector<std::tuple<const_border_impl, int>> corners;
		std::vector<std::tuple<const_border_impl, int>> stubs;
	};

	//! Currently cached draw info.
	std::optional<cached_draw_info_s> cached_draw_info;

	//! If the cached draw info is cached, return it, else compute it.

	cached_draw_info_s &get_cached_draw_info(ONLY IN_THREAD);

 public:

	THREAD_DATA_ONLY(surrounding_elements);
	THREAD_DATA_ONLY(old_surrounding_elements);

	//! The grid layout manager has updated the surrounding_elements()

	void updated(ONLY IN_THREAD);

	//! Constructor
	implObj(const container_impl &container);

 private:
	//! Internal constructor
	implObj(const container_impl &container,
		generic_windowObj::handlerObj &h);

	//! Destructor
	~implObj();

	//! Initialize this element.
	void initialize(ONLY IN_THREAD) override;

	//! The current theme has changed.
	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;

	//! Recalculate my metrics.
	void compute_metrics(ONLY IN_THREAD);

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
