/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "container_impl.H"
#include "x/w/container.H"
#include "layoutmanager.H"
#include "child_element.H"
#include "element_draw.H"
#include "connection_thread.H"
#include "draw_info.H"

LIBCXXW_NAMESPACE_START

containerObj::implObj::implObj()=default;

containerObj::implObj::~implObj()=default;

generic_windowObj::handlerObj &containerObj::implObj::get_window_handler()
{
	return get_element_impl().get_window_handler();
}

const generic_windowObj::handlerObj &containerObj::implObj::get_window_handler()
	const
{
	return get_element_impl().get_window_handler();
}

void containerObj::implObj
::install_layoutmanager(const ref<layoutmanagerObj::implObj> &impl)
{
	layoutmanager_ptr_t::lock lock(layoutmanager_ptr);

	if (*lock)
		throw EXCEPTION("Internal error: duplicate layout manager installation (already have a " << (*lock)->objname() << ")");

	if (impl->container_impl !=
	    ref<containerObj::implObj>(this))
		throw EXCEPTION("Internal error: layout manager getting installed into the wrong container.");

	*lock=impl;
}


void containerObj::implObj::uninstall_layoutmanager()
{
	layoutmanager_ptr_t::lock lock(layoutmanager_ptr);

	if (!*lock)
		throw EXCEPTION("Internal error - no layout manager to uninstall");
	*lock=ptr<layoutmanagerObj::implObj>();
}

void containerObj::implObj::do_for_each_child(IN_THREAD_ONLY,
					      const function<void (const element &e)> &f)
{
	invoke_layoutmanager([&]
			     (const auto &manager)
			     {
				     manager->do_for_each_child(IN_THREAD, f);
			     });
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
// - A rectangle_set where the area of the element, including its padding
//   region, gets added to.
//
// The calling convention is optimized for the do_draw() caller, with do_draw
// acquiring its get_draw_info() and clip region once, and using it for
// drawing all of its elements' padding; and accumulated the total area drawn
// into a rectangle_set. The layout manager, when it calls this as a result
// of the element's background color change, simply rides the coat-tails.

void container_clear_padding(IN_THREAD_ONLY,
			     elementObj::implObj &container_element_impl,
			     layoutmanagerObj::implObj &manager,
			     const elementimpl &e_impl,
			     const draw_info &di,
			     clip_region_set &clip,
			     rectangle_set &child_areas)
{
	rectangle padded_position=manager.padded_position(IN_THREAD, e_impl);

	// We combine all padded child areas into child_areas.
	child_areas.insert(padded_position);

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

void containerObj::implObj::do_draw(IN_THREAD_ONLY)
{
	const auto &di=get_element_impl().get_draw_info(IN_THREAD);

	do_draw(IN_THREAD, di, di.entire_area());
}

void containerObj::implObj::do_draw(IN_THREAD_ONLY,
				    const draw_info &di,
				    const rectangle_set &areas)
{
	auto &element_impl=get_element_impl();

	// Compute the areas where the child elements are.

	rectangle_set child_areas;

	clip_region_set clip{IN_THREAD, di};

	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->for_each_child
				 (IN_THREAD,
				  [&]
				  (const element &e)
				  {
					  container_clear_padding(IN_THREAD,
								  element_impl,
								  *manager,
								  e->impl,
								  di, clip,
								  child_areas);
				  });
		 });

	// Subtract it from our area.
	auto current_position=
		element_impl.data(IN_THREAD).current_position;

	current_position.x=0;
	current_position.y=0;

	rectangle_set my_area{ current_position };

	auto remaining=subtract(my_area, child_areas);

	if (!remaining.empty())
		element_impl.clear_to_color(IN_THREAD, clip, di, di, remaining);
}


void containerObj::implObj
::child_background_color_changed(IN_THREAD_ONLY, const elementimpl &child)
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
::child_visibility_updated(IN_THREAD_ONLY,
			   const elementimpl &child,
			   inherited_visibility_info &info)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->child_visibility_changed(IN_THREAD, child,
							   info);
		 });
}

void containerObj::implObj
::inherited_visibility_updated(IN_THREAD_ONLY,
			       inherited_visibility_info &info)
{
	// When the container gets hidden, the child elements are hidden
	// first. When the container gets shown, the child elements are
	// shown after the container.

	if (!info.flag)
		propagate_inherited_visibility(IN_THREAD, info);

	get_element_impl().do_inherited_visibility_updated(IN_THREAD, info);

	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->inherited_visibility_updated(IN_THREAD,
							       info.flag);
		 });

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
::propagate_inherited_visibility(IN_THREAD_ONLY,
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

void containerObj::implObj::draw_after_visibility_updated(IN_THREAD_ONLY,
							  bool flag)
{
	draw_child_elements_after_visibility_updated(IN_THREAD, flag);
}

void containerObj::implObj
::draw_child_elements_after_visibility_updated(IN_THREAD_ONLY, bool flag)
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


void containerObj::implObj::process_updated_position(IN_THREAD_ONLY)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->process_updated_position(IN_THREAD,
							   get_element_impl()
							   .data(IN_THREAD)
							   .current_position);
		 });
}

void containerObj::implObj::request_child_visibility_recursive(IN_THREAD_ONLY,
							       bool flag)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->request_visibility_recursive(IN_THREAD, flag);
		 });
}

void containerObj::implObj::request_visibility_recursive(IN_THREAD_ONLY,
							 bool flag)
{
	request_child_visibility_recursive(IN_THREAD, flag);

	auto &element_impl=get_element_impl();
	element_impl.elementObj::implObj::request_visibility_recursive
		(IN_THREAD, flag);
}

#if 0
void containerObj::implObj::child_visibility_updated(IN_THREAD_ONLY,
						     bool flag)
{
	needs_recalculation(IN_THREAD);
	get_element_impl().schedule_redraw(IN_THREAD);
}
#endif

void containerObj::implObj
::tell_layout_manager_it_needs_recalculation(IN_THREAD_ONLY)
{
	invoke_layoutmanager
		([&]
		 (const auto &lm)
		 {
			 lm->needs_recalculation(IN_THREAD);
		 });
}

void containerObj::implObj::needs_recalculation(IN_THREAD_ONLY)
{
	(*IN_THREAD->containers_2_recalculate(IN_THREAD))
		[get_element_impl().nesting_level]
		.insert(ref<implObj>(this));
}

void containerObj::implObj::theme_updated(IN_THREAD_ONLY,
					  const defaulttheme &new_theme)
{
	invoke_layoutmanager([&](const auto &manager)
			     {
				     manager->theme_updated(IN_THREAD,
							    new_theme);
			     });
}

void containerObj::implObj::ensure_visibility(IN_THREAD_ONLY,
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
::ensured_visibility_of_child_element(IN_THREAD_ONLY,
				      elementObj::implObj &e,
				      const rectangle &r)
{
	const auto &my_pos=get_element_impl().data(IN_THREAD).current_position;

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


	get_element_impl().ensure_visibility(IN_THREAD, {
			new_x, new_y,
				dim_t::truncate(right-new_x),
				dim_t::truncate(bottom-new_y)
				});
}

LIBCXXW_NAMESPACE_END
