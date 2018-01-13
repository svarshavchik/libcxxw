/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peephole_toplevel.H"
#include "child_element.H"
#include "always_visible.H"
#include "gridlayoutmanager.H"
#include "peephole/peephole.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_toplevel_gridlayoutmanager.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "peephole/peepholed_toplevel_element.H"
#include "scrollbar/scrollbar_impl.H"
#include "current_border_impl.H"
#include "border_impl.H"
#include "x/w/screen.H"
#include "x/w/scrollbar.H"
#include "element_screen.H"

LIBCXXW_NAMESPACE_START

//! Subclass the peephole layout manager, for the top level peephole.

//! Mostly to override recalculate(), which updates the top level peephole's
//! metrics, and keeps an eye on the element's metrics.

class LIBCXX_HIDDEN toplevelpeephole_layoutmanagerObj
	: public peepholeObj::layoutmanager_implObj::scrollbarsObj {

 public:

	//! Constructor
	toplevelpeephole_layoutmanagerObj
		(const ref<containerObj::implObj> &container_impl,
		 peephole_style style,
		 const peepholed_toplevel &element_in_peephole,
		 const current_border_implptr &peephole_border,
		 const peephole_scrollbars &scrollbars);

	const peepholed_toplevel element_in_peephole;

	const current_border_implptr peephole_border;

	//! Destructor
	~toplevelpeephole_layoutmanagerObj();

	//! Override recalculate().

	//! Performs some value-added adjustments before invoking the
	//! overridden recalculate().

	void recalculate(IN_THREAD_ONLY) override;
};

//! Implementation object for the top level peephole element.

//! Intercepts new_focusable() calls, so that the scrollbars always are last
//! in the tabbing order.

class LIBCXX_HIDDEN peephole_toplevel_implObj
	: public always_visibleObj<peepholeObj::implObj> {

	typedef always_visibleObj<peepholeObj::implObj> superclass_t;

 public:

	//! The top level peephole's horizontal scrollbar

	//! This peephole is not a parent or a child of the scrollbar,
	//! so capturing this reference is ok.
	const scrollbar horizontal_scrollbar;

	//! The top level peephole's vertical scrollbar.

	//! This peephole is not a parent or a child of the scrollbar,
	//! so capturing this reference is ok.
	const scrollbar vertical_scrollbar;

	peephole_toplevel_implObj(const ref<containerObj::implObj> &parent,
				  const scrollbar &horizontal_scrollbar,
				  const scrollbar &vertical_scrollbar)
		: superclass_t(parent),
		horizontal_scrollbar(horizontal_scrollbar),
		vertical_scrollbar(vertical_scrollbar)
		{
		}

	~peephole_toplevel_implObj()=default;

	//! Override focusable_initialized()

	void focusable_initialized(IN_THREAD_ONLY,
				   focusableImplObj &fimpl) override
	{
		set_top_level_peephole_scrollbar_focus_order
			(IN_THREAD,
			 fimpl,
			 horizontal_scrollbar,
			 vertical_scrollbar);
	}

};

layoutmanager
create_peephole_toplevel_impl(const ref<containerObj::implObj> &toplevel,
			      const std::optional<border_arg> &border,
			      peephole_style style,
			      const function<create_peepholed_element_t>
			      &factory)
{
	// Never mind what layout_factory is. The main window uses the
	// grid layout manager, this is my final word.
	auto toplevel_impl=
		peephole_toplevel_gridlayoutmanager::create(toplevel);

	auto toplevel_grid=toplevel_impl->create_gridlayoutmanager();

	// Create the scrollbars, they'll also be child elements
	// of the toplevel_grid.

	auto scrollbars=create_peephole_scrollbars(toplevel_grid->impl
						   ->container_impl);

	// The toplevel_grid will have a peephole as its child element,
	// and the scrollbars, but we'll get around to them later.
	//
	// First, create the "internal" implementation object for the peephole.
	//
	// This peephole element will be always_visible.

	auto peephole_impl=
		ref<peephole_toplevel_implObj>::create(toplevel,
						       scrollbars.horizontal_scrollbar,
						       scrollbars.vertical_scrollbar);

	// Now the fake top level element that we wanted to create originally,
	// it'll be a child element of the peephole.

	auto inner_container=factory(peephole_impl);

	// Install everything into the toplevel_grid.

	auto row0_factory=toplevel_grid->append_row();
	row0_factory->padding(0);
	if (border)
		row0_factory->border(border.value());

	// The peephole layoutmanager needs to know what border is in place,
	// because that needs to be factored into calculations.
	current_border_implptr border_impl=
		gridfactoryObj::implObj::new_grid_element_t::lock{
		row0_factory->impl->new_grid_element}->left_border;

	// Create the peephole layoutmanager...

	auto peephole_layoutmanager=
		ref<toplevelpeephole_layoutmanagerObj>::create
		(peephole_impl,
		 style,
		 inner_container,
		 border_impl,
		 scrollbars);

	peephole_layoutmanager->initialize_scrollbars();

	// ... and the element, to go with the peephole implementation object
	// and the layout manager.
	auto peephole_element=peephole::create(peephole_impl,
					       peephole_layoutmanager);

	row0_factory->created_internally(peephole_element);

	auto row1_factory=toplevel_grid->append_row();

	if (border)
	{
		row0_factory->top_border(border.value());
		row0_factory->right_border(border.value());
		row0_factory->bottom_border(border.value());
		row1_factory->left_border(border.value());
		row1_factory->bottom_border(border.value());
		row1_factory->right_border(border.value());
	}

	install_peephole_scrollbars(toplevel_grid,
				    scrollbars.vertical_scrollbar,
				    scrollbar_visibility::never,
				    row0_factory,
				    scrollbars.horizontal_scrollbar,
				    scrollbar_visibility::never,
				    row1_factory);

	// Final misc details.
	set_peephole_scrollbar_focus_order(scrollbars.horizontal_scrollbar,
					   scrollbars.vertical_scrollbar);


	return toplevel_grid;
}

//////////////////////////////////////////////////////////////////////////////

toplevelpeephole_layoutmanagerObj::toplevelpeephole_layoutmanagerObj
(const ref<containerObj::implObj> &container_impl,
 peephole_style style,
 const peepholed_toplevel &element_in_peephole,
 const current_border_implptr &peephole_border,
 const peephole_scrollbars &scrollbars)
	: peepholeObj::layoutmanager_implObj
	::scrollbarsObj(container_impl,
			style,
			element_in_peephole,
			scrollbars,

			// Opening bid: do not show the
			// scrollbars.
			scrollbar_visibility::never,
			scrollbar_visibility::never),
	element_in_peephole(element_in_peephole),
	peephole_border(peephole_border)
{
}

toplevelpeephole_layoutmanagerObj::~toplevelpeephole_layoutmanagerObj()=default;

void toplevelpeephole_layoutmanagerObj::recalculate(IN_THREAD_ONLY)
{
	// How big the element in the peephole wants to be.
	auto peepholed_metrics=element_in_peephole->get_peepholed_element()
		->impl->get_horizvert(IN_THREAD);

	element_in_peephole
		->recalculate_peepholed_metrics(IN_THREAD,
						get_element_impl()
						.get_screen());

	// Maximum size of the peephole.
	auto max_width=element_in_peephole->max_width(IN_THREAD);
	auto max_height=element_in_peephole->max_height(IN_THREAD);

	dim_t border_width=0;
	dim_t border_height=0;

	if (peephole_border)
	{
		auto b=peephole_border->border(IN_THREAD);

		border_width=b->calculated_border_width;
		border_height=b->calculated_border_height;
	}

	border_width=dim_t::truncate(border_width+border_width);
	border_height=dim_t::truncate(border_height+border_height);

	// Reduce max_width by our border's overhead.

	max_width = border_width < max_width ? max_width-border_width:1;
	max_height = border_height < max_height ? max_height-border_height:1;

	// Scrollbars' overhead.
	dim_t vertical_scrollbar_width=
		 dim_t::truncate(vertical_scrollbar_element
				 ->get_horizvert(IN_THREAD)
				 ->horiz.minimum()+border_width);

	dim_t horizontal_scrollbar_height=
		dim_t::truncate(horizontal_scrollbar_element
				->get_horizvert(IN_THREAD)
				->vert.minimum()+border_height);

	// Now, compute max_width/height less then scrollbars' overhead.

	auto max_width_with_scrollbars=
		max_width > vertical_scrollbar_width
		? max_width-vertical_scrollbar_width:0;

	auto max_height_with_scrollbars=
		max_height > horizontal_scrollbar_height
		? max_height-horizontal_scrollbar_height:0;

	// Rather than having the peephole control the visibility of the
	// scrollbar we'll manage this ourselves, here. When the height of
	// the element in the peephole exceeds what we believe should be
	// its maximum height, we'll show the scrollbar, otherwise we'll
	// hide it. Ditto for the width.

	if (peepholed_metrics->vert.minimum() > max_height)
	{
		vertical_scrollbar_visibility(IN_THREAD)=
			scrollbar_visibility::always;

		// Adjust max_width by the width of the visible scrollbar.
		max_width=max_width_with_scrollbars;
	}
	else
	{
		vertical_scrollbar_visibility(IN_THREAD)=
			scrollbar_visibility::never;
	}

	if (peepholed_metrics->horiz.minimum() > max_width)
	{
		horizontal_scrollbar_visibility(IN_THREAD)=
			scrollbar_visibility::always;

		max_height=max_height_with_scrollbars;

		// Need to re-check this.
		if (peepholed_metrics->vert.minimum() > max_height)
		{
			vertical_scrollbar_visibility(IN_THREAD)=
				scrollbar_visibility::always;

			max_width=max_width_with_scrollbars;
		}
	}
	else
	{
		horizontal_scrollbar_visibility(IN_THREAD)=
			scrollbar_visibility::never;
	}

	// Now set the peephole's metrics. Set them to match the peepholed
	// element's metrics, if possible, but do not exceed max_width and
	// max_height. So the peephole's size will never exceed the
	// computed maximum width/height that can be accomodated by the
	// screen's workarea.

	auto h_minimum=peepholed_metrics->horiz.minimum();
	auto h_preferred=peepholed_metrics->horiz.preferred();
	auto h_maximum=peepholed_metrics->horiz.maximum();

	auto v_minimum=peepholed_metrics->vert.minimum();
	auto v_preferred=peepholed_metrics->vert.preferred();
	auto v_maximum=peepholed_metrics->vert.maximum();

	if (h_maximum > max_width)
		h_maximum=max_width;

	if (h_preferred > max_width)
		h_preferred=max_width;

	if (h_minimum > max_width)
		h_minimum=max_width;

	if (v_maximum > max_height)
		v_maximum=max_height;

	if (v_preferred > max_height)
		v_preferred=max_height;

	if (v_minimum > max_height)
		v_minimum=max_height;

	get_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      {h_minimum, h_preferred, h_maximum},
				      {v_minimum, v_preferred, v_maximum});

	peepholeObj::layoutmanager_implObj::scrollbarsObj
		::recalculate(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
