/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager_impl_elements.H"
#include "grid_map_info.H"
#include "catch_exceptions.H"
#include "calculate_borders.H"
#include "straight_border.H"
#include "corner_border.H"
#include "current_border_impl.H"
#include "container_impl.H"
#include "connection_thread.H"
#include "element_screen.H"
#include "element_draw.H"
#include <x/number_hash.H>

LIBCXXW_NAMESPACE_START

// #define PROCESS_UPDATED_POSITION_DEBUG

// #define STRAIGHT_BORDERS_DEBUG

size_t gridlayoutmanagerObj::implObj::elementsObj::border_map_hash
::operator()(const std::tuple<metrics::grid_xy, metrics::grid_xy> &xy) const
{
	return (((size_t)std::get<0>(xy).n) << 16) ^ (size_t)std::get<1>(xy).n;
}

gridlayoutmanagerObj::implObj::elementsObj::elementsObj()=default;

gridlayoutmanagerObj::implObj::elementsObj::~elementsObj()=default;

void gridlayoutmanagerObj::implObj::child_metrics_updated(IN_THREAD_ONLY)
{
	auto &ge=grid_elements(IN_THREAD);

	ge->child_metrics_updated_flag=true;

	layoutmanagerObj::implObj::child_metrics_updated(IN_THREAD);
}

static void register_border(auto &lookup,
			    grid_element *ptr,
			    const element &border_element)
{
	auto iter=lookup.find( (*ptr)->grid_element->impl );

	if (iter == lookup.end())
		throw EXCEPTION("Internal element, unable to find grid element in the lookup table.");

	iter->second->border_elements.push_back(border_element);
}

// When a child element's background color changes, schedule a redraw of its
// surrounding borders.

void gridlayoutmanagerObj::implObj
::redraw_child_borders_and_padding(IN_THREAD_ONLY,
				   const ref<elementObj::implObj> &child)
{
	elementObj::implObj &container_element_impl=get_element_impl();

	if (!container_element_impl.data(IN_THREAD).inherited_visibility)
		return; // This container is not visible, don't bother.

	if (container_element_impl.redraw_scheduled(IN_THREAD))
		return; // This container is scheduled for redrawing anyway.

	// Clear any padding around the element to its background color,
	// borrowing the container code.

	{
		const draw_info &di=container_element_impl
			.get_draw_info(IN_THREAD);
		clip_region_set clip{IN_THREAD,
				container_element_impl.get_window_handler(),
				di};
		rectangle_set dummy;

		container_clear_padding(IN_THREAD,
					container_element_impl,
					*this,
					child,
					di,
					clip,
					dummy);
	}
	grid_map_t::lock lock(grid_map);

	auto &lookup=(*lock)->get_lookup_table();

	auto iter=lookup.find(child);

	if (iter == lookup.end())
		return;

	for (const auto &b:iter->second->border_elements)
		b->impl->schedule_redraw(IN_THREAD);
}

void gridlayoutmanagerObj::implObj::theme_updated(IN_THREAD_ONLY,
						  const defaulttheme &new_theme)
{
	grid_map_t::lock lock(grid_map);

	for_each_child(IN_THREAD,
		       [&]
		       (const auto &child)
		       {
			       child->impl->theme_updated(IN_THREAD, new_theme);
		       });

	(*lock)->padding_recalculated();
	(*lock)->borders_changed();

	needs_recalculation(IN_THREAD);
}

bool gridlayoutmanagerObj::implObj::rebuild_elements(IN_THREAD_ONLY)
{
	grid_map_t::lock lock(grid_map);

	auto &ge=grid_elements(IN_THREAD);

	if (!(*lock)->element_modifications_need_processing())
	{
		// Still need to recalculate everything if child_metrics_updated

		bool flag=ge->child_metrics_updated_flag;

		ge->child_metrics_updated_flag=false;
		return flag;
	}

#ifdef GRID_REBUILD_ELEMENTS
	GRID_REBUILD_ELEMENTS();
#endif

	rebuild_elements_start(IN_THREAD, lock);

	auto &lookup=(*lock)->get_lookup_table();

	// Recalculate the border elements around each grid element. Each
	// grid element can have up to eight borders: four side borders and
	// four corner borders.

	for (auto &lookup_entry:lookup)
	{
		lookup_entry.second->border_elements.clear();
		lookup_entry.second->border_elements.reserve(8);
	}

	//! Clear the existing straight borders

#ifdef STRAIGHT_BORDERS_DEBUG
	std::cout << "*** START STRAIGHT BORDER DEBUG: "
		  << &*ge << std::endl;
#endif
	for (auto &sb:ge->straight_borders)
	{
#ifdef STRAIGHT_BORDERS_DEBUG
		std::cout << "*** STRAIGHT BORDER: EXISTING BORDER: "
			  << &*((element)sb.second.border)->impl << std::endl;
#endif
		sb.second.is_current=false;
	}
	// Clear the existing corner borders
	//
	// Clear both is_new and is_updated. get_corner_border() will set
	// is_updated on every corner border it finds.

	{
		corner_borderObj::implObj::surrounding_elements_and_borders
			new_info;

		for (auto &sb:ge->corner_borders)
		{
			sb.second.is_new=false;
			sb.second.is_updated=false;
			auto &b=*sb.second.border->impl;

			b.old_surrounding_elements(IN_THREAD)=
				b.surrounding_elements(IN_THREAD);

			b.surrounding_elements(IN_THREAD)=new_info;
		}
	}

	calculate_borders((*lock)->elements,
			  // v_lambda()
			  [&]
			  (grid_element *left,
			   grid_element *right,
			   metrics::grid_xy column_number,
			   metrics::grid_xy row_start,
			   metrics::grid_xy row_end)
			  {
				  // If either one does not takes_up_space(),
				  // pretend it does not exist for the purpose
				  // of creating straight borders.

				  if (left && !(*left)->takes_up_space(IN_THREAD))
					  left=nullptr;
				  if (right && !(*right)->takes_up_space(IN_THREAD))
					  right=nullptr;

				  if (!left && !right)
					  return;

				  // Otherwise, let's construct real ptrs.

				  auto eleft=left
					  ? grid_elementptr(*left)
					  : grid_elementptr();

				  auto eright=right
					  ? grid_elementptr(*right)
					  : grid_elementptr();

				  // Check if a default border was given.

				  auto col_default=
					  (*lock)->column_defaults.find
					  (BORDER_COORD_TO_ROWCOL(column_number));

				  current_border_implptr default_border;

				  if (col_default !=
				      (*lock)->column_defaults.end())
					  default_border=col_default->second
						  .default_border;

				  auto b=ge->get_straight_border
					  (IN_THREAD,
					   this->container_impl,
					   &straight_border::base
					   ::create_vertical_border,
					   &straight_border::base
					   ::update_vertical_border,

					   default_border,

					   column_number,
					   column_number,

					   row_start,
					   row_end,
					   eleft, eright);

				  // Now that we have a new straight border,
				  // connect it to the corner on both of
				  // its sides.

				  // The corner border on the top of this new
				  // vertical border.

				  auto cb1=ge->get_corner_border
					  (IN_THREAD,
					   this->container_impl,
					   column_number, row_start-1);

				  // We can set the bottomleft and bottomright
				  // elements in this corner, the corner on
				  // the top end of this vertical border, and
				  // the new straight border we just created
				  // is the bottom border in that corner border.

				  auto &top_info=
					  cb1->impl->surrounding_elements
					  (IN_THREAD);

				  top_info.bottomleft=eleft;
				  top_info.bottomright=eright;
				  top_info.frombottom_border=b;

				  // The corner border on the top of this new
				  // vertical border.

				  auto cb2=ge->get_corner_border
					  (IN_THREAD,
					   this->container_impl,
					   column_number, row_end+1);

				  // We can set the topleft and topright
				  // elements in this corner, the corner on
				  // the bottom end of this vertical border, and
				  // the new straight border we just created
				  // is the top border in that corner border.

				  auto &bottom_info=
					  cb2->impl->surrounding_elements
					  (IN_THREAD);
				  bottom_info.topleft=eleft;
				  bottom_info.topright=eright;
				  bottom_info.fromtop_border=b;

				  // Register the vertical border with the
				  // element on each side of the border.
				  //
				  // We also register the corner borders too.
				  // We only need to register the corner
				  // borders here, and don't need to do it
				  // in h_lambda below, because the same
				  // corner border is seen by both h_lambda
				  // and v_lambda.

				  if (left)
				  {
					  register_border(lookup, left, b);
					  register_border(lookup, left, cb1);
					  register_border(lookup, left, cb2);
				  }

				  if (right)
				  {
					  register_border(lookup, right, b);
					  register_border(lookup, right, cb1);
					  register_border(lookup, right, cb2);
				  }
			  },
			  // h_lambda
			  [&]
			  (grid_element *above,
			   grid_element *below,
			   metrics::grid_xy row_number,
			   metrics::grid_xy col1,
			   metrics::grid_xy col2)
			  {
				  // If either one does not takes_up_space(),
				  // pretend it does not exist for the purpose
				  // of creating straight borders.

				  if (above && !(*above)->takes_up_space(IN_THREAD))
					  above=nullptr;
				  if (below && !(*below)->takes_up_space(IN_THREAD))
					  below=nullptr;

				  if (!above && !below)
					  return;

				  // Otherwise, let's construct real ptrs.

				  auto eabove=above
					  ? grid_elementptr(*above)
					  : grid_elementptr();

				  auto ebelow=below
					  ? grid_elementptr(*below)
					  : grid_elementptr();

				  // Check if a default border was given.

				  auto row_default=
					  (*lock)->row_defaults.find
					  (BORDER_COORD_TO_ROWCOL(row_number));

				  current_border_implptr default_border;

				  if (row_default !=
				      (*lock)->row_defaults.end())
					  default_border=row_default->second
						  .default_border;

				  auto b=ge->get_straight_border
					  (IN_THREAD,
					   this->container_impl,
					   &straight_border::base
					   ::create_horizontal_border,
					   &straight_border::base
					   ::update_horizontal_border,
					   default_border,

					   col1,
					   col2,

					   row_number,
					   row_number,

					   eabove, ebelow);

				  // Register the horizontal border with
				  // the element on each side of the border.

				  if (above)
					  register_border(lookup, above, b);

				  if (below)
					  register_border(lookup, below, b);

				  // Now that we have a new straight border,
				  // connect it to the corner on both of
				  // its sides.

				  // The corner border on the left of this new
				  // horizontal border.

				  auto cb=ge->get_corner_border
					  (IN_THREAD,
					   this->container_impl,
					   col1-1, row_number);

				  // We can set the topright and bottomright
				  // elements in this corner, the corner on
				  // the left end of this horizontal border, and
				  // the new straight border we just created
				  // is the right border in that corner border.

				  auto &left_info=
					  cb->impl->surrounding_elements
					  (IN_THREAD);

				  left_info.topright=eabove;
				  left_info.bottomright=ebelow;
				  left_info.fromright_border=b;

				  // The corner border on the right of this new
				  // horizontal border.

				  cb=ge->get_corner_border
					  (IN_THREAD,
					   this->container_impl,
					   col2+1, row_number);

				  // We can set the topleft and bottomleft
				  // elements in this corner, the corner on
				  // the right end of this horizontal border,
				  // and the new straight border we just created
				  // is the left border in that corner border.

				  auto &right_info=
					  cb->impl->surrounding_elements
					  (IN_THREAD);

				  right_info.topleft=eabove;
				  right_info.bottomleft=ebelow;
				  right_info.fromleft_border=b;
			  });

	// Make a pass, and remove all non-current borders.

	for (auto b=ge->straight_borders.begin(),
		     e=ge->straight_borders.end(); b != e; )
	{
		if (!b->second.is_current)
		{
#ifdef STRAIGHT_BORDERS_DEBUG
			std::cout << "*** STRAIGHT BORDER: REMOVE "
				  << &*((element)b->second.border)->impl << std::endl;
			std::cout << "         " << &*b->second.border << std::endl;
#endif
			b=ge->straight_borders.erase(b);
			continue;
		}

		++b;
	}

	for (auto b=ge->corner_borders.begin(),
		     e=ge->corner_borders.end(); b != e; )
	{
		// Examine each corner border. If it is NOT new...

		if (!b->second.is_new)
		{
			// ... and it has been seen, then it's updated().

			if (b->second.is_updated)
			{
				b->second.border->impl->updated(IN_THREAD);
			}
			else
			{
				// It's not been seen. Remove.

				b=ge->corner_borders.erase(b);
				continue;
			}
		}
		++b;
	}

	size_t total_size=ge->straight_borders.size() +
		ge->corner_borders.size();

	// Now add to total_size the count of all actual grid elements.

	for (const auto &row:(*lock)->elements)
		total_size += row.size();

	std::vector<elementsObj::pos_axis> &all_elements=ge->all_elements;

	all_elements.clear();
	all_elements.reserve(total_size);

	for (const auto &row:(*lock)->elements)
		for (const auto &col:row)
		{
			if (!col->initialized(IN_THREAD))
				col->initialize(IN_THREAD);

			// We don't care about the keys. col is:
			//
			//       grid_element - the child element.
			//       pos     - its metrics::grid_pos
			//
			// The all_elements vector contains a class that
			// inherits from metrics::pos_axis which contains:
			//       pos     - the metrics::grid_pos
			//       horizvert - the element's axises
			//
			// plus the element itself.
			//
			// So this is really just shuffling the deck, a
			// little bit.

			col->pos->validate();
			all_elements.emplace_back
				(col->pos,
				 col->takes_up_space(IN_THREAD),
				 metrics::horizvert(col->grid_element->impl
						    ->get_horizvert(IN_THREAD)),
				 *col,
				 col->grid_element,
				 col->horizontal_alignment,
				 col->vertical_alignment);
		}

	// Now, add all the borders to the mix.

	metrics::pos_axis_padding no_padding;

	for (const auto &b:ge->straight_borders)
	{
		b.second.pos->validate();
		all_elements.emplace_back(b.second.pos,
					  true,
					  metrics::horizvert(b.second.border
							     ->impl
							     ->get_horizvert
							     (IN_THREAD)),
					  no_padding,
					  b.second.border,
					  halign::fill,
					  valign::fill);

#ifdef STRAIGHT_BORDERS_DEBUG
		std::cout << "*** STRAIGHT BORDER: FINAL: "
			  << &*((element)b.second.border)->impl << std::endl;
		std::cout << "         " << &*b.second.border << std::endl;
#endif

	}

	for (const auto &b:ge->corner_borders)
	{
		b.second.pos->validate();
		all_elements.emplace_back(b.second.pos,
					  true,
					  metrics::horizvert(b.second.border
							     ->impl
							     ->get_horizvert
							     (IN_THREAD)),
					  no_padding,
					  b.second.border,
					  halign::fill,
					  valign::fill);
	}

	(*lock)->element_modifications_are_processed();
	ge->child_metrics_updated_flag=false;

#ifdef GRID_REBUILD_ELEMENTS_DONE
	GRID_REBUILD_ELEMENTS_DONE();
#endif

	return true;
}

void gridlayoutmanagerObj::implObj::rebuild_elements_start(IN_THREAD_ONLY,
							   grid_map_t::lock &)
{
}

straight_border gridlayoutmanagerObj::implObj::elementsObj
::get_straight_border(IN_THREAD_ONLY,
		      const ref<containerObj::implObj> &container_impl,
		      straight_border_factory_t factory,
		      straight_border_update_t update,
		      const current_border_implptr &default_border,

		      metrics::grid_xy xstart,
		      metrics::grid_xy xend,
		      metrics::grid_xy ystart,
		      metrics::grid_xy yend,

		      const grid_elementptr &e1,
		      const grid_elementptr &e2)
{
	std::tuple<metrics::grid_xy, metrics::grid_xy> xy{xstart, ystart};

	auto iter=straight_borders.find(xy);

	if (iter != straight_borders.end())
	{
		if (iter->second.pos->horiz_pos.end == xend &&
		    iter->second.pos->vert_pos.end == yend)
		{
			// Preserve the visibility of the
			// updated border.

			bool orig_visibility=iter->second.border
				->impl->data(IN_THREAD).requested_visibility;

#ifdef STRAIGHT_BORDERS_DEBUG
			auto orig_border=iter->second.border;
#endif
			iter->second.border=
				update(IN_THREAD,
				       iter->second.border,
				       e1, e2,
				       default_border);
			iter->second.border->impl->request_visibility
				(IN_THREAD, orig_visibility);
			iter->second.is_current=true;

#ifdef STRAIGHT_BORDERS_DEBUG
			std::cout << "*** STRAIGHT BORDER: RECYCLE "
				  << &*((element)orig_border)->impl
				  << " -> "
				  << &*((element)iter->second.border)->impl << std::endl;
#endif

			return iter->second.border;
		}
#ifdef STRAIGHT_BORDERS_DEBUG
		std::cout << "*** STRAIGHT BORDER: REMOVE "
			  << &*((element)iter->second.border)->impl << std::endl;
#endif
		straight_borders.erase(iter);
	}

	auto new_border=factory(IN_THREAD,
				container_impl, e1, e2, default_border);

	// The new border is visible, by default.
	new_border->impl->request_visibility(IN_THREAD, true);

	auto pos=metrics::grid_pos::create();

	pos->horiz_pos.start=xstart;
	pos->vert_pos.start=ystart;
	pos->horiz_pos.end=xend;
	pos->vert_pos.end=yend;

	straight_borders.insert({xy, {new_border, pos, true}});
#ifdef STRAIGHT_BORDERS_DEBUG
	std::cout << "*** STRAIGHT BORDER: CREATE "
		  << &*((element)new_border)->impl << std::endl;
#endif
	return new_border;
}

corner_border gridlayoutmanagerObj::implObj::elementsObj
::get_corner_border(IN_THREAD_ONLY,
		    const ref<containerObj::implObj> &container_impl,
		    metrics::grid_xy x,
		    metrics::grid_xy y)
{
	// Check if this element exists already.
	std::tuple<metrics::grid_xy, metrics::grid_xy> xy{x, y};

	auto iter=corner_borders.find(xy);

	if (iter != corner_borders.end())
	{
		// If it is, set is_updated.
		iter->second.is_updated=true;
		return iter->second.border;
	}

	// A new corner border element, not seen before. Here's it's position.

	auto pos=metrics::grid_pos::create();

	pos->horiz_pos.start=x;
	pos->vert_pos.start=y;
	pos->horiz_pos.end=x;
	pos->vert_pos.end=y;

	auto impl=ref<corner_borderObj::implObj>::create(container_impl);

	auto new_border=corner_border::create(impl);

	// The new border is visible, by default.
	new_border->impl->request_visibility(IN_THREAD, true);

	// is_new=true
	// is_updated=false
	corner_borders.insert({xy, {new_border, pos, true, false}});
	return new_border;
}

void gridlayoutmanagerObj::implObj::initialize_new_elements(IN_THREAD_ONLY)
{
	grid_map_t::lock lock(grid_map);

	bool flag;

	// Before calculating the metrics, check if any of these
	// need to be initialize()d.

	do
	{
		for_each_child
			(IN_THREAD,
			 [&]
			 (const auto &child)
			 {
				 child->impl->initialize_if_needed(IN_THREAD);
			 });

		// initialize_new_elements() was invoked only if
		// rebuild_elements() retursn true.
		//
		// Maybe the initialize() callback add more elements
		// to this grid? If so, let's do this again.

		flag=rebuild_elements(IN_THREAD);
	} while (flag);
}

void gridlayoutmanagerObj::implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const element &e)> &callback)
{
	// It's possible that we get here before processing any new elements
	// that were added to the grid. So iterating over
	// grid_elements->all_elements is not enough. First, copy all the
	// "official" grid elements.

	std::unordered_set<element> all_elements;

	{
		grid_map_t::lock lock(grid_map);

		for (const auto &row:(*lock)->elements)
			for (const auto &col:row)
			{
				all_elements.insert(col->grid_element);
				col->grid_element->impl
					->initialize_if_needed(IN_THREAD);

				// And pass them to the callback.
				callback(col->grid_element);
			}
	}

	// Now, pass over all_elements, skipping the ones that were already
	// called.
	for (const auto &child:grid_elements(IN_THREAD)->all_elements)
	{
		if (all_elements.find(child.child_element) ==
		    all_elements.end())
			callback(child.child_element);
	}
}

std::tuple<bool, metrics::axis, metrics::axis>
gridlayoutmanagerObj::implObj::elementsObj
::recalculate_metrics(IN_THREAD_ONLY,
		      bool flag)
{
	auto do_not_expand_borders=[]
		(metrics::grid_xy rowcol)
		{
			return IS_BORDER_RESERVED_COORD(rowcol);
		};

	auto new_horiz_metrics=
		metrics::calculate_grid_horiz_metrics(all_elements,
						      do_not_expand_borders);

	auto new_vert_metrics=
		metrics::calculate_grid_vert_metrics(all_elements,
						     do_not_expand_borders);

#if 0

	bool has_canvas=false;

	for (const auto &e:all_elements)
		if (e.child_element->impl->objname() == "x::w::canvasObj::implObj")
			has_canvas=true;

	if (has_canvas)
	{
		std::cout << "METRICS:" << std::endl;

		for (const auto &m:new_horiz_metrics)
			std::cout << "   " << m.first
				  << "=" << m.second
				  << std::endl;
	}
#endif

	if (horiz_metrics != new_horiz_metrics ||
	    vert_metrics != new_vert_metrics)
	{
		flag=true;
	}

	horiz_metrics=new_horiz_metrics;
	vert_metrics=new_vert_metrics;

	return {flag, total_metrics(horiz_metrics),
			total_metrics(vert_metrics) };
}

metrics::axis gridlayoutmanagerObj::implObj::elementsObj
::total_metrics(const metrics::grid_metrics_t &metrics)
{
	metrics::axis total{0, 0, 0};

	auto b=metrics.begin();
	auto e=metrics.end();

	if (b != e)
	{
		total=b->second;
		++b;
	}

	while (b != e)
	{
		total=total+b->second;
		++b;
	}

	return total;
}


bool gridlayoutmanagerObj::implObj::elementsObj
::recalculate_sizes(const grid_map_t::lock &lock,
		    dim_t target_width,
		    dim_t target_height)
{
	bool flag=false;

	// Requested axis size:

	// For axises that are used by collapsed border elements always return
	// requested size of 0 (which will be automatically increased by any
	// minimum required to draw the border).
	//
	// Otherwise we consult (column|row)_defaults.axis_size.
	//
	// The requested axis size is used only when the
	// total recalculated size based on each row/column's
	// preferred/minimum/maximum is still less than target_width/height,
	// and then we'll look for any specified sizes.

	std::unordered_map<metrics::grid_xy, int> axis_sizes;

	for (const auto &as: (*lock)->column_defaults)
	{
		if (as.second.axis_size < 0)
			continue;

		axis_sizes.insert({CALCULATE_BORDERS_COORD(as.first),
					as.second.axis_size});
	}

	auto lookup=make_function<metrics::get_req_axis_size_t>
		([&]
		 (const metrics::grid_xy &xy)
		 {
			 if (IS_BORDER_RESERVED_COORD(xy))
				 return 0;

			 auto iter=axis_sizes.find(xy);

			 if (iter == axis_sizes.end())
				 return -1;

			 return iter->second;
		 });

	if (metrics::do_calculate_grid_size(horiz_metrics,
					    horiz_sizes,
					    target_width, lookup))
		flag=true;

	axis_sizes.clear();
	for (const auto &as: (*lock)->row_defaults)
	{
		if (as.second.axis_size < 0)
			continue;

		axis_sizes.insert({CALCULATE_BORDERS_COORD(as.first),
					as.second.axis_size});
	}

	if (metrics::do_calculate_grid_size(vert_metrics,
					    vert_sizes,
					    target_height, lookup))
		flag=true;

	return flag;
}

void gridlayoutmanagerObj::implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	auto &elements=*grid_elements(IN_THREAD);

	// Ignore the return value from recalculate_sizes(). We can get
	// here when the child metrics have changed. This may not necessarily
	// result in the sizes of the rows and columns changing. However if
	// the child element's maximum metrics have changed, the child
	// element might need to be reposition within its cells, by the
	// code below.
	elements.recalculate_sizes(grid_map_t::lock(grid_map),
				   position.width, position.height);

#ifdef PROCESS_UPDATED_POSITION_DEBUG

	rectangle_set total_set;

	std::cout << "----" << std::endl;
#endif

	for (const auto &child:elements.all_elements)
	{
		const auto &element=child.child_element;

		// If this element does not takes_up_space(), we need
		// to make it disappear. Otherwise it could still attempt
		// to try to erase itself (to its background color);
		// meanwhile we have other elements using its real estate.
		// Simply position the element outside of our drawing area.

		if (!child.takes_up_space)
		{
			// But we need to logically set its size to (0,0)

			auto p=element->impl->data(IN_THREAD)
				.current_position;

			p.x=coord_t::truncate(position.width);
			p.y=0;

			element->impl->update_current_position(IN_THREAD, p);
			continue;
		}

		const metrics::pos_axis_padding &padding=child.padding;

		auto hv=element->impl->get_horizvert(IN_THREAD);

		auto element_position=
			elements.compute_element_position(child.pos);

		// Adjust for the element's requested padding. Reduce
		// the computed position by the element's stated padding,
		// as the first order of business.

		if (element_position.width >=
		    dim_t::value_type(padding.total_horiz_padding))
		{
			element_position.width -= padding.total_horiz_padding;
			element_position.x = coord_t::truncate
				(element_position.x+padding.left_padding);
		}

		if (element_position.height >=
		    dim_t::value_type(padding.total_vert_padding))
		{
			element_position.height -= padding.total_vert_padding;
			element_position.y = coord_t::truncate
				(element_position.y+padding.top_padding);
		}

		// Take this element's maximum metrics. If they're larger
		// than the alloted element-position, reduce them.
		// At this point we expect the grid to always give us at
		// least as much room as as the minimums.
		//
		// Note that dim_t::infinite will always be bigger than
		// the element_position.

		dim_t max_width=hv->horiz.maximum();
		dim_t max_height=hv->vert.maximum();

		if (max_width > element_position.width)
			max_width=element_position.width;

		if (max_height > element_position.height)
			max_height=element_position.height;

		// And if max_width and max_height is smaller than
		// element_position, then align() the element accordingly.

		auto aligned=metrics::align(element_position.width,
					    element_position.height,
					    max_width,
					    max_height,
					    child.horizontal_alignment,
					    child.vertical_alignment);

		element_position.x=
			element_position.x.truncate
			(element_position.x+aligned.x);
		element_position.y=
			element_position.y.truncate
			(element_position.y+aligned.y);

		element_position.width=aligned.width;
		element_position.height=aligned.height;

		element->impl->update_current_position(IN_THREAD,
						       element_position);

#ifdef PROCESS_UPDATED_POSITION_DEBUG
		if (element_position.width > 0 && element_position.height > 0)
		{
			rectangle_set check_overlap=intersect(total_set, {
					element_position
					});

			if (!check_overlap.empty())
				abort();

			total_set=add(total_set, { element_position });
			std::cout << "RECT: " << element_position
				  << " ("
				  << element->impl->objname()
				  << " "
				  << &*element->impl
				  << ")"
				  << std::endl;
		}
#endif
	}
#ifdef PROCESS_UPDATED_POSITION_DEBUG
	std::cout << "SET: " << total_set.size() << std::endl;
#endif
}

rectangle gridlayoutmanagerObj::implObj
::padded_position(IN_THREAD_ONLY,
		  const ref<elementObj::implObj> &e_impl)
{
	rectangle ret=layoutmanagerObj::implObj::padded_position(IN_THREAD,
								 e_impl);

	grid_map_t::lock lock(grid_map);

	auto &lookup=(*lock)->get_lookup_table();

	auto iter=lookup.find(e_impl);

	if (iter == lookup.end())
		return ret; // This is probably a border element. No padding.

	auto ge=(*lock)->elements.at(iter->second->row).at(iter->second->col);

	if (!ge->initialized(IN_THREAD) ||
	    !ge->takes_up_space(IN_THREAD))
		return ret;

	return grid_elements(IN_THREAD)->compute_element_position(ge->pos);
}

rectangle gridlayoutmanagerObj::implObj::elementsObj
::compute_element_position(const metrics::grid_pos &pos)
{
	// Find the element's first and last row and column.

	const auto &horiz_pos=pos->horiz_pos;
	const auto &vert_pos=pos->vert_pos;

	auto h_start=horiz_sizes.find(horiz_pos.start);
	auto h_end=horiz_sizes.find(horiz_pos.end);

	auto v_start=vert_sizes.find(vert_pos.start);
	auto v_end=vert_sizes.find(vert_pos.end);

	if (h_start == horiz_sizes.end() ||
	    h_end == horiz_sizes.end() ||
	    v_start == vert_sizes.end() ||
	    v_end == vert_sizes.end())
	{
		LOG_FATAL("Internal: cannot find grid rows or columns for an element, horiz_pos=" << horiz_pos.start << "-" << horiz_pos.end
			  << ", vert_pos=" << vert_pos.start << "-" <<vert_pos.end);
		return {};
	}

	coord_t x=std::get<coord_t>(h_start->second);
	coord_t y=std::get<coord_t>(v_start->second);

	// Compute the sum total size of the element's rows and columns

	dim_t new_width=dim_t::truncate
		(std::get<coord_t>(h_end->second)
		 + std::get<dim_t>(h_end->second)
		 - x);

	dim_t new_height=dim_t::truncate
		(std::get<coord_t>(v_end->second)
		 + std::get<dim_t>(v_end->second)
		 - y);

	return {x, y, new_width, new_height};
}


LIBCXXW_NAMESPACE_END
