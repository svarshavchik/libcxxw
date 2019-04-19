/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/element.H"
#include "defaulttheme.H"
#include "inherited_visibility_info.H"
#include "screen.H"
#include "connection_thread.H"
#include "batch_queue.H"
#include "generic_window_handler.H"
#include "x/w/impl/draw_info.H"
#include "busy.H"
#include "icon.H"
#include "pixmap_with_picture.H"
#include "cursor_pointer.H"
#include "run_as.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/child_elementobj.H"
#include "grabbed_pointer.H"
#include "x/w/element_state.H"
#include "x/w/scratch_buffer.H"
#include "x/w/motion_event.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/tooltip.H"
#include "x/w/main_window.H"
#include "focus/label_for.H"
#include "x/w/impl/fonts/fontcollection.H"
#include "xim/ximclient.H"
#include "popup/popup.H"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"
#include "popup/popup_attachedto_info.H"
#include "catch_exceptions.H"
#include <x/logger.H>
#include <x/weakcapture.H>
#include <x/strtok.H>
#include <x/join.H>
#include <algorithm>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::elementObj::implObj);

LIBCXXW_NAMESPACE_START

// #define DEBUG_EXPOSURE_CALCULATIONS

elementObj::implObj::implObj(size_t nesting_level,
			     const rectangle &initial_position,
			     const popupptr &attached_popup,
			     const metrics::horizvert_axi &initial_metrics,
			     const screen &my_screen,
			     const const_pictformat &my_pictformat,
			     const std::string &scratch_buffer_id)
	: metrics::horizvertObj(initial_metrics),
	data_thread_only
	({
	  initial_position, initial_position, attached_popup
	}),
	nesting_level(nesting_level),
	element_scratch_buffer(my_screen->impl->create_scratch_buffer
			       (my_screen, scratch_buffer_id, my_pictformat))
{
}

elementObj::implObj::~implObj()
{
	if (data_thread_only.removed)
		return;

	LOG_ERROR("removed_from_container() was not called for an element");
}

screen elementObj::implObj::get_screen()
{
	return get_window_handler().screenref;
}

const_screen elementObj::implObj::get_screen() const
{
	return get_window_handler().screenref;
}

void elementObj::implObj::removed_from_container()
{
	THREAD->run_as
		([impl=ref(this)]
		 (ONLY IN_THREAD)
		 {
			 impl->removed_from_container(IN_THREAD);
		 });
}

void elementObj::implObj::removed_from_container(ONLY IN_THREAD)
{
	if (removed_from_container_was_called_in_destructor)
		return;

	removed_from_container_was_called_in_destructor=true;

	// Who knows, maybe we haven't been initialized yet?

	initialize_if_needed(IN_THREAD);

	if (data(IN_THREAD).removed)
		return;

	data(IN_THREAD).removed=true;
	removed(IN_THREAD);
	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->removed_from_container(IN_THREAD);
		       });
	set_inherited_visibility_flag(IN_THREAD, false, false);

	get_window_handler().removing_element_from_window(IN_THREAD,
							  ref(this));
}

void elementObj::implObj::removed(ONLY IN_THREAD)
{
}

void elementObj::implObj::toggle_visibility()
{
	THREAD->get_batch_queue()
		->run_as([me=ref(this)]
			 (ONLY IN_THREAD)
			 {
				 me->toggle_visibility(IN_THREAD);
			 });
}

void elementObj::implObj::request_visibility(bool flag)
{
	// Batch it up, to be done in bulk.

	THREAD->get_batch_queue()->schedule_for_visibility(ref(this), flag);
}

void elementObj::implObj::toggle_visibility(ONLY IN_THREAD)
{
	request_visibility(IN_THREAD,
			   !data(IN_THREAD).requested_visibility);
}

void elementObj::implObj::request_visibility(ONLY IN_THREAD, bool flag)
{
	if (data(IN_THREAD).requested_visibility == flag)
		return;

	data(IN_THREAD).requested_visibility=flag;

	schedule_update_visibility(IN_THREAD);
}

void elementObj::implObj::schedule_update_visibility(ONLY IN_THREAD)
{
	IN_THREAD->insert_element_set
		(*IN_THREAD->visibility_updated(IN_THREAD), element_impl(this));
}

void elementObj::implObj::request_visibility_recursive(bool flag)
{
	THREAD->get_batch_queue()->run_as
		([flag, me=element_impl(this)]
		 (ONLY IN_THREAD)
		 {
			 me->request_visibility_recursive(IN_THREAD, flag);
		 });
}

void elementObj::implObj::request_visibility_recursive(ONLY IN_THREAD,
						       bool flag)
{
	request_visibility(IN_THREAD, flag);
}

void elementObj::implObj::update_visibility(ONLY IN_THREAD)
{
	// Ignore visibility updates until such time we are
	// initialize_if_needed().

	if (!initialized(IN_THREAD))
		return;

	// This is invoked from the connection thread, when it processes the
	// IN_THREAD->visibility_updated set.
	//
	// Do nothing if request_visibility() matches actual_visibility.
	// Otherwise set actual_visibility to requested_visibility and call
	// visibility_updated().

	if (data(IN_THREAD).actual_visibility ==
	    data(IN_THREAD).requested_visibility)
		return;

	visibility_updated(IN_THREAD,
			   (
			    data(IN_THREAD).actual_visibility=
			    data(IN_THREAD).requested_visibility));
}

void elementObj::implObj::visibility_updated(ONLY IN_THREAD, bool flag)
{
	// This is called when this element's actual visibility changes.
	//
	// Note that child_element overrides it, and checks if the parent
	// container's inherited_visibility is false, and then overrides 'flag'
	// to false in that case.
	//
	// If the visibility matches the current inherited_visibility, do
	// nothing.

	if (data(IN_THREAD).reported_inherited_visibility == flag)
		return;

	// Prepare the default inherited_visibility_info object: here's the
	// new visibility status, in "flag", and do_not_redraw is false,
	// because we certainly want to redraw the element, as a result of the
	// visibility change.

	inherited_visibility_info visibility_info{flag, false};

	inherited_visibility_updated(IN_THREAD, visibility_info);

	if (!visibility_info.do_not_redraw)
		draw_after_visibility_updated(IN_THREAD, flag);
}

void elementObj::implObj
::inherited_visibility_updated(ONLY IN_THREAD,
			       inherited_visibility_info &visibility_info)
{
	// This is called when the element's inherited_visibility, the
	// "real" visibility, after taking into consideration the parent
	// display element's visibility, changes.
	//
	// This calls do_inherited_visibility_updated(). container_impl
	// overrides this, and also takes care of whatever needs to be done
	// with the child elements, in addition to calling
	// do_inherited_visibility_updated(), too.
	do_inherited_visibility_updated(IN_THREAD, visibility_info);
}

void elementObj::implObj
::do_inherited_visibility_updated(ONLY IN_THREAD,
				  inherited_visibility_info &info)
{
	// Officially record the fact that this element is now visible, or
	// not visible, for real.
	//
	// Notify handlers that we're about to show or hide this element.

	invoke_element_state_updates(IN_THREAD,
				     info.flag
				     ? element_state::before_showing
				     : element_state::before_hiding);

	set_inherited_visibility(IN_THREAD, info);

	// Notify handlers that we just shown or hidden this element.

	invoke_element_state_updates(IN_THREAD,
				     info.flag
				     ? element_state::after_showing
				     : element_state::after_hiding);
}

void elementObj::implObj
::set_inherited_visibility(ONLY IN_THREAD,
			   inherited_visibility_info &info)
{
	set_inherited_visibility_flag(IN_THREAD, info.flag, info.flag);
}

void elementObj::implObj
::set_inherited_visibility_flag(ONLY IN_THREAD,
				bool logical_flag,
				bool reported_flag)
{
	// Offically update this element's "real" visibility.

	data(IN_THREAD).logical_inherited_visibility=logical_flag;
	data(IN_THREAD).reported_inherited_visibility=reported_flag;

	if (!reported_flag)
	{
		unschedule_hover_action(IN_THREAD);

		// Also hide the popup.
		if (data(IN_THREAD).attached_popup_impl)
			data(IN_THREAD).attached_popup_impl
				->request_visibility(IN_THREAD, false);
	}
}

void elementObj::implObj::draw_after_visibility_updated(ONLY IN_THREAD,
							bool flag)
{
	// Display the contents of this element.
	//
	// generic_window_handler overrides this, and maps or unmaps the
	// window. This is what this action means for actual windows.
	//
	// Otherwise we call schedule_full_redraw().
	schedule_full_redraw(IN_THREAD);
}

void elementObj::implObj
::invalidate_cached_draw_info(ONLY IN_THREAD,
			      draw_info_invalidation_reason reason)
{
	if (!data(IN_THREAD).cached_draw_info)
		// Any child elements should not have anything cached either
		// get_draw_info_from_scratch() calls the parent element's
		// get_draw_info(), so iff an element has a cached draw_info,
		// its parent container should have one too.
		return;

	if (reason == draw_info_invalidation_reason::something_changed)
	{
		auto previous_di=data(IN_THREAD).cached_draw_info;

		auto &new_di=get_draw_info_from_scratch(IN_THREAD);

		if (new_di == previous_di)
			return; // Nothing really changed.
	}
	else
	{
		// If something_changed, we went through the hassle to
		// get_draw_info_from_scratch, so we might as well keep it,
		// and just do a recursive_invalidation.
		//
		// Otherwise, we are simply nuking the stale stuff, here.
		data(IN_THREAD).cached_draw_info.reset();
	}

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->invalidate_cached_draw_info
				       (IN_THREAD,
					draw_info_invalidation_reason
					::recursive_invalidation);
		       });
}

void elementObj::implObj::schedule_full_redraw(ONLY IN_THREAD)
{
	if (!get_window_handler().has_exposed(IN_THREAD))
		return;

	if (data(IN_THREAD).current_position.width == 0 ||
	    data(IN_THREAD).current_position.height == 0)
		return; // Nothing to redraw.

	IN_THREAD->elements_to_redraw(IN_THREAD)->insert(element_impl{this});
	data(IN_THREAD).areas_to_redraw.reset();
}

void elementObj::implObj::schedule_redraw(ONLY IN_THREAD,
					  const rectangle &area)
{
	if (!get_window_handler().has_exposed(IN_THREAD))
		return;

	if (DO_NOT_DRAW(IN_THREAD))
		return;

	if (data(IN_THREAD).current_position.width == 0 ||
	    data(IN_THREAD).current_position.height == 0)
		return; // Nothing to redraw.

	if (!data(IN_THREAD).areas_to_redraw)
		return; // Full redraw pending.

	auto &areas=*data(IN_THREAD).areas_to_redraw;
	auto b=areas.begin(), e=areas.end();

	if (e-b >= 10)
	{
		schedule_full_redraw(IN_THREAD); // Just do everything
		return;
	}
	if (std::find(b, e, area) != e)
		return; // Already there, minor optimization.

	IN_THREAD->elements_to_redraw(IN_THREAD)->insert(element_impl{this});
	areas.push_back(area);
}

void elementObj::implObj::schedule_redraw_recursively()
{
	THREAD->run_as([me=ref(this)]
		       (ONLY IN_THREAD)
		       {
			       me->schedule_redraw_recursively(IN_THREAD);
		       });
}

void elementObj::implObj::schedule_redraw_recursively(ONLY IN_THREAD)
{
	if (data(IN_THREAD).current_position.width == 0 ||
	    data(IN_THREAD).current_position.height == 0)
		return; // Nothing to redraw.

	schedule_full_redraw(IN_THREAD);

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->schedule_redraw_recursively(IN_THREAD);
		       });
}

void elementObj::implObj::enablability_changed(ONLY IN_THREAD)
{
	schedule_full_redraw(IN_THREAD);

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->enablability_changed(IN_THREAD);
		       });
}

rectarea draw_info::entire_area() const
{
	if (absolute_location.width == 0 || absolute_location.height == 0)
		return {};

	return {{0, 0, absolute_location.width, absolute_location.height}};
}

rectangle elementObj::implObj::get_absolute_location_on_screen(ONLY IN_THREAD)
	const
{
	auto r=get_absolute_location(IN_THREAD);

	get_window_handler().get_absolute_location_on_screen(IN_THREAD, r);

	return r;
}

bool elementObj::implObj::can_be_under_pointer(ONLY IN_THREAD) const
{
	// Ignore zombies!

	if (data(IN_THREAD).removed)
		return false;

	if (!data(IN_THREAD).reported_inherited_visibility)
		return false;

	return true;
}

bool elementObj::implObj::full_redraw_scheduled(ONLY IN_THREAD)
{
	if (!data(IN_THREAD).areas_to_redraw)
		return true; // Fast check.

	auto elements_to_redraw=IN_THREAD->elements_to_redraw(IN_THREAD);

	return elements_to_redraw->find(ref{this}) != elements_to_redraw->end();
}

void elementObj::implObj::explicit_redraw(ONLY IN_THREAD)
{
	// Remove myself from the connection thread's list.

	IN_THREAD->elements_to_redraw(IN_THREAD)->erase(ref<implObj>(this));

	if (data(IN_THREAD).removed)
		return;

	// Invoke draw() to refresh the contents of this disiplay element.

	auto &di=get_draw_info(IN_THREAD);

	rectarea what_to_redraw;

	if (data(IN_THREAD).areas_to_redraw)
	{
		std::swap(*data(IN_THREAD).areas_to_redraw,
			  what_to_redraw); // Resets area_to_redraw.
	}
	else
	{
		what_to_redraw=di.entire_area();
		data(IN_THREAD).areas_to_redraw=rectarea{}; // Resets it.
	}

	// Simulate an exposure of the entire element.

	draw(IN_THREAD, di, what_to_redraw);
}

void elementObj::implObj
::on_state_update(const functionref<element_state_callback_t> &cb)
{
	THREAD->get_batch_queue()->run_as
		([cb, me=ref{this}]
		 (ONLY IN_THREAD)
		 {
			 me->on_state_update(IN_THREAD, cb);
		 });
}

void elementObj::implObj
::on_state_update(ONLY IN_THREAD,
		  const functionref<element_state_callback_t> &cb)
{
	data(IN_THREAD).element_state_callback=cb;

	try {
		cb(IN_THREAD,
		   create_element_state(IN_THREAD,
					element_state::current_state),
		   busy_impl{*this});
	} REPORT_EXCEPTIONS(this);
}

//! Install a metrics update callback.

void elementObj::implObj
::on_metrics_update(const functionref<metrics_update_callback_t> &cb)
{
	// We want to install the metrics callback immediately, however
	// keep a reference on the batch queue until this is done, in order
	// to delay element position update and recalculation until
	// the metrics callback gets installed.
	//
	// Font picker hooks the metrics update of the font element dropdown
	// in the popup, in order to set the metrics of the current font name,
	// so we want this to get propagated to the current font name
	// display element before the font name label's metrics get
	// factored in.

	auto batch_queue=THREAD->get_batch_queue();

	THREAD->run_as
		([cb, me=ref(this), batch_queue]
		 (ONLY IN_THREAD)
		 {
			 me->data(IN_THREAD).metrics_update_callback=cb;

			 auto hv=me->get_horizvert(IN_THREAD);

			 try {
				 cb(IN_THREAD, hv->horiz, hv->vert);
			 } REPORT_EXCEPTIONS(me);
		 });
}


void elementObj::implObj::set_minimum_override(dim_t horiz_override,
					       dim_t vert_override)
{
	THREAD->run_as([me=ref(this),
			horiz_override,
			vert_override]
		       (ONLY IN_THREAD)
		       {
			       me->set_minimum_override(IN_THREAD,
							horiz_override,
							vert_override);
		       });
}

void elementObj::implObj::set_minimum_override(ONLY IN_THREAD,
					       dim_t horiz_override,
					       dim_t vert_override)
{
	get_horizvert(IN_THREAD)
		->set_minimum_override(IN_THREAD,
				       horiz_override, vert_override);
}

void elementObj::implObj::update_current_position(ONLY IN_THREAD,
						  const rectangle &r)
{
	auto &current_data=data(IN_THREAD);

	if (r == current_data.current_position)
		return;

	current_data.current_position=r;

	notify_updated_position(IN_THREAD);
	current_position_updated(IN_THREAD);
}

void elementObj::implObj::scroll_by_parent_container(ONLY IN_THREAD,
						     coord_t x,
						     coord_t y)
{
	auto current_position=data(IN_THREAD).current_position;

	current_position.x=x;
	current_position.y=y;

	update_current_position(IN_THREAD, current_position);
}

void elementObj::implObj::current_position_updated(ONLY IN_THREAD)
{
	schedule_update_position_processing(IN_THREAD);

	// Well, if we changed position, so must be all child elements.

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->current_position_updated(IN_THREAD);
		       });
}

void elementObj::implObj
::absolute_location_updated(ONLY IN_THREAD,
			    absolute_location_update_reason reason)
{
	if (reason == absolute_location_update_reason::internal)
	{
		invalidate_cached_draw_info(IN_THREAD, {});
		schedule_full_redraw(IN_THREAD);
	}

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->absolute_location_updated(IN_THREAD,
								  reason);
		       });
	update_attachedto_info(IN_THREAD);
}

std::string elementObj::implObj::element_name()
{
	std::ostringstream o;

	element_name(o);
	return o.str();
}

void elementObj::implObj::element_name(std::ostream &o)
{
	// Some heuristics, to come up with a reasonable label, based on the
	// class name.

	std::vector<std::string> components;

	strtok_str(objname(), ":", components);

	size_t n=components.size();

	if (n >= 2)
	{
		n -= 2;

		if (components.at(n) == "w")
			++n;
	}
	else
		n=0;

	std::string s=join(components.begin()+n, components.end(), "::");

	o << s.substr(0, s.find('>'));
}

void elementObj::implObj::schedule_update_position_processing(ONLY IN_THREAD)
{
	IN_THREAD->insert_element_set(*IN_THREAD->element_position_updated
				      (IN_THREAD),
				      element_impl(this));
}

void elementObj::implObj::process_updated_position(ONLY IN_THREAD)
{
	schedule_full_redraw(IN_THREAD);

	// Position gets factored into cached_draw_info, so this may no
	// longer be valid.
	invalidate_cached_draw_info(IN_THREAD,
				    draw_info_invalidation_reason
				    ::something_changed);

	update_attachedto_info(IN_THREAD);

	// If our position relative to our parent container has changed,
	// we need to notify any child element of this container, if tihs
	// element is a container, that their absolute_location_updated().

	if (data(IN_THREAD).previous_position.x !=
	    data(IN_THREAD).current_position.x ||
	    data(IN_THREAD).previous_position.y !=
	    data(IN_THREAD).current_position.y)
	{
		for_each_child
			(IN_THREAD,
			 [&]
			 (const element &e)
			 {
				 e->impl->absolute_location_updated
					 (IN_THREAD,
					  absolute_location_update_reason
					  ::internal);
			 });
	}
}

void elementObj::implObj::process_same_position(ONLY IN_THREAD)
{
}

void elementObj::implObj::notify_updated_position(ONLY IN_THREAD)
{
	invoke_element_state_updates(IN_THREAD,
				     element_state::current_state);
}

element_state elementObj::implObj
::create_element_state(ONLY IN_THREAD,
		       element_state::state_update_t element_state_for)
{
	auto &current_data=data(IN_THREAD);

	return element_state{
		element_state_for,
			// We send this update any time inherited visibility
			// changes, see do_inherited_visibility_updated().
		current_data.reported_inherited_visibility,
			// We send this update any time current_position
			// changes. update_current_position() calls
			// notify_updated_position().
		current_data.current_position
			};
}

void elementObj::implObj
::invoke_element_state_updates(ONLY IN_THREAD,
			       element_state::state_update_t reason)
{
	auto &cb=data(IN_THREAD).element_state_callback;

	if (cb)
		try {
			cb(IN_THREAD, create_element_state(IN_THREAD, reason),
			   busy_impl{*this});
		} REPORT_EXCEPTIONS(this);
}

clip_region_set::clip_region_set(ONLY IN_THREAD,
				 elementObj::implObj &e,
				 const draw_info &di)
	: clip_region_set{IN_THREAD, e.get_window_handler(), di}
{
}

clip_region_set::clip_region_set(ONLY IN_THREAD,
				 generic_windowObj::handlerObj &h,
				 const draw_info &di)
{
	// Our window inherits from pictureObj::implObj.

	h.set_clip_rectangles(di.element_viewport);
}

void elementObj::implObj
::exposure_event_recursively_top_down(ONLY IN_THREAD,
				      const rectarea &r)
{
	std::queue<std::tuple<element_impl, rectarea>> q;

	q.emplace(ref{this}, r);

	while (!q.empty())
	{
		auto &[impl, s]=q.front();

		impl->exposure_event_recursive(IN_THREAD, s, q);

		q.pop();
	}
}

void elementObj::implObj::exposure_event_recursive(ONLY IN_THREAD,
						   const rectarea &areas,
						   std::queue<std::tuple
						   <element_impl, rectarea>
						   > &q)
{
	auto &current_position=data(IN_THREAD).current_position;

	if (current_position.width == 0 || current_position.height == 0)
		return;

	if (data(IN_THREAD).removed)
		return;

	auto &di=get_draw_info(IN_THREAD);

#ifdef DEBUG_EXPOSURE_CALCULATIONS

	std::cout << "Exposure: " << objname() << ": "
		  << data(IN_THREAD).current_position << std::endl;

	for (const auto &r:areas)
		std::cout << "        " << r << std::endl;

	std::cout << "    Viewport:" << std::endl;

	for (const auto &r:di.element_viewport)
		std::cout << "        " << r << std::endl;
#endif
	// The intersection of areas, and the calculated viewport, is what
	// we need to draw.
	//
	// But draw() expects all coordinates relative to the display
	// element, and they're absolute now. No problem.

	auto draw_area=intersect(di.element_viewport, areas,
				 -di.absolute_location.x,
				 -di.absolute_location.y);

	if (draw_area.size() == 1)
	{
		auto &r=*draw_area.begin();

		if (r.x == 0 && r.y == 0 &&
		    r.width == current_position.width &&
		    r.height == current_position.height)
		{
			// The entire element is exposed. It looks better
			// for a popup to have its entire area immediately
			// cleared, before proceeding and rendering all the
			// elements inside the popup. A complicated popup
			// will take noticably longer to render, and it looks
			// sloppy to have the popup drawn, rectangle by
			// rectangle. This clears the top level popup element
			// (well, the element inside the real top level popup
			// element, which contains the popup's borders)
			// to the popup's background color, and we'll proceed
			// and draw what's inside the popup.

			if (should_preclear_entirely_exposed_element(IN_THREAD)
			    &&
			    get_window_handler()
			    .should_preclear_exposed(IN_THREAD))
				clear_to_color(IN_THREAD,
					       get_draw_info(IN_THREAD),
					       draw_area);
		}
	}


#ifdef DEBUG_EXPOSURE_CALCULATIONS

	std::cout << "    Draw:" << std::endl;

	for (const auto &r:draw_area)
		std::cout << "        " << r << std::endl;
#endif

	if (draw_area.empty())
		return;

	// If there's a queued redraw, we'll just redraw it right now, and
	// forget it.

	if (full_redraw_scheduled(IN_THREAD))
	{
		explicit_redraw(IN_THREAD);
	}
	else
	{
		draw(IN_THREAD, di, draw_area);
	}

	// Now, we need to recursively propagate this event.

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       q.emplace(e->impl, areas);
		       });
}

bool elementObj::implObj
::should_preclear_entirely_exposed_element(ONLY IN_THREAD)
{
	return false;
}

void elementObj::implObj::draw(ONLY IN_THREAD,
			       const draw_info &di,
			       const rectarea &areas)
{
	if (areas.empty() || di.element_viewport.empty())
		return; // Don't bother.

	if (DO_NOT_DRAW(IN_THREAD))
		clear_to_color(IN_THREAD, di, areas);
	else
		do_draw(IN_THREAD, di, areas);
}

void elementObj::implObj::do_draw(ONLY IN_THREAD,
				  const draw_info &di,
				  const rectarea &areas)
{
	clear_to_color(IN_THREAD, di, areas);
}

void elementObj::implObj
::do_draw_using_scratch_buffer(ONLY IN_THREAD,
			       const function<scratch_buffer_draw_func_t> &cb,
			       const rectangle &rect,
			       const draw_info &di,
			       const draw_info &background_color_di,
			       const clip_region_set &clipped)
{
	do_draw_using_scratch_buffer(IN_THREAD, cb, rect,
				     di, background_color_di, clipped,
				     element_scratch_buffer);
}

void elementObj::implObj
::do_draw_using_scratch_buffer(ONLY IN_THREAD,
			       const function<scratch_buffer_draw_func_t> &cb,
			       const rectangle &rect,
			       const draw_info &di,
			       const draw_info &background_color_di,
			       const clip_region_set &clipped,
			       const scratch_buffer &buffer)
{
	if (di.no_viewport())
		return;

	buffer->get
		(rect.width,
		 rect.height,
		 [&, this]
		 (const picture &area_picture,
		  const pixmap &area_pixmap,
		  const gc &area_gc)
		 {
			 rectangle area_entire_rect{0, 0,
					 rect.width, rect.height};

			 auto bgxy=background_color_di.background_xy_to(di);

			 area_picture->impl
				 ->composite(background_color_di
					     .window_background_color->impl,
					     coord_t::truncate(bgxy.first
							       + rect.x),
					     coord_t::truncate(bgxy.second
							       + rect.y),
					     area_entire_rect);

			 cb(area_picture, area_pixmap, area_gc);

			 this->draw_to_window_picture(IN_THREAD,
						      clipped,
						      di,
						      area_picture,
						      rect);
		 });

}

void elementObj::implObj
::draw_to_window_picture(ONLY IN_THREAD,
			 const clip_region_set &set,
			 const draw_info &di,
			 const picture &contents,
			 const rectangle &rect)
{
	rectangle cpy=rect;

	cpy.x = coord_t::truncate(cpy.x + di.absolute_location.x);
	cpy.y = coord_t::truncate(cpy.y + di.absolute_location.y);

	auto &wh=get_window_handler();

	if ((draw_to_window_picture_as_disabled(IN_THREAD) ||
	     set.draw_as_disabled) &&
	    data(IN_THREAD).logical_inherited_visibility)
	{
		// Disabled element rendering -- dither using our
		// element's background color.

		auto xy=di.background_xy_to(di, rect.x, rect.y);

		contents->impl->composite(di.window_background_color->impl,
					  wh.disabled_mask(IN_THREAD)
					  ->image->icon_picture->impl,
					  xy.first, xy.second,
					  rect.x, rect.y,
					  0, 0,
					  cpy.width, cpy.height,
					  render_pict_op::op_over);
	}

	// If there's a busy mcguffin outstanding, and composition is available,
	// draw a shade on top of us.

	if (wh.is_shade_busy() && wh.drawable_pictformat->alpha_depth > 0)
	{
		contents->composite(wh.shaded_color(IN_THREAD)
				    ->get_current_color(IN_THREAD),
				    cpy.x,
				    cpy.y,
				    {0, 0, cpy.width, cpy.height},
				    render_pict_op::op_atop);
	}

	// generic_window_handler inherits from pictureObj::implObj
	wh.composite(contents->impl, 0, 0, cpy);
}
void elementObj::implObj::clear_to_color(ONLY IN_THREAD,
					 const draw_info &di,
					 const rectarea &areas)
{
	clear_to_color(IN_THREAD,
		       clip_region_set(IN_THREAD, get_window_handler(), di),
		       di, di, areas);
}

void elementObj::implObj::clear_to_color(ONLY IN_THREAD,
					 const clip_region_set &clip,
					 const draw_info &di,
					 const draw_info &background_color_di,
					 const rectarea &areas)
{
#ifdef CLEAR_TO_COLOR_LOG
	CLEAR_TO_COLOR_LOG();
#endif

	// Take the viewport, and mask out the areas we're clearing.
	// If we have a large element inside a peephole, this avoids us having
	// to allocate a huge scratch buffer, with most of it being unused.

	rectarea absareas=di.element_viewport;

	for (auto &area:absareas)
	{
		area.x = coord_t::truncate(area.x-di.absolute_location.x);
		area.y = coord_t::truncate(area.y-di.absolute_location.y);
	}

	absareas=intersect(absareas, areas);

	for (const auto &area : absareas)
	{
#ifdef CLEAR_TO_COLOR_RECT
		CLEAR_TO_COLOR_RECT();
#endif
		draw_using_scratch_buffer
			(IN_THREAD,
			 []
			 (const auto &, const auto &, const auto &)
			 {
			 },
			 area,
			 di, background_color_di,
			 clip);
	}
}

void elementObj::implObj::remove_background_color()
{
	THREAD->run_as([impl=ref<implObj>(this)]
		       (ONLY IN_THREAD)
		       {
			       impl->remove_background_color(IN_THREAD);
		       });
}

void elementObj::implObj
::set_background_color(const color_arg &theme_color)
{
	set_background_color(get_screen()->impl
			     ->create_background_color(theme_color));
}

void elementObj::implObj
::set_background_color(const background_color &c)
{
	THREAD->run_as([impl=ref<implObj>(this), c]
		       (ONLY IN_THREAD)
		       {
			       impl->set_background_color(IN_THREAD, c);
		       });
}

void elementObj::implObj::background_color_changed(ONLY IN_THREAD)
{
	schedule_full_redraw(IN_THREAD);

	// background color factors into the cached_draw_info, so
	// something_changed.
	invalidate_cached_draw_info(IN_THREAD,
				    draw_info_invalidation_reason
				    ::something_changed);

	for_each_child(IN_THREAD, [&]
		       (const element &e)
		       {
			       if (e->impl->data(IN_THREAD)
				   .logical_inherited_visibility &&
				   e->impl->has_own_background_color(IN_THREAD))
				       return;
			       e->impl->background_color_changed(IN_THREAD);
		       });
}

void elementObj::implObj::theme_updated(ONLY IN_THREAD,
					const const_defaulttheme &new_theme)
{
	invalidate_cached_draw_info(IN_THREAD,
				    draw_info_invalidation_reason
				    ::recursive_invalidation);

	if (data(IN_THREAD).logical_inherited_visibility)
		schedule_full_redraw(IN_THREAD);

	for_each_child(IN_THREAD, [&]
		       (const element &e)
		       {
			       e->impl->theme_updated(IN_THREAD, new_theme);
		       });
}

void elementObj::implObj::initialize_or_log_exception(ONLY IN_THREAD)
{
	try {
		initialize(IN_THREAD);
	} CATCH_EXCEPTIONS;
}

void elementObj::implObj::initialize(ONLY IN_THREAD)
{
	update_attachedto_info(IN_THREAD);
}

void elementObj::implObj::do_for_each_child(ONLY IN_THREAD,
					    const function<void
					    (const element &e)> &)
{
}

size_t elementObj::implObj::num_children(ONLY IN_THREAD)
{
	return 0;
}

fontcollection elementObj::implObj::create_fontcollection(const font &f)
{
	auto &wh=get_window_handler();

	auto s=wh.get_screen();

	return s->create_fontcollection(f, wh.font_alpha_depth(),
					s->impl->current_theme.get());
}

fontcollection elementObj::implObj::create_fontcollection(const font &f,
							  const defaulttheme &t)
{
	auto &wh=get_window_handler();

	return wh.get_screen()->create_fontcollection(f, wh.font_alpha_depth(),
						      t);
}

background_color elementObj::implObj
::create_background_color(const color_arg &color_name)
{
	return get_screen()->impl->create_background_color(color_name);
}

void elementObj::implObj::on_keyboard_focus(const
					    functionref<focus_callback_t>
					    &callback)
{
	THREAD->run_as([me=ref(this), callback]
		       (ONLY IN_THREAD)
		       {
			       me->on_keyboard_focus(IN_THREAD, callback);
		       });
}

void elementObj::implObj::on_keyboard_focus(ONLY IN_THREAD,
					    const
					    functionref<focus_callback_t>
					    &callback)
{
	data(IN_THREAD).on_keyboard_callback=callback;
	invoke_keyboard_focus_callback(IN_THREAD, initial{});
}

void elementObj::implObj::report_keyboard_focus(ONLY IN_THREAD,
						focus_change event,
						const callback_trigger_t &t)
{
	if (event != focus_change::focus_movement_complete)
	{
		most_recent_keyboard_focus_change(IN_THREAD)=event;
		return;
	}

	keyboard_focus(IN_THREAD, t);
}


void elementObj::implObj::keyboard_focus(ONLY IN_THREAD,
					 const callback_trigger_t &trigger)
{
	unschedule_hover_action(IN_THREAD);
	invoke_keyboard_focus_callback(IN_THREAD, trigger);
}

void elementObj::implObj
::invoke_keyboard_focus_callback(ONLY IN_THREAD,
				 const callback_trigger_t &trigger)
{
	try {
		auto &cb=data(IN_THREAD).on_keyboard_callback;

		if (cb)
			cb(IN_THREAD,
			   most_recent_keyboard_focus_change(IN_THREAD),
			   trigger);
	} CATCH_EXCEPTIONS;
}

void elementObj::implObj::on_pointer_focus(const
					   functionref<focus_callback_t>
					   &callback)
{
	THREAD->run_as([me=ref(this), callback]
		       (ONLY IN_THREAD)
		       {
			       me->on_pointer_focus(IN_THREAD, callback);
		       });
}

void elementObj::implObj::on_pointer_focus(ONLY IN_THREAD,
					   const
					   functionref<focus_callback_t>
					   &callback)
{
	data(IN_THREAD).on_pointer_callback=callback;
	invoke_pointer_focus_callback(IN_THREAD, initial{});
}

void elementObj::implObj::report_pointer_focus(ONLY IN_THREAD,
					       focus_change event,
					       const callback_trigger_t
					       &trigger)
{
	if (event != focus_change::focus_movement_complete)
	{
		most_recent_pointer_focus_change(IN_THREAD)=event;
		return;
	}

	pointer_focus(IN_THREAD, trigger);
}

void elementObj::implObj::pointer_focus(ONLY IN_THREAD,
					const callback_trigger_t &trigger)
{
	unschedule_hover_action(IN_THREAD);
	invoke_pointer_focus_callback(IN_THREAD, trigger);
}

void elementObj::implObj
::invoke_pointer_focus_callback(ONLY IN_THREAD,
				const callback_trigger_t &trigger)
{
	try {
		auto &cb=data(IN_THREAD).on_pointer_callback;

		if (cb)
			cb(IN_THREAD,
			   most_recent_pointer_focus_change(IN_THREAD),
			   trigger);
	} REPORT_EXCEPTIONS(this);
}

void elementObj::implObj::window_focus_change(ONLY IN_THREAD, bool flag)
{
}

bool elementObj::implObj::current_keyboard_focus(ONLY IN_THREAD)
{
	return in_focus(most_recent_keyboard_focus_change(IN_THREAD));
}

bool elementObj::implObj::current_pointer_focus(ONLY IN_THREAD)
{
	return in_focus(most_recent_pointer_focus_change(IN_THREAD));
}

bool in_focus(focus_change v)
{
	return v != focus_change::lost && v != focus_change::child_lost;
}

void elementObj::implObj
::on_key_event(const functionref<key_event_callback_t> &cb)
{
	THREAD->run_as([me=ref(this), cb]
		       (ONLY IN_THREAD)
		       {
			       me->on_key_event(IN_THREAD, cb);
		       });
}

void elementObj::implObj
::on_key_event(ONLY IN_THREAD,
	       const functionref<key_event_callback_t> &cb)
{
	data(IN_THREAD).on_key_event_callback=cb;
}

bool elementObj::implObj::activate_for(const key_event &ke) const
{
	return ke.keypress;
}

bool elementObj::implObj::activate_for(const button_event &be) const
{
	return be.press;
}

bool elementObj::implObj::process_key_event(ONLY IN_THREAD, const key_event &e)
{
	busy_impl mcguffin{*this};

	auto &cb=data(IN_THREAD).on_key_event_callback;

	bool ret=false;

	if (cb)
		try {
			ret=cb(THREAD, &e, activate_for(e), mcguffin);
		} REPORT_EXCEPTIONS(this);
	return ret;
}

void elementObj::implObj::grabbed_key_event(ONLY IN_THREAD)
{
}

bool elementObj::implObj::uses_input_method()
{
	return false;
}

void elementObj::implObj::report_current_cursor_position(ONLY IN_THREAD,
							 rectangle pos)
{
	auto loc=get_absolute_location(IN_THREAD);

	pos.x=coord_t::truncate(pos.x+loc.x);
	pos.y=coord_t::truncate(pos.y+loc.y);

	get_window_handler().with_xim_client
		([&]
		 (auto &client)
		 {
			 client->current_cursor_position(IN_THREAD, pos);
		 });
}

void elementObj::implObj::report_motion_event(ONLY IN_THREAD,
					      const motion_event &me)
{
	data(IN_THREAD).last_motion_x=me.x;
	data(IN_THREAD).last_motion_y=me.y;

	// If a tooltip is installed, uninstall it before scheduling the
	// tooltip again.
	if (me.type == motion_event_type::real_motion)
	{
		unschedule_hover_action(IN_THREAD);

		if (me.mask.ordinal(true) == 0)
			schedule_hover_action(IN_THREAD);
	}

	if (data(IN_THREAD).on_motion_event_callback)
	{
		try {
			data(IN_THREAD).on_motion_event_callback(IN_THREAD, me);
		} REPORT_EXCEPTIONS(this);
	}
}

void elementObj::implObj::schedule_hover_action(ONLY IN_THREAD)
{
	auto &d=data(IN_THREAD);

	auto initial_delay=hover_action_delay(IN_THREAD);

	if (initial_delay == std::chrono::milliseconds{0})
		return;

	auto now=tick_clock_t::now();

	d.hover_scheduled_creation=now+initial_delay;

	if (d.hover_scheduled_mcguffin)
		return; // Timer already set.

	schedule_hover_timer(IN_THREAD, now);
}

void elementObj::implObj::schedule_hover_timer(ONLY IN_THREAD,
						 tick_clock_t::time_point now)
{
	data(IN_THREAD).hover_scheduled_mcguffin=IN_THREAD->schedule_callback
		(IN_THREAD,
		 now < data(IN_THREAD).hover_scheduled_creation
		 ? data(IN_THREAD).hover_scheduled_creation - now
		 : ++tick_clock_t::duration::zero(),
		 [me=make_weak_capture(ref<implObj>(this))]
		 (ONLY IN_THREAD)
		 {
			 auto got=me.get();

			 if (got)
			 {
				 auto &[me]=*got;

				 me->check_hover_timer(IN_THREAD);
			 }
		 });
}

void elementObj::implObj::check_hover_timer(ONLY IN_THREAD)
{
	// The timer to show the tooltip gets scheduled on a motion event.
	// For optimal performance the timer does not get rescheduled with
	// every event. Rather we save the hover_scheduled_creation time,
	// and now that the timer expired we'll check if the
	// hover_scheduled_creation time was reset, then try again.

	auto now=tick_clock_t::now();

	auto &d=data(IN_THREAD);
	d.hover_scheduled_mcguffin=nullptr; // Clear the expired mcguffin.

	// Recheck things.

	if (hover_action_delay(IN_THREAD) == std::chrono::milliseconds{0})
		return;

	if (now < d.hover_scheduled_creation)
	{
		schedule_hover_timer(IN_THREAD, now);
		return; // Reschedule me.
	}

	hover_action(IN_THREAD);
}

void elementObj::implObj::unschedule_hover_action(ONLY IN_THREAD)
{
	data(IN_THREAD).hover_scheduled_mcguffin=nullptr;

	hover_cancel(IN_THREAD);
}

void elementObj::implObj::ensure_visibility(ONLY IN_THREAD, const rectangle &r)
{
}

void elementObj::implObj::ensure_entire_visibility(ONLY IN_THREAD)
{
	ensure_visibility(IN_THREAD, {0, 0,
				data(IN_THREAD).current_position.width,
				data(IN_THREAD).current_position.height});
}

std::string elementObj::implObj::default_cut_paste_selection() const
{
	return get_window_handler().get_screen()->impl
		->current_theme.get()->default_cut_paste_selection();
}

bool elementObj::implObj::selection_can_be_received()
{
	return false;
}

bool elementObj::implObj::cut_or_copy_selection(cut_or_copy_op, xcb_atom_t)
{
	return false;
}

bool elementObj::implObj::cut_or_copy_selection(ONLY IN_THREAD,
						cut_or_copy_op, xcb_atom_t)
{
	return false;
}

bool elementObj::implObj::pasted(ONLY IN_THREAD,
				 const std::u32string_view &str)
{
	busy_impl mcguffin{*this};

	auto &cb=data(IN_THREAD).on_key_event_callback;

	return cb ? cb(IN_THREAD, &str, true, mcguffin):false;
}

void elementObj::implObj::creating_focusable_element()
{
}

void elementObj::implObj::exception_message(const exception &e)
{
	auto mw=get_window_handler().get_main_window();

	if (mw)
		mw->exception_message(e);
}

void elementObj::implObj::stop_message(const text_param &t)
{
	auto mw=get_window_handler().get_main_window();

	if (mw)
		mw->stop_message(t);
}

//////////////////////////////////////////////////////////////////////////////

void elementObj::implObj::set_cursor_pointer(ONLY IN_THREAD,
					     const cursor_pointer &p)
{
	if (data(IN_THREAD).pointer != p)
	{
		data(IN_THREAD).pointer=p;
		get_window_handler().update_displayed_cursor_pointer(IN_THREAD);
	}
}

void elementObj::implObj::remove_cursor_pointer(ONLY IN_THREAD)
{
	if (data(IN_THREAD).pointer)
	{
		data(IN_THREAD).pointer=nullptr;
		get_window_handler().update_displayed_cursor_pointer(IN_THREAD);
	}
}

cursor_pointerptr elementObj::implObj::get_cursor_pointer(ONLY IN_THREAD)
{
	return data(IN_THREAD).pointer;
}

void elementObj::implObj::update_attachedto_info(ONLY IN_THREAD)
{
	if (!data(IN_THREAD).initialized || !data(IN_THREAD).attached_popup)
		return;

	auto h=data(IN_THREAD).attached_popup->impl->handler;

	switch (h->attachedto_info->how) {
	case attached_to::tooltip:
		return;

	case attached_to::below_or_above:
	case attached_to::above_or_below:
	case attached_to::right_or_left:
		break;
	}

	h->update_attachedto_element_position
		(IN_THREAD,
		 get_absolute_location_on_screen(IN_THREAD));
}

void elementObj::implObj::save(ONLY IN_THREAD, const screen_positions &pos)
{
	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->save(IN_THREAD, pos);
		       });
}

LIBCXXW_NAMESPACE_END
