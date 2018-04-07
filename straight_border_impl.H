/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef straight_border_impl_h
#define straight_border_impl_h

#include "border_implobj.H"
#include "x/w/impl/metrics_horizvertobj.H"
#include "straight_border.H"
#include "grid_elementfwd.H"
#include "screen.H"
#include "current_border_impl.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

/*!

Template for a straight_borderObj::implObj subclass.

A very scary template. horizontal_straight_borderObj and
vertical_straight_borderObj are the less-scary versions.

Implement horizontal/vertical border-specific logic as a subclass
of straight_borderObj::implObj. This is a template, whose template
parameters define which parts of the border's horizontal and vertical
properties are looked at.

Template for horizontal or vertical border element implementation uses:

  Type        | calc_major      | calc_minor       | axis_major | axis_minor
  ------------+---------------- +-------------------+------------+----------
  Horizontal  | c_border_width  | c_border_height  | horiz       | vert
  Vertical    | c_border_height | c_border_width   | vert        | horiz
*/

template<dim_t border_implObj::*calc_major,
	 dim_t border_implObj::*calc_minor,
	 metrics::axis metrics::horizvert_axi::*axis_major,
	 metrics::axis metrics::horizvert_axi::*axis_minor,

	 // Fill in elements' background color
	 void (straight_borderObj::implObj::*do_fill_bg)
	 (ONLY IN_THREAD, const border_implObj::draw_info &) const,
	 // draw_horizontal, or draw_vertical, of course.
	 void (border_implObj::*do_draw_horizvert)
				  (ONLY IN_THREAD,
				   const border_implObj::draw_info &,
				   const grid_elementptr &,
				   const grid_elementptr &) const>
class LIBCXX_HIDDEN straight_border_implObj
	: public straight_borderObj::implObj {

 public:

	using straight_borderObj::implObj::implObj;

	~straight_border_implObj()=default;

	template<typename F>
		inline void for_each_non_null_border(ONLY IN_THREAD,
						     F &&f)
	{
		const auto &b=this->borders(IN_THREAD);
		if (b.border_1)
			f(b.border_1);
		if (b.border_2)
			f(b.border_2);
		if (b.border_default)
			f(b.border_default);
	}

	//! This border element was just added to its container.

	//! Compute our metrics for the first time.

	void initialize(ONLY IN_THREAD) override
	{
		straight_borderObj::implObj::initialize(IN_THREAD);
		theme_updated(IN_THREAD,
			      get_screen()->impl->current_theme.get());

		for_each_non_null_border(IN_THREAD, [&]
					 (const auto &b)
					 {
						 b->initialize(IN_THREAD);
					 });
	}

	//! The screen's theme has been updated.

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
		override
	{
		for_each_non_null_border(IN_THREAD,
					 [&, this]
					 (const auto &b)
					 {
						 b->theme_updated(IN_THREAD,
								  new_theme);
					 });
		compute_metrics(IN_THREAD);
		straight_borderObj::implObj::theme_updated(IN_THREAD,
							   new_theme);
	}

 private:
	//! Pick the best border.

	void compute_metrics(ONLY IN_THREAD) override
	{
		horizvert_axi current_axi;

		const auto &b=this->best_border(IN_THREAD);

		// The horizontal axis, for horizontal borders,
		// can range 0 to infinite.

		current_axi.*axis_major =
			metrics::axis(0, 0, dim_t::infinite());

		if (b)
		{
			auto &b_impl=b->border(IN_THREAD);
			auto value= (*b_impl).*calc_minor;

			current_axi.*axis_minor = metrics::axis(value,
								value,
								value);
		}

		this->get_horizvert(IN_THREAD)
			->set_element_metrics(IN_THREAD,
					      current_axi.horiz,
					      current_axi.vert);
	}


 private:

	void draw_horizvert(ONLY IN_THREAD,
			    const border_implObj::draw_info &di)
		const override
	{
		((*this).*do_fill_bg)(IN_THREAD, di);

		const auto &b=this->best_border(IN_THREAD);

		if (b)
		{
			auto &b_impl=b->border(IN_THREAD);

// Straight border object already took care of filling in the background
// color.

			((*b_impl).*do_draw_horizvert)
				(IN_THREAD, di, borders(IN_THREAD).element_1,
				 borders(IN_THREAD).element_2);
		}
	}

};

//! Superclass of horizontal_straight_borderObj

typedef straight_border_implObj<
	&border_implObj::calculated_border_width,
	&border_implObj::calculated_border_height,
	&metrics::horizvert_axi::horiz,
	&metrics::horizvert_axi::vert,
	&straight_borderObj::implObj::background_horizontal,
	&border_implObj::draw_horizontal> horizontal_impl;

/*!

Less-scary looking subclass of straight_border_implObj

Instantiates the template for a horizontal border.
*/

class LIBCXX_HIDDEN horizontal_straight_borderObj : public horizontal_impl {

 public:

	//! Constructor

	//! Used by the grid layout manager to build a border element
	//! based on the grid elements next to it.

	horizontal_straight_borderObj(const ref<containerObj::implObj> &c,
				      const grid_elementptr &element_1,
				      const current_border_implptr &border1,
				      const grid_elementptr &element_2,
				      const current_border_implptr &border2,
				      const current_border_implptr &border_default);
	horizontal_straight_borderObj(const ref<containerObj::implObj> &c,
				      const current_border_implptr &border_default);

	//! Destructor
	~horizontal_straight_borderObj();
};

//! Superclass of vertical_straight_borderObj

typedef straight_border_implObj<
	&border_implObj::calculated_border_height,
	&border_implObj::calculated_border_width,
	&metrics::horizvert_axi::vert,
	&metrics::horizvert_axi::horiz,
	&straight_borderObj::implObj::background_vertical,
	&border_implObj::draw_vertical> vertical_impl;

/*!

Less-scary looking subclass of straight_border_implObj

Instantiates the template for a vertical border.
*/

class LIBCXX_HIDDEN vertical_straight_borderObj : public vertical_impl {

 public:

	//! Constructor

	//! Used by the grid layout manager to build a border element
	//! based on the grid elements above and below it.
	vertical_straight_borderObj(const ref<containerObj::implObj> &c,
				    const grid_elementptr &element_1,
				    const current_border_implptr &border1,
				    const grid_elementptr &element_2,
				    const current_border_implptr &border2,
				    const current_border_implptr &border_default);

	vertical_straight_borderObj(const ref<containerObj::implObj> &c,
				    const current_border_implptr &border_default);

	//! Destructor
	~vertical_straight_borderObj();
};

LIBCXXW_NAMESPACE_END

#endif
