/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/canvas.H"
#include "x/w/gridfactory.H"
#include "x/w/button_event.H"
#include "focus/focusable.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "scrollbar/scrollbar_impl.H"
#include "x/w/impl/container.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

typedef peepholeObj::layoutmanager_implObj::scrollbarsObj scrollbarsObj;

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
	//
	// TODO: this always gets invoked in the connection thread, as such
	// run_as() is not really needed.

	inline void updated_value(const auto &config)
	{
		auto p=get_layoutmanager();

		if (!p)
			return;

		p->get_element_impl().THREAD
			->run_as([p, update_func=this->update_func,
				  v=dim_t::truncate(config.dragged_value)]
				 (ONLY IN_THREAD)
				 {
					 ((*p).*update_func)(IN_THREAD, v);
				 });
	}
};

peephole_scrollbars
create_peephole_scrollbars(const ref<containerObj::implObj> &container,
			   const std::optional<color_arg> &background_color)
{

	auto horizontal_impl=
		ref<scrollbarsObj::callbackObj>
		::create(&scrollbarsObj
			 ::update_horizontal_scroll);

	auto vertical_impl=
		ref<scrollbarsObj::callbackObj>
		::create(&scrollbarsObj
			 ::update_vertical_scroll);

	auto horizontal=do_create_h_scrollbar
		(container, background_color, scrollbar_config{},
		 0,
		 [=]
		 (THREAD_CALLBACK, const auto &config)
		 {
			 horizontal_impl->updated_value(config);
		 });

	auto vertical=do_create_v_scrollbar
		(container, background_color, scrollbar_config{},
		 0,
		 [=]
		 (THREAD_CALLBACK, const auto &config)
		 {
			 vertical_impl->updated_value(config);
		 });

	return {horizontal, vertical, horizontal_impl, vertical_impl};
}

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
	row1_factory->padding(0).create_canvas([]
					       (const auto &ignore)
					       {
					       },
					       {0, 0, 0},
					       {0, 0, 0});
}

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

void set_peephole_scrollbar_focus_order(const focusable &horizontal_scrollbar,
					const focusable &vertical_scrollbar)
{
	horizontal_scrollbar->get_focus_after(vertical_scrollbar);
}

void set_top_level_peephole_scrollbar_focus_order
(ONLY IN_THREAD,
 focusableImplObj &new_element,
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

scrollbarsObj
::scrollbarsObj(const ref<containerObj::implObj> &container_impl,
			peephole_style style,
			const peepholed &element_in_peephole,

			const peephole_scrollbars &scrollbars,
			const scrollbar_visibility horizontal_scrollbar_visibility,
			const scrollbar_visibility vertical_scrollbar_visibility)
	: layoutmanager_implObj(container_impl, style, element_in_peephole),
	h_scrollbar(scrollbars.horizontal_scrollbar),
	v_scrollbar(scrollbars.vertical_scrollbar),
	horizontal_scrollbar_visibility_thread_only(horizontal_scrollbar_visibility),
	vertical_scrollbar_visibility_thread_only(vertical_scrollbar_visibility),
	h_callback(scrollbars.h_callback),
	v_callback(scrollbars.v_callback),
	horizontal_scrollbar_element(scrollbars.horizontal_scrollbar->elementObj::impl),
	vertical_scrollbar_element(scrollbars.vertical_scrollbar->elementObj::impl)
{
}

void scrollbarsObj::initialize_scrollbars()
{
	auto me=ref(this);

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


bool scrollbarsObj
::process_button_event(ONLY IN_THREAD,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	if (be.button != 4 && be.button != 5)
		return false;

	if (!container_impl->container_element_impl().activate_for(be))
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
	update_scrollbar(IN_THREAD,
			 h_scrollbar->impl,
			 horizontal_scrollbar_element,
			 horizontal_scrollbar_visibility(IN_THREAD),
			 element_pos.x, element_pos.width,
			 current_position.width,
			 element_in_peephole->horizontal_increment(IN_THREAD));

	update_scrollbar(IN_THREAD,
			 v_scrollbar->impl,
			 vertical_scrollbar_element,
			 vertical_scrollbar_visibility(IN_THREAD),
			 element_pos.y, element_pos.height,
			 current_position.height,
			 element_in_peephole->vertical_increment(IN_THREAD));
}

void scrollbarsObj
::update_scrollbar(ONLY IN_THREAD,
		   const ref<scrollbarObj::implObj> &scrollbar,
		   const elementimpl &visibility_element,
		   const scrollbar_visibility visibility,
		   coord_t pos,
		   dim_t size,
		   dim_t peephole_size,
		   dim_t increment)
{
	scrollbar_config new_config;

	new_config.range=scroll_v_t::truncate(size);
	new_config.page_size=scroll_v_t::truncate(peephole_size);
	new_config.value=scroll_v_t::truncate(-pos);
	new_config.increment=scroll_v_t::truncate(increment);

	if (new_config.page_size >= new_config.range)
	{
		// Easier for update_config() to optimize itself away.
		new_config.range=new_config.page_size;
		new_config.value=0;
	}

	scrollbar->reconfigure(IN_THREAD, new_config);

	// Even when we're not visible we must still religiously do the above
	// and update_config(), so that the scrollbar configuration reflects
	// reality. Scroll wheel-initiated scrolling (pointer button 4/5)
	// may initiate scrolling even when this vertical scrollbar is not
	// visible, and if it's not visible because there's nothing to scroll,
	// the scrollbar metrics should reflect that.

	if (visibility == scrollbar_visibility::never)
		visibility_element->request_visibility(IN_THREAD, false);
	else if (visibility == scrollbar_visibility::always)
		visibility_element->request_visibility(IN_THREAD, true);
	else
		visibility_element->request_visibility(IN_THREAD,
						       new_config.range
						       > new_config.page_size);
}

LIBCXXW_NAMESPACE_END
