/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/canvas.H"
#include "x/w/gridfactory.H"
#include "x/w/button_event.H"
#include "x/w/impl/focus/focusable.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "scrollbar/scrollbar_impl.H"
#include "x/w/impl/container.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

typedef peepholelayoutmanagerObj::implObj::scrollbarsObj scrollbarsObj;

//////////////////////////////////////////////////////////////////////////////
//
// An intermediate object used by scrollbars' update callback.
//
// When the peephole scrollbar gets dragged, the scrollbar's update
// callback invokes update_value().
//
// The constructor stores either update_horizontal_scrollbar() or
// update_vertical_scroll() in update_func, so the callback value gets passed
// there.
//
// This is constructed before the layout manager, and the link to the layout
// manager must be a weak reference, to avoid circular references.

class LIBCXX_HIDDEN scrollbarsObj::callbackObj
	: virtual public obj {

public:
	typedef void (scrollbarsObj::*update_func_t)(ONLY IN_THREAD, dim_t);

	// update_horizontal_scroll() or update_vertical_scroll()
	const update_func_t update_func;

	// Need to find my layout manager.
	typedef mpobj<weakptr<ptr<scrollbarsObj>>> my_layoutmanager_t;

	my_layoutmanager_t my_layoutmanager;

	// Constructor
	callbackObj(update_func_t update_func)
		: update_func(update_func)
	{
	}

	inline auto get_layoutmanager()
	{
		my_layoutmanager_t::lock lock{my_layoutmanager};

		return lock->getptr();
	}

	// Scrollbar has been dragged.

	inline void updated_value(ONLY IN_THREAD,
				  const scrollbar_info_t &config)
	{
		auto p=get_layoutmanager();

		if (!p)
			return;

		auto v=dim_t::truncate(config.dragged_value);

		((*p).*update_func)(IN_THREAD, v);
	}
};

// Create the scrollbars for a new peephole.

static inline peephole_scrollbars
create_peephole_scrollbars(const container_impl &container,
			   const std::optional<color_arg> &background_color,
			   const const_scrollbar_appearance &happearance,
			   const const_scrollbar_appearance &vappearance)
{

	auto horizontal_impl=
		ref<scrollbarsObj::callbackObj>
		::create(&scrollbarsObj
			 ::update_horizontal_scroll);

	auto vertical_impl=
		ref<scrollbarsObj::callbackObj>
		::create(&scrollbarsObj
			 ::update_vertical_scroll);

	scrollbar_config sc;

	auto horizontal=do_create_h_scrollbar
		(container, background_color, sc,
		 happearance,
		 [=]
		 (ONLY IN_THREAD, const auto &config)
		 {
			 horizontal_impl->updated_value(IN_THREAD, config);
		 });

	auto vertical=do_create_v_scrollbar
		(container, background_color, sc,
		 vappearance,
		 [=]
		 (ONLY IN_THREAD, const auto &config)
		 {
			 vertical_impl->updated_value(IN_THREAD, config);
		 });

	return {horizontal, vertical, horizontal_impl, vertical_impl};
}

// Install the peephole scrollbars.

static inline
void install_peephole_scrollbars(const gridlayoutmanager &lm,
				 const scrollbar &vertical_scrollbar,
				 scrollbar_visibility v_visibility,
				 const gridfactory &row0_factory,
				 const scrollbar &horizontal_scrollbar,
				 scrollbar_visibility h_visibility,
				 const gridfactory &row1_factory)
{
	// Take this opportunity to set the peephole itself, in (0, 0)
	// to absorb any additional space given to the peephole.
	//
	// combo-box's popup peephole is stretched to make sure its width
	// matches the combo-box's element's width, so this gets attribute
	// to the peephole.
	lm->requested_col_width(0, 100);
	lm->requested_row_height(0, 100);

	// Install the scrollbars, and have the grid layout manager not
	// include them in the grid, when they are not visible.
	row0_factory->remove_when_hidden(v_visibility !=
					 scrollbar_visibility
					 ::automatic_reserved)
		.padding(0).created_internally(vertical_scrollbar);

	row1_factory->remove_when_hidden(h_visibility !=
					 scrollbar_visibility
					 ::automatic_reserved)
		.padding(0).created_internally(horizontal_scrollbar);

	// Create a canvas to fill in the unused bottom-right corner.
	row1_factory->padding(0).create_canvas({std::nullopt, {0}, {0}});
}

//! Set scrollbar focus order.

//! If the peephole is not for a focusable element, the horizontal scrollbar
//! gets focus after the vertical one, that's it.

static inline
void set_peephole_scrollbar_focus_order(const focusable &horizontal_scrollbar,
					const focusable &vertical_scrollbar)
{
	horizontal_scrollbar->get_focus_after(vertical_scrollbar);
}

//! Set correct focus order for a focusable element in a peephole.

//! After everything gets constructed, we'll arrange for the vertical
//! scrollbar to get focus after the focusable element, and the horizontal
//! scrollbar after the vertical scrollbar.

static inline
void set_peephole_scrollbar_focus_order(const focusable &element,
					const focusable &horizontal_scrollbar,
					const focusable &vertical_scrollbar)
{
	// Note that the resulting order corresponds to the
	// order specified in peepholed_focusableObj::get_impl().

	vertical_scrollbar->get_focus_after(element);

	set_peephole_scrollbar_focus_order(horizontal_scrollbar,
					   vertical_scrollbar);
}

void set_top_level_peephole_scrollbar_focus_order
(ONLY IN_THREAD,
 focusableObj::implObj &new_element,
 const focusable &horizontal_scrollbar,
 const focusable &vertical_scrollbar)
{
	get_focus_impl_after_in_thread(IN_THREAD, vertical_scrollbar,
				       ref(&new_element));
	get_focus_after_in_thread(IN_THREAD, horizontal_scrollbar,
				  vertical_scrollbar);
}

peephole_scrollbars
::peephole_scrollbars(const scrollbar &horizontal_scrollbar,
		      const scrollbar &vertical_scrollbar,
		      const ref<scrollbarsObj
		      ::callbackObj> &h_callback,
		      const ref<scrollbarsObj
		      ::callbackObj> &v_callback)
	: horizontal_scrollbar(horizontal_scrollbar),
	  vertical_scrollbar(vertical_scrollbar),
	  h_callback(h_callback),
	  v_callback(v_callback)
{
}

peephole_scrollbars::peephole_scrollbars(const peephole_scrollbars &)=default;

peephole_scrollbars::peephole_scrollbars(peephole_scrollbars &&)=default;

peephole_scrollbars::~peephole_scrollbars()=default;

//////////////////////////////////////////////////////////////////////////////

scrollbarsObj::scrollbarsObj(const peephole_with_scrollbars_info &info,
			     const peephole_scrollbars &scrollbars,
			     const container_impl &peephole_impl,
			     const peepholed &element_in_peephole)
	: peepholelayoutmanagerObj::implObj{peephole_impl, info.style,
					    element_in_peephole},
	  h_scrollbar{scrollbars.horizontal_scrollbar},
	  v_scrollbar{scrollbars.vertical_scrollbar},
	  horizontal_scrollbar_visibility_thread_only
	{info.horizontal_visibility},
	  vertical_scrollbar_visibility_thread_only{info.vertical_visibility},
	  h_callback{scrollbars.h_callback},
	  v_callback{scrollbars.v_callback},
	  horizontal_scrollbar_element{scrollbars
				       .horizontal_scrollbar->elementObj::impl},
	  vertical_scrollbar_element{scrollbars
				     .vertical_scrollbar->elementObj::impl}
{
}

void scrollbarsObj::initialize_scrollbars()
{
	auto me=ref{this};

	h_callback->my_layoutmanager=me;
	v_callback->my_layoutmanager=me;
}

void scrollbarsObj::vert_scroll_low(ONLY IN_THREAD,
				    const input_mask &m)
{
	v_scrollbar->impl->to_low(IN_THREAD, m);
}

void scrollbarsObj::vert_scroll_high(ONLY IN_THREAD,
				     const input_mask &m)
{
	v_scrollbar->impl->to_high(IN_THREAD, m);
}

scrollbarsObj::~scrollbarsObj()=default;


void scrollbarsObj::do_for_each_child(ONLY IN_THREAD,
				      const function<void (const element &e)>
				      &callback)
{
	superclass_t::do_for_each_child(IN_THREAD, callback);

	callback(h_scrollbar);
	callback(v_scrollbar);
}

bool scrollbarsObj
::process_button_event(ONLY IN_THREAD,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	if (be.button != 4 && be.button != 5)
		return false;

	if (!layout_container_impl->container_element_impl().activate_for(be))
		return true;

	if (be.button == 4)
		vert_scroll_low(IN_THREAD, be);
	else
		vert_scroll_high(IN_THREAD, be);
	return true;
}

void scrollbarsObj::update_scrollbars(ONLY IN_THREAD,
				      const rectangle &element_pos,
				      const rectangle &current_position)
{
	horizontal_scrollbar_visible=
		update_scrollbar(IN_THREAD,
				 h_scrollbar->impl,
				 horizontal_scrollbar_element,
				 horizontal_scrollbar_visibility(IN_THREAD),
				 element_pos.x, element_pos.width,
				 current_position.width,
				 element_in_peephole
				 ->horizontal_increment(IN_THREAD));

	vertical_scrollbar_visible=
		update_scrollbar(IN_THREAD,
				 v_scrollbar->impl,
				 vertical_scrollbar_element,
				 vertical_scrollbar_visibility(IN_THREAD),
				 element_pos.y, element_pos.height,
				 current_position.height,
				 element_in_peephole
				 ->vertical_increment(IN_THREAD));
}

bool scrollbarsObj
::update_scrollbar(ONLY IN_THREAD,
		   const ref<scrollbarObj::implObj> &scrollbar,
		   const element_impl &visibility_element,
		   const scrollbar_visibility visibility,
		   coord_t pos,
		   dim_t size,
		   dim_t peephole_size,
		   dim_t increment)
{
	scrollbar_config new_config;

	new_config.minimum_size=scrollbar->state(IN_THREAD).minimum_size;

	new_config.range=scroll_v_t::truncate(size);
	new_config.page_size=scroll_v_t::truncate(peephole_size);
	new_config.value=scroll_v_t::truncate(-pos);
	new_config.increment=scroll_v_t::truncate(increment);

	if (new_config.page_size >= new_config.range)
	{
		// Easier for reconfigure() to optimize itself away.
		new_config.range=new_config.page_size;
		new_config.value=0;
	}

	scrollbar->reconfigure(IN_THREAD, new_config);

	// Even when we're not visible we must still religiously do the above
	// and reconfigure(), so that the scrollbar configuration reflects
	// reality. Scroll wheel-initiated scrolling (pointer button 4/5)
	// may initiate scrolling even when this vertical scrollbar is not
	// visible, and if it's not visible because there's nothing to scroll,
	// the scrollbar metrics should reflect that.

	// For never and always visibility we can update the scrollbar's
	// visibility immediately. Ditto for automatic_reserved. However
	// defer visibility update after we process_finalized_position
	// if the visibility is automatic. This gives the parent container's
	// an opportunity to resize the peephole, nullifying the need to
	// update the visibility in the first place.

	auto should_be_visible=new_config.range > new_config.page_size;

	// Making the scrollbar visible if it should_be_visible affects
	// horizontal and vertical. This results in undesirable visual
	// jitter.
	//
	// This is because updated metrics roll uphill, all the way out
	// to the top level window, and they get included as updated
	// window manager hints. But reposition gets deferred while
	// top level window is_resizing.
	//
	// The top level window gets resized to accomodate the scrollbar's
	// contribution to preferred metrics, then our container discovers
	// that it can resize itself for the combined size of the peephole
	// and the scrollbar. Once the peephole gets resized we discover
	// that we don't need the scrollbar visible any more, and everything
	// gets resized back, again.
	//
	// Defer scrollbar's request_visibility until we
	// process_finalized_position, if the visibility is automatic. For
	// all other visibility policies we can set or update them
	// immediately without deferring anything. request_visibility()
	// is a no-op if it doesn't actually change the widget's existing
	// visibility.

	switch (visibility) {
	case scrollbar_visibility::never:
		visibility_element->request_visibility(IN_THREAD, false);
		return false;
	case scrollbar_visibility::always:
		visibility_element->request_visibility(IN_THREAD, true);
		return true;
	case scrollbar_visibility::automatic:
		layout_container_impl->container_element_impl()
			.schedule_finalized_position(IN_THREAD);
		break;
	case scrollbar_visibility::automatic_reserved:
		visibility_element->request_visibility(IN_THREAD,
						       should_be_visible);
		break;
	}

	return should_be_visible;
}

void scrollbarsObj::process_finalized_position(ONLY IN_THREAD)
{
	superclass_t::process_finalized_position(IN_THREAD);

	horizontal_scrollbar_element->request_visibility
		(IN_THREAD, horizontal_scrollbar_visible);
	vertical_scrollbar_element->request_visibility
		(IN_THREAD, vertical_scrollbar_visible);
}

///////////////////////////////////////////////////////////////////////////

create_peephole_with_scrollbars_ret_t do_create_peephole_with_scrollbars
(const function<peephole_with_scrollbars_layoutmanager_factory> &lm_factory,
 const function<peephole_element_factory> &pe_factory,
 const function<peephole_with_scrollbars_gridlayoutmanager_factory> &glm_factory,
 const peephole_with_scrollbars_info &info)
{
	auto scrollbars=
		create_peephole_scrollbars(info.grid_container_impl,
					   info.background_color,
					   info.horizontal_appearance,
					   info.vertical_appearance);

	auto layout_impl=lm_factory(info, scrollbars);

	layout_impl->initialize_scrollbars();

	const auto &[peephole_container,
		     grid_peephole_element,
		     grid_peephole_element_border,
		     grid_scrollbar_border,
		     focusable_peephole_element,
		     peephole_left_padding,
		     peephole_right_padding,
		     peephole_top_padding,
		     peephole_bottom_padding]=
		pe_factory(layout_impl);

	// Make sure the tabbing order is right.

	if (focusable_peephole_element)
		set_peephole_scrollbar_focus_order
			(focusable_peephole_element,
			 scrollbars.horizontal_scrollbar,
			 scrollbars.vertical_scrollbar);
	else
		set_peephole_scrollbar_focus_order
			(scrollbars.horizontal_scrollbar,
			 scrollbars.vertical_scrollbar);

	auto grid_impl=glm_factory({info.grid_container_impl,
				    peephole_container,
				    scrollbars.vertical_scrollbar,
				    scrollbars.horizontal_scrollbar
		});

	auto grid=grid_impl->create_gridlayoutmanager();

	auto factory=grid->append_row();

	factory->left_padding(peephole_left_padding);
	factory->right_padding(peephole_right_padding);
	factory->top_padding(peephole_top_padding);
	factory->bottom_padding(peephole_bottom_padding);

	if (grid_peephole_element_border)
		factory->border(*grid_peephole_element_border);
	factory->created_internally(grid_peephole_element);

	auto factory2=grid->append_row();

	if (grid_scrollbar_border)
	{
		factory->top_border(*grid_scrollbar_border);
		factory->right_border(*grid_scrollbar_border);
		factory->bottom_border(*grid_scrollbar_border);
		factory2->left_border(*grid_scrollbar_border);
		factory2->bottom_border(*grid_scrollbar_border);
		factory2->right_border(*grid_scrollbar_border);
	}

	install_peephole_scrollbars(grid,
				    scrollbars.vertical_scrollbar,
				    info.vertical_visibility,
				    factory,
				    scrollbars.horizontal_scrollbar,
				    info.horizontal_visibility,
				    factory2);

	return {layout_impl, grid_impl, grid};
}

ref<peephole_gridlayoutmanagerObj>
create_peephole_gridlayoutmanager(const peephole_gridlayoutmanagerObj
				  ::init_args &init_args)
{
	return ref<peephole_gridlayoutmanagerObj>::create(init_args);
}

LIBCXXW_NAMESPACE_END
