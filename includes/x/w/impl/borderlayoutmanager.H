/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_borderlayoutmanager_h
#define x_w_impl_borderlayoutmanager_h

#include "x/w/borderlayoutmanager.H"
#include "x/w/impl/singletonlayoutmanager.H"
#include "x/w/impl/bordercontainer_impl.H"
#include "x/w/impl/border_implfwd.H"
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN borderlayoutmanagerObj::implObj
	: public singletonlayoutmanagerObj::implObj {

 protected:
	// Alias
	typedef singletonlayoutmanagerObj::implObj superclass_t;

 public:
	//! Our container
	const ref<bordercontainer_implObj> bordercontainer_impl;

 private:

	//! Current border being used for drawing.

	const_border_implptr current_left_border;

	//! Current border being used for drawing.

	const_border_implptr current_right_border;

	//! Current border being used for drawing.

	const_border_implptr current_top_border;

	//! Current border being used for drawing.

	const_border_implptr current_bottom_border;

 public:
	//! Constructor
	implObj(const container_impl &container_impl,
		const ref<bordercontainer_implObj> &bordercontainer_impl,
		const element &initial_element,
		halign element_halign,
		valign element_valign);

	//! Destructor
	~implObj();

	//! Override recalculated()
	void recalculate(ONLY IN_THREAD) override;

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
		     rectarea &drawn_areas) override;

 private:

	//! What get_all_borders returns.

	struct border_info {

		const_border_impl lb, //!< Left border.
			rb, //!< Right border.
			tb, //!< Top border.
			bb, //!< Bottom border.
			tl, //!< Top-left border.
			tr, //!< Top-right border.
			bl, //!< Bottom-left border.
			br; //!< Bottom-right border.

		dim_t left_pad,	        //!< Padding
			right_pad,	//!< Padding
			top_pad,	//!< Padding
			bottom_pad;	//!< Padding
	};

	//! Cached border_info
	std::optional<border_info> cached_border_info;

	//! Obtain all four borders from the container, compute four corner borders.

	const border_info &get_all_borders(ONLY IN_THREAD);

 public:
	//! Override theme_updated().

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;
};

LIBCXXW_NAMESPACE_END

#endif
