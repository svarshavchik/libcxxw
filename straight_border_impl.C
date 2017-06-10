/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "straight_border.H"
#include "generic_window_handler.H"
#include "draw_info.H"
#include "current_border_impl.H"
#include "grid_element.H"
#include "scratch_and_mask_buffer_draw.H"
#include "x/w/scratch_buffer.H"
#include "x/w/pictformat.H"

LIBCXXW_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
//
// Implement horizontal/vertical border-specific logic as a subclass
// of straight_borderObj::implObj. This is a template, whose template
// parameters define which parts of the border's horizontal and vertical
// properties are looked at.
//
// Template for horizontal or vertical border element implementation uses:
//
// Type        | calc_major      | calc_minor       | axis_major | axis_minor
// ------------+---------------- +-------------------+------------+----------
// Horizontal  | c_border_width  | c_border_height  | horiz       | vert
// Vertical    | c_border_height | c_border_width   | vert        | horiz

template<dim_t border_implObj::*calc_major,
	 dim_t border_implObj::*calc_minor,
	 metrics::axis metrics::horizvert_axi::*axis_major,
	 metrics::axis metrics::horizvert_axi::*axis_minor,

	 // Fill in elements' background color
	 void (straight_borderObj::implObj::*do_fill_bg)
	 (IN_THREAD_ONLY, const border_implObj::draw_info &) const,
	 // draw_horizontal, or draw_vertical, of course.
	 void (border_implObj::
	       *do_draw_horizvert)(const border_implObj::draw_info &,
				   const grid_elementptr &,
				   const grid_elementptr &) const>
class LIBCXX_HIDDEN straight_border_implObj
	: public straight_borderObj::implObj {

 public:

	using straight_borderObj::implObj::implObj;

	~straight_border_implObj()=default;

	//! This border element was just added to its container.

	//! Compute our metrics for the first time.

	void initialize(IN_THREAD_ONLY) override
	{
		compute_metrics(IN_THREAD);
		straight_borderObj::implObj::initialize(IN_THREAD);
	}

	//! The screen's theme has been updated.

	void theme_updated(IN_THREAD_ONLY) override
	{
		const auto &b=this->borders(IN_THREAD);

		if (!b.border_1.null())
			b.border_1->theme_updated(IN_THREAD);
		if (!b.border_2.null())
			b.border_2->theme_updated(IN_THREAD);
		if (!b.border_default.null())
			b.border_default->theme_updated(IN_THREAD);

		compute_metrics(IN_THREAD);
		straight_borderObj::implObj::theme_updated(IN_THREAD);
	}

 private:
	//! Pick the best border.

	void compute_metrics(IN_THREAD_ONLY) override
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

	void draw_horizvert(IN_THREAD_ONLY,
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
				(di, borders(IN_THREAD).element_1,
				 borders(IN_THREAD).element_2);
		}
	}

};

border_source::border_source(const grid_elementptr &e) : e(e)
{
}

current_border_implptr border_source::get_border(IN_THREAD_ONLY,
						 current_border_implptr
						 grid_elementObj::*which_one)
	const
{
	return e ? (*e).*which_one:current_border_implptr();
}

// Horizontal border implementation

typedef straight_border_implObj<
	&border_implObj::calculated_border_width,
	&border_implObj::calculated_border_height,
	&metrics::horizvert_axi::horiz,
	&metrics::horizvert_axi::vert,
	&straight_borderObj::implObj::background_horizontal,
	&border_implObj::draw_horizontal> horizontal_impl;

class LIBCXX_HIDDEN horizontal_straight_borderObj : public horizontal_impl {

 public:

	horizontal_straight_borderObj(const ref<containerObj::implObj> &c,
				      const grid_elementptr &element_1,
				      const current_border_implptr &border1,
				      const grid_elementptr &element_2,
				      const current_border_implptr &border2,
				      const current_border_implptr &border_default)
		: horizontal_impl(c,
				  metrics::horizvert_axi(),
				  "horiz-border@libcxx",
				  element_1,
				  border1,
				  element_2,
				  border2,
				  border_default)
	{
	}

	~horizontal_straight_borderObj()=default;
};

straight_border straight_borderBase
::create_horizontal_border(IN_THREAD_ONLY,
			   const ref<containerObj::implObj> &container_impl,
			   const border_source &element_above,
			   const border_source &element_below,
			   const current_border_implptr &default_border)
{
#ifdef CREATE_STRAIGHT_BORDER
	CREATE_STRAIGHT_BORDER();
#endif

	auto impl=ref<horizontal_straight_borderObj>
		::create(container_impl,

			 element_above.e,
			 element_above
			 .get_border(IN_THREAD,
				     &grid_elementObj::bottom_border),

			 element_below.e,
			 element_below
			 .get_border(IN_THREAD,
				     &grid_elementObj::top_border),

			 default_border);

	return ptrrefBase::objfactory<straight_border>::create(impl);
}

// Vertical border implementation
typedef straight_border_implObj<
	&border_implObj::calculated_border_height,
	&border_implObj::calculated_border_width,
	&metrics::horizvert_axi::vert,
	&metrics::horizvert_axi::horiz,
	&straight_borderObj::implObj::background_vertical,
	&border_implObj::draw_vertical> vertical_impl;

class LIBCXX_HIDDEN vertical_straight_borderObj : public vertical_impl {

 public:

	vertical_straight_borderObj(const ref<containerObj::implObj> &c,
				      const grid_elementptr &element_1,
				      const current_border_implptr &border1,
				      const grid_elementptr &element_2,
				      const current_border_implptr &border2,
				      const current_border_implptr &border_default)
		: vertical_impl(c,
				metrics::horizvert_axi(),
				"vert-border@libcxx",
				element_1,
				border1,
				element_2,
				border2,
				border_default)
	{
	}

	~vertical_straight_borderObj()=default;
};

straight_border straight_borderBase
::create_vertical_border(IN_THREAD_ONLY,
			 const ref<containerObj::implObj> &container_impl,
			 const border_source &element_onleft,
			 const border_source &element_onright,
			 const current_border_implptr &default_border)
{
#ifdef CREATE_STRAIGHT_BORDER
	CREATE_STRAIGHT_BORDER();
#endif
	auto impl=ref<vertical_straight_borderObj>
		::create(container_impl,

			 element_onleft.e,
			 element_onleft
			 .get_border(IN_THREAD, &grid_elementObj::right_border),

			 element_onright.e,
			 element_onright
			 .get_border(IN_THREAD, &grid_elementObj::left_border),

			 default_border);

	return ptrrefBase::objfactory<straight_border>::create(impl);
}

// We have an existing horizontal or a vertical border that lives where
// we are requesting an existence of a particular border. Check to see
// if the existing  border matches the requirements; if so return the
// existing border, otherwise create a new one.

straight_border straight_borderBase
::update_horizontal_border(IN_THREAD_ONLY,
			   const straight_border &existing_border,
			   const border_source &element_above,
			   const border_source &element_below,
			   const current_border_implptr &default_border)
{
	const auto &b=existing_border->impl->borders(IN_THREAD);

	if (b.element_1 == element_above.e &&
	    b.element_2 == element_below.e &&
	    b.border_default == default_border)
	{
		// We also want to compare the actual borders, they must be
		// the same.
		if (element_above.get_border(IN_THREAD,
					     &grid_elementObj::bottom_border)
		    == b.border_1 &&
		    element_below.get_border(IN_THREAD,
					     &grid_elementObj::top_border)
		    == b.border_2)
			return existing_border;
	}
	return create_horizontal_border(IN_THREAD,
					existing_border->impl->container,
					element_above,
					element_below,
					default_border);
}

straight_border straight_borderBase
::update_vertical_border(IN_THREAD_ONLY,
			 const straight_border &existing_border,
			 const border_source &element_onleft,
			 const border_source &element_onright,
			 const current_border_implptr &default_border)
{
	const auto &b=existing_border->impl->borders(IN_THREAD);

	if (b.element_1 == element_onleft.e &&
	    b.element_2 == element_onright.e &&
	    b.border_default == default_border)
	{
		// We also want to compare the actual borders, they must be
		// the same.
		if (element_onleft.get_border(IN_THREAD,
					      &grid_elementObj::right_border)
		    == b.border_1
		    &&
		    element_onright.get_border(IN_THREAD,
					       &grid_elementObj::left_border)
		    == b.border_2)
		return existing_border;
	}
	return create_vertical_border(IN_THREAD,
				      existing_border->impl->container,
				      element_onleft,
				      element_onright,
				      default_border);
}

straight_borderObj::implObj
::implObj(const ref<containerObj::implObj> &container,
	  const metrics::horizvert_axi &initial_metrics,
	  const char *scratch_buffer_label,
	  const grid_elementptr &element_1,
	  const current_border_implptr &border1,
	  const grid_elementptr &element_2,
	  const current_border_implptr &border2,
	  const current_border_implptr &border_default)
	: implObj(container, initial_metrics,
		  scratch_buffer_label,
		  container->get_window_handler(),
		  element_1, border1, element_2, border2, border_default)
{
}

straight_borderObj::implObj
::implObj(const ref<containerObj::implObj> &container,
	  const metrics::horizvert_axi &initial_metrics,
	  const char *scratch_buffer_label,
	  generic_windowObj::handlerObj &h,
	  const grid_elementptr &element_1,
	  const current_border_implptr &border1,
	  const grid_elementptr &element_2,
	  const current_border_implptr &border2,
	  const current_border_implptr &border_default)
	: scratch_and_mask_buffer_draw<child_elementObj>
	(std::string("mask-")+scratch_buffer_label,
	 h.get_width()/10+1,
	 h.get_height()/10+1, container,
	 child_element_init_params{std::string("area-")+scratch_buffer_label,
			initial_metrics}),
	borders_thread_only{element_1, border1, element_2, border2,
		border_default}
{
}

straight_borderObj::implObj::~implObj()=default;

const current_border_implptr &straight_borderObj::implObj
::best_border(IN_THREAD_ONLY) const
{
	const auto &b=borders(IN_THREAD);

	if (b.border_1 && b.border_2)
	{
		return b.border_1->border(IN_THREAD)
			->compare(*b.border_2->border(IN_THREAD))
			? b.border_2:b.border_1;
	}

	if (b.border_1)
		return b.border_1;
	if (b.border_2)
		return b.border_2;

	return b.border_default;
}

void straight_borderObj::implObj::do_draw(IN_THREAD_ONLY,
					  const draw_info &di,
					  const picture &area_picture,
					  const pixmap &area_pixmap,
					  const gc &area_gc,
					  const picture &mask_picture,
					  const pixmap &mask_pixmap,
					  const gc &mask_gc,
					  const rectangle &area_entire_rect)
{
	border_implObj::draw_info
		border_draw_info{area_picture,
			area_entire_rect,
			area_pixmap,
			mask_picture,
			mask_pixmap,
			mask_gc,
			di.absolute_location.x,
			di.absolute_location.y};

	draw_horizvert(IN_THREAD, border_draw_info);
}

void straight_borderObj::implObj
::background_horizontal(IN_THREAD_ONLY,
			const border_implObj::draw_info &bg) const
{
	dim_t top_height=bg.area_rectangle.height/2;
	dim_t bottom_height=bg.area_rectangle.height - top_height;

	if (borders(IN_THREAD).element_1 && top_height > 0)
	{
		auto &di=borders(IN_THREAD).element_1->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=di.background_xy_to(bg.area_x, bg.area_y);

		bg.area_picture->impl->composite(di.window_background,
						 xy.first, xy.second,
						 0, 0,
						 bg.area_rectangle.width,
						 top_height);
	}

	if (borders(IN_THREAD).element_2 && top_height > 0)
	{
		auto &di=borders(IN_THREAD).element_2->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=di.background_xy_to(bg.area_x, bg.area_y,
					    0,
					    dim_t::value_type(top_height));

		bg.area_picture->impl->composite(di.window_background,
						 xy.first, xy.second,
						 0,
						 coord_t::truncate(top_height),
						 bg.area_rectangle.width,
						 bottom_height);
	}
}

void straight_borderObj::implObj
::background_vertical(IN_THREAD_ONLY,
		      const border_implObj::draw_info &bg) const
{
	dim_t left_width=bg.area_rectangle.width/2;
	dim_t right_width=bg.area_rectangle.width - left_width;

	if (borders(IN_THREAD).element_1 && left_width > 0)
	{
		auto &di=borders(IN_THREAD).element_1->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=di.background_xy_to(bg.area_x, bg.area_y);

		bg.area_picture->impl->composite(di.window_background,
						 xy.first, xy.second,
						 0, 0,
						 left_width,
						 bg.area_rectangle.height);
	}

	if (borders(IN_THREAD).element_2 && left_width > 0)
	{
		auto &di=borders(IN_THREAD).element_2->grid_element->impl
			->get_draw_info(IN_THREAD);

		auto xy=di.background_xy_to(bg.area_x, bg.area_y,
					    dim_t::value_type(left_width),
					    0);

		bg.area_picture->impl->composite(di.window_background,
						 xy.first, xy.second,
						 coord_t::truncate(left_width),
						 0,
						 right_width,
						 bg.area_rectangle.height);
	}
}

LIBCXXW_NAMESPACE_END
