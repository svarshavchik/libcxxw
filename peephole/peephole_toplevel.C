/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peephole_toplevel.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/always_visible.H"
#include "gridlayoutmanager.H"
#include "peephole/peephole.H"
#include "peephole/peephole_impl_element.H"
#include "peephole/peephole_toplevel_gridlayoutmanager.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "peephole/peepholed_toplevel_element.H"
#include "scrollbar/scrollbar_impl.H"
#include "screen.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/border_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/screen.H"
#include "x/w/scrollbar.H"

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

//! Subclass the peephole layout manager, for the top level peephole.

//! Mostly to override recalculate(), which updates the top level peephole's
//! metrics, and keeps an eye on the element's metrics.

class LIBCXX_HIDDEN toplevelpeephole_layoutmanagerObj
	: public peepholeObj::layoutmanager_implObj::scrollbarsObj {

 public:

	//! Constructor

	toplevelpeephole_layoutmanagerObj(const peephole_with_scrollbars_info
					  &info,
					  const peephole_scrollbars
					  &peephole_scrollbars,
					  // Can be ref<peepholeObj::implObj>:
					  const container_impl &peephole_impl,
					  const peepholed_toplevel
					  &element_in_peephole,
					  const current_border_implptr
					  &peephole_border);

	const peepholed_toplevel element_in_peephole;

	const current_border_implptr peephole_border;

	//! Destructor
	~toplevelpeephole_layoutmanagerObj();

	//! Override recalculate().

	//! Performs some value-added adjustments before invoking the
	//! overridden recalculate().

	void recalculate(ONLY IN_THREAD) override;
};

//! Implementation object for the top level peephole element.

//! Intercepts new_focusable() calls, so that the scrollbars always are last
//! in the tabbing order.

class LIBCXX_HIDDEN peephole_toplevel_implObj
	: public always_visibleObj<peephole_impl_elementObj<
					   container_elementObj<
						   child_elementObj>>> {

	typedef always_visibleObj<peephole_impl_elementObj<
		container_elementObj<child_elementObj>>> superclass_t;

 public:

	//! The top level peephole's horizontal scrollbar

	//! This peephole is not a parent or a child of the scrollbar,
	//! so capturing this reference is ok.
	const scrollbar horizontal_scrollbar;

	//! The top level peephole's vertical scrollbar.

	//! This peephole is not a parent or a child of the scrollbar,
	//! so capturing this reference is ok.
	const scrollbar vertical_scrollbar;

	peephole_toplevel_implObj(const container_impl &parent,
				  const scrollbar &horizontal_scrollbar,
				  const scrollbar &vertical_scrollbar,
				  const child_element_init_params &init_params)
		: superclass_t{parent, init_params},
		horizontal_scrollbar{horizontal_scrollbar},
		vertical_scrollbar{vertical_scrollbar}
		{
		}

	~peephole_toplevel_implObj()=default;

	//! Override should_preclear_entirely_exposed_element()

	bool should_preclear_entirely_exposed_element(ONLY IN_THREAD)
		override
	{
		return true;
	}

	//! Override focusable_initialized()

	void focusable_initialized(ONLY IN_THREAD,
				   focusableObj::implObj &fimpl) override
	{
		set_top_level_peephole_scrollbar_focus_order
			(IN_THREAD,
			 fimpl,
			 horizontal_scrollbar,
			 vertical_scrollbar);
	}

};

#if 0
{
#endif
}

layoutmanager
create_peephole_toplevel_impl(const container_impl &toplevel,
			      const std::optional<border_arg> &border,
			      const std::optional<color_arg>
			      &peephole_background_color,
			      const std::optional<color_arg>
			      &scrollbars_background_color,
			      const peephole_style &style,
			      const function<create_peepholed_element_t>
			      &factory)
{

	ptr<peephole_toplevel_implObj> peephole_implptr;

	const auto &[layout_impl, grid_impl, grid]=
		create_peephole_with_scrollbars
		([&]
		 (const ref<peepholeObj::layoutmanager_implObj> &layout_impl)
		 -> peephole_element_factory_ret_t
		 {
			 // Ok, we can now create the container.
			 auto peephole_container=
				 peephole::create(peephole_implptr,
						  layout_impl);

			 return {
				 peephole_container,
				 peephole_container,
				 border,
				 border,
				 {},
			 };
		 },
		 [&]
		 (const auto &info, const auto &scrollbars)
		 {
			 // First, create the "internal" implementation
			 // object for the peephole.
			 //
			 // This peephole element will be always_visible.

			 child_element_init_params init_params;

			 init_params.background_color=peephole_background_color;

			 auto peephole_impl=ref<peephole_toplevel_implObj>
				 ::create(toplevel,
					  scrollbars.horizontal_scrollbar,
					  scrollbars.vertical_scrollbar,
					  init_params);

			 peephole_implptr=peephole_impl;

			 // Now the fake top level element that we wanted
			 // to create originally, it'll be a child element
			 // of the peephole.

			 auto inner_container=factory(peephole_impl);

			 // The peephole layoutmanager needs to know what
			 // border is in place, because that needs to be
			 // factored into calculations.
			 current_border_implptr border_impl;

			 if (border)
				 border_impl=toplevel->container_element_impl()
					 .get_screen()->impl
					 ->get_cached_border(*border);



			 return ref<toplevelpeephole_layoutmanagerObj>::create
				 (info,
				  scrollbars,
				  peephole_impl,
				  inner_container,
				  border_impl);
		 },
		 [&]
		 (const peephole_gridlayoutmanagerObj::init_args &args)
		 {
			 return ref<peephole_toplevel_gridlayoutmanagerObj>
				 ::create(args);
		 },
		 {
		  toplevel,
		  scrollbars_background_color,
		  style,
		  // Opening bid: do not show the
		  // scrollbars.
		  scrollbar_visibility::never,
		  scrollbar_visibility::never,
		 });

	return grid;
}

//////////////////////////////////////////////////////////////////////////////

toplevelpeephole_layoutmanagerObj::toplevelpeephole_layoutmanagerObj
(const peephole_with_scrollbars_info &info,
 const peephole_scrollbars &peephole_scrollbars,
 const container_impl &peephole_impl,
 const peepholed_toplevel &element_in_peephole,
 const current_border_implptr &peephole_border)
	: peepholeObj::layoutmanager_implObj::scrollbarsObj
	{
	 info,
	 peephole_scrollbars,
	 peephole_impl,
	 element_in_peephole,
	},
	  element_in_peephole{element_in_peephole},
	  peephole_border{peephole_border}
{
}

toplevelpeephole_layoutmanagerObj::~toplevelpeephole_layoutmanagerObj()=default;

void toplevelpeephole_layoutmanagerObj::recalculate(ONLY IN_THREAD)
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

	metrics::axis new_horiz{h_minimum, h_preferred, h_maximum};

	// input field search popup forces specific width.
	//
	// If the peephole's width_algorithm specifies explicit metrics,
	// don't override them here, instead use them.
	if (std::holds_alternative<dim_axis_arg>(style.width_algorithm))
		new_horiz=horizontal_metrics(IN_THREAD);

	get_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      new_horiz,
				      {v_minimum, v_preferred, v_maximum});

	peepholeObj::layoutmanager_implObj::scrollbarsObj
		::recalculate(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
