/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/container.H"
#include "container_impl.H"
#include "x/w/container.H"
#include "x/w/impl/layoutmanager.H"
#include "x/w/impl/child_element.H"
#include "inherited_visibility_info.H"
#include "x/w/impl/element_draw.H"
#include "connection_thread.H"
#include "x/w/impl/draw_info.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

containerObj::implObj::implObj(get_window_handlerObj &parent_get_window_handler)
	: get_window_handlerObj{parent_get_window_handler.get_window_handler()}
{
}

containerObj::implObj::~implObj()=default;

void containerObj::implObj
::install_layoutmanager(const layout_impl &impl)
{
	layoutmanager_ptr_t::lock lock(layoutmanager_ptr);

	if (*lock)
		throw EXCEPTION("Internal error: duplicate layout manager installation (already have a " << (*lock)->objname() << ")");

	if (impl->layout_container_impl !=
	    container_impl{this})
		throw EXCEPTION("Internal error: layout manager getting installed into the wrong container.");

	*lock=impl;
}


void containerObj::implObj::uninstall_layoutmanager(ONLY IN_THREAD)
{
	{
		layoutmanager_ptr_t::lock lock(layoutmanager_ptr);

		auto p=*lock;

		if (!p)
			throw EXCEPTION("Internal error - no layout manager to uninstall");
		*lock=layout_implptr();
	}
}

void containerObj::implObj::removed_from_container(ONLY IN_THREAD)
{
	const auto &logger=elementObj::implObj::logger;

	try {
		auto &e=container_element_impl();

		if (e.data(IN_THREAD).removed)
			return; // Already did this.

		e.elementObj::implObj
			::removed_from_container(IN_THREAD);
	} CATCH_EXCEPTIONS;

	uninstall_layoutmanager(IN_THREAD);
}

void containerObj::implObj::do_for_each_child(ONLY IN_THREAD,
					      const function<void (const element &e)> &f)
{
	invoke_layoutmanager([&]
			     (const auto &manager)
			     {
				     manager->do_for_each_child(IN_THREAD, f);
			     });
}

size_t containerObj::implObj::num_children(ONLY IN_THREAD)
{
	size_t n=0;

	invoke_layoutmanager([&]
			     (const auto &manager)
			     {
				     n=manager->num_children(IN_THREAD);
			     });
	return n;
}

// Common code shared by the container and the layout manager, to clear
// the container-provided padding around an element in the container, using
// the element's background color.
//
// This is invoked from containerObj::implObj::do_draw(), when drawing the
// container portion. This is also invoked by the layout manager when an
// element changes its background color, to redraw the padding with the new
// background color.
//
// Provided parameters:
//
// - The container element
// - The layout manager
// - The element whose padding needs to get drawn
// - The container element's get_draw_info().
// - The container element's instantiated clip region.
// - A rectarea where the area of the element, including its padding
//   region, gets added to.
//
// The calling convention is optimized for the do_draw() caller, with do_draw
// acquiring its get_draw_info() and clip region once, and using it for
// drawing all of its elements' padding; and accumulated the total area drawn
// into a rectarea. The layout manager, when it calls this as a result
// of the element's background color change, simply rides the coat-tails.

void container_clear_padding(ONLY IN_THREAD,
			     elementObj::implObj &container_element_impl,
			     layoutmanagerObj::implObj &manager,
			     const element_impl &e_impl,
			     const draw_info &di,
			     clip_region_set &clip,
			     rectarea &child_areas)
{
	rectangle padded_position=manager.padded_position(IN_THREAD, e_impl);

	// We combine all padded child areas into child_areas.
	child_areas.push_back(padded_position);

	rectangle position=e_impl->data(IN_THREAD).current_position;

	// Subtract position from
	// padded_position, to obtain the
	// padding area, then clear it to
	// the child's color.

	auto padding=subtract({padded_position}, {position});

	if (padding.empty())
		return;

	auto &child_di=e_impl->get_draw_info(IN_THREAD);

	container_element_impl
		.clear_to_color(IN_THREAD, clip, di, child_di, padding);
}

redraw_priority_t containerObj::implObj::get_redraw_priority(ONLY IN_THREAD)
{
	return clear_area;
}

void containerObj::implObj::do_draw(ONLY IN_THREAD)
{
	const auto &di=container_element_impl().get_draw_info(IN_THREAD);

	do_draw(IN_THREAD, di, di.entire_area());
}

void containerObj::implObj::do_draw(ONLY IN_THREAD,
				    const draw_info &di,
				    const rectarea &areas)
{
	auto &element_impl=container_element_impl();

	// Compute the areas where the child elements are.

	rectarea child_areas;

	clip_region_set clip{IN_THREAD, element_impl.get_window_handler(), di};

	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->do_draw(IN_THREAD, di, clip, child_areas);
		 });

	// Subtract it from our area.
	auto current_position=
		element_impl.data(IN_THREAD).current_position;

	current_position.x=0;
	current_position.y=0;

	rectarea my_area{ current_position };

	auto remaining=subtract(my_area, child_areas);

	if (!remaining.empty())
		element_impl.clear_to_color(IN_THREAD, clip, di, di, remaining);
}


void containerObj::implObj
::child_background_color_changed(ONLY IN_THREAD, const element_impl &child)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->child_background_color_changed(IN_THREAD,
								 child);
		 });
}

void containerObj::implObj
::requested_child_visibility_updated(ONLY IN_THREAD,
				     const element_impl &child,
				     bool flag)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->requested_child_visibility_changed
				 (IN_THREAD, child, flag);
		 });
}

void containerObj::implObj
::inherited_child_visibility_updated(ONLY IN_THREAD,
				     const element_impl &child,
				     inherited_visibility_info &info)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->inherited_child_visibility_changed
				 (IN_THREAD, child, info);
		 });
}

void containerObj::implObj
::inherited_visibility_updated_before(ONLY IN_THREAD,
				      inherited_visibility_info &info)
{
	// When the container gets hidden, the child elements are hidden
	// first. When the container gets shown, the child elements are
	// shown after the container.

	if (!info.flag)
		propagate_inherited_visibility(IN_THREAD, info);
}

void containerObj::implObj
::inherited_visibility_updated_after(ONLY IN_THREAD,
				     inherited_visibility_info &info)
{
	if (info.flag)
		propagate_inherited_visibility(IN_THREAD, info);
}

// If this container's child elements have actual_visibility set (they
// were explicitly shown, then hiding or showing the container changes
// the child elements' inherited visibility.
//
// Get the child elements from the layout manager, and invoke their
// inherited_visibility_updated().

void containerObj::implObj
::propagate_inherited_visibility(ONLY IN_THREAD,
				 inherited_visibility_info &info)
{
	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       if (!e->impl->data(IN_THREAD).actual_visibility)
				       return;

			       e->impl->inherited_visibility_updated
				       (IN_THREAD, info);
		       });
}

// If this container was shown, propagate this event to children.

void containerObj::implObj::draw_after_visibility_updated(ONLY IN_THREAD,
							  bool flag)
{
	draw_child_elements_after_visibility_updated(IN_THREAD, flag);
}

void containerObj::implObj
::draw_child_elements_after_visibility_updated(ONLY IN_THREAD, bool flag)
{
	if (!flag)
		return;

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       if (!e->impl->data(IN_THREAD).actual_visibility)
				       return;

			       // visibility_updated() took care of
			       // invoking draw_after_visbility_updated()
			       // for the container. We now need
			       // to do this ourselves for the
			       // children, so they can redraw
			       // themselves

			       e->impl->draw_after_visibility_updated
				       (IN_THREAD, flag);
		       });
}


void containerObj::implObj::process_updated_position(ONLY IN_THREAD)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->process_updated_position
				 (IN_THREAD,
				  container_element_impl().data(IN_THREAD)
				  .current_position);
		 });
}

void containerObj::implObj::process_same_position(ONLY IN_THREAD)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->process_same_position(IN_THREAD,
							container_element_impl()
							.data(IN_THREAD)
							.current_position);
		 });
}

void containerObj::implObj::request_child_visibility_recursive(ONLY IN_THREAD,
							       bool flag)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->request_visibility_recursive(IN_THREAD, flag);
		 });
}

#if 0
void containerObj::implObj::child_visibility_updated(ONLY IN_THREAD,
						     bool flag)
{
	needs_recalculation(IN_THREAD);
	container_element_impl().schedule_full_redraw(IN_THREAD);
}
#endif

void containerObj::implObj
::tell_layout_manager_it_needs_recalculation(ONLY IN_THREAD)
{
	invoke_layoutmanager
		([&]
		 (const auto &lm)
		 {
			 lm->needs_recalculation(IN_THREAD);
		 });
}

void containerObj::implObj::needs_recalculation(ONLY IN_THREAD)
{
	(*IN_THREAD->containers_2_recalculate(IN_THREAD))
		[container_element_impl().nesting_level]
		.insert(ref<implObj>(this));
}

void containerObj::implObj::initialize(ONLY IN_THREAD)
{
	invoke_layoutmanager([&](const auto &manager)
			     {
				     manager->initialize(IN_THREAD);
			     });
}

void containerObj::implObj::theme_updated(ONLY IN_THREAD,
					  const const_defaulttheme &new_theme)
{
	invoke_layoutmanager([&](const auto &manager)
			     {
				     manager->theme_updated(IN_THREAD,
							    new_theme);
			     });
}

void containerObj::implObj::ensure_visibility(ONLY IN_THREAD,
					      elementObj::implObj &e,
					      const rectangle &r)
{
	// First, have the layout manager do any adjustments to the element.

	invoke_layoutmanager([&](const auto &manager)
			     {
				     manager->ensure_visibility(IN_THREAD,
								e,
								r);
			     });

	ensured_visibility_of_child_element(IN_THREAD, e, r);
}

void containerObj::implObj
::ensured_visibility_of_child_element(ONLY IN_THREAD,
				      elementObj::implObj &e,
				      const rectangle &r)
{
	const auto &my_pos=container_element_impl().data(IN_THREAD).current_position;

	// Add the child element (x, y) coordinate to the requested visibility
	// (x, y) coordinate, to derive the position in this container whose
	// visibility is requested.

	auto ex=e.data(IN_THREAD).current_position.x+r.x;
	auto ey=e.data(IN_THREAD).current_position.y+r.y;

	// Adding the requested width+height to (ex, ey) computes the
	// bottom-right coordinate of position in this container whose
	// visibility is requested.

	auto right=ex+r.width;
	auto bottom=ey+r.height;

	// Is this element completely out of this container's boundaries?

	if (dim_t::truncate(ex) >= my_pos.width ||
	    dim_t::truncate(ey) >= my_pos.height ||
	    right <= 0 || bottom <= 0)
		return;

	// Clip the coordinates to this countainer's boundaries.

	if (ex < 0)
		ex=0;
	if (ey < 0)
		ey=0;

	if (right > coord_squared_t::truncate(my_pos.width))
		right=coord_squared_t::truncate(my_pos.width);

	if (bottom > coord_squared_t::truncate(my_pos.height))
		bottom=coord_squared_t::truncate(my_pos.height);

	coord_t new_x=coord_t::truncate(ex);
	coord_t new_y=coord_t::truncate(ey);


	container_element_impl().ensure_visibility(IN_THREAD, {
			new_x, new_y,
				dim_t::truncate(right-new_x),
				dim_t::truncate(bottom-new_y)
				});
}

void containerObj::implObj
::save(ONLY IN_THREAD, const screen_positions &pos)
{
	invoke_layoutmanager([&]
			     (const auto &manager)
			     {
				     manager->save(IN_THREAD, pos);
			     });
}

LIBCXXW_NAMESPACE_END
