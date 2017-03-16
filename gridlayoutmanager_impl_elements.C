/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager_impl_elements.H"
#include "catch_exceptions.H"
#include "calculate_borders.H"
#include "straight_border.H"
#include "corner_border.H"
#include "current_border_impl.H"
#include "container_impl.H"
#include "element_screen.H"
#include "element_draw.H"
#include <x/number_hash.H>

LIBCXXW_NAMESPACE_START

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
	elementObj::implObj &container_element_impl=
		container_impl->get_element_impl();

	if (!container_element_impl.data(IN_THREAD).inherited_visibility)
		return; // This container is not visible, don't bother.

	// Clear any padding around the element to its background color,
	// borrowing the container code.

	{
		const draw_info &di=container_element_impl
			.get_draw_info(IN_THREAD);
		clip_region_set clip{IN_THREAD,  di};
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

	auto &lookup=lock->get_lookup_table();

	auto iter=lookup.find(child);

	if (iter == lookup.end())
		return;

	for (const auto &b:iter->second->border_elements)
		b->impl->schedule_redraw_if_visible(IN_THREAD);
}

void gridlayoutmanagerObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	grid_map_t::lock lock(grid_map);

	grid_element_padding_lock
		padding_lock{container_impl->get_element_impl().get_screen()};

	for (const auto &row:lock->elements)
		for (const auto &col:row)
			col->calculate_padding(padding_lock);

	lock->padding_recalculated();

	needs_recalculation(IN_THREAD);
}

bool gridlayoutmanagerObj::implObj::rebuild_elements(IN_THREAD_ONLY)
{
	grid_map_t::lock lock(grid_map);

	auto &ge=grid_elements(IN_THREAD);

	if (!lock->element_modifications_need_processing())
	{
		// Still need to recalculate everything if child_metrics_updated

		bool flag=ge->child_metrics_updated_flag;

		ge->child_metrics_updated_flag=false;
		return flag;
	}

#ifdef GRID_REBUILD_ELEMENTS
	GRID_REBUILD_ELEMENTS();
#endif

	auto &lookup=lock->get_lookup_table();

	// Recalculate the border elements around each grid element. Each
	// grid element can have up to eight borders: four side borders and
	// four corner borders.

	for (auto &lookup_entry:lookup)
	{
		lookup_entry.second->border_elements.clear();
		lookup_entry.second->border_elements.reserve(8);
	}

	//! Clear the existing straight borders

	for (auto &sb:ge->straight_borders)
		sb.second.is_current=false;

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

	calculate_borders(lock->elements,
			  // v_lambda()
			  [&]
			  (grid_element *left,
			   grid_element *right,
			   metrics::grid_xy column_number,
			   metrics::grid_xy row_start,
			   metrics::grid_xy row_end)
			  {
				  auto eleft=left
					  ? grid_elementptr(*left)
					  : grid_elementptr();

				  auto eright=right
					  ? grid_elementptr(*right)
					  : grid_elementptr();

				  auto b=ge->get_straight_border
					  (IN_THREAD,
					   this->container_impl,
					   &straight_border::base
					   ::create_vertical_border,
					   &straight_border::base
					   ::update_vertical_border,

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
				  auto eabove=above
					  ? grid_elementptr(*above)
					  : grid_elementptr();

				  auto ebelow=below
					  ? grid_elementptr(*below)
					  : grid_elementptr();

				  auto b=ge->get_straight_border
					  (IN_THREAD,
					   this->container_impl,
					   &straight_border::base
					   ::create_horizontal_border,
					   &straight_border::base
					   ::update_horizontal_border,

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

	for (const auto &row:lock->elements)
		total_size += row.size();

	std::vector<elementsObj::pos_axis> &all_elements=ge->all_elements;

	all_elements.clear();
	all_elements.reserve(total_size);

	for (const auto &row:lock->elements)
		for (const auto &col:row)
		{
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
				 metrics::horizvert(col->grid_element->impl
						    ->get_horizvert(IN_THREAD)),
				 *col,
				 col->grid_element);
		}

	// Now, add all the borders to the mix.

	metrics::pos_axis_padding no_padding;

	for (const auto &b:ge->straight_borders)
	{
		b.second.pos->validate();
		all_elements.emplace_back(b.second.pos,
					  metrics::horizvert(b.second.border
							     ->impl
							     ->get_horizvert
							     (IN_THREAD)),
					  no_padding,
					  b.second.border);
	}

	for (const auto &b:ge->corner_borders)
	{
		b.second.pos->validate();
		all_elements.emplace_back(b.second.pos,
					  metrics::horizvert(b.second.border
							     ->impl
							     ->get_horizvert
							     (IN_THREAD)),
					  no_padding,
					  b.second.border);
	}

	lock->element_modifications_are_processed();
	ge->child_metrics_updated_flag=false;

#ifdef GRID_REBUILD_ELEMENTS_DONE
	GRID_REBUILD_ELEMENTS_DONE();
#endif

	return true;
}

straight_border gridlayoutmanagerObj::implObj::elementsObj
::get_straight_border(IN_THREAD_ONLY,
		      const ref<containerObj::implObj> &container_impl,
		      straight_border_factory_t factory,
		      straight_border_update_t update,

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

			iter->second.border=
				update(IN_THREAD,
				       iter->second.border,
				       e1, e2,

				       // TODO: default border
				       current_border_implptr());
			iter->second.border->impl->request_visibility
				(IN_THREAD, orig_visibility);
			iter->second.is_current=true;
			return iter->second.border;
		}
		straight_borders.erase(iter);
	}

	auto new_border=factory(container_impl, e1, e2,

				// TODO: default border
				current_border_implptr());

	// The new border is visible, by default.
	new_border->impl->request_visibility(IN_THREAD, true);

	auto pos=metrics::grid_pos::create();

	pos->horiz_pos.start=xstart;
	pos->vert_pos.start=ystart;
	pos->horiz_pos.end=xend;
	pos->vert_pos.end=yend;

	straight_borders.insert({xy, {new_border, pos, true}});
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
	bool flag;

	// Before calculating the metrics, check if any of these
	// need to be initialize()d.

	do
	{
		for (const auto &element:grid_elements(IN_THREAD)->all_elements)
		{
			try {
				element.child_element->impl
					->initialize_if_needed(IN_THREAD);
			} CATCH_EXCEPTIONS;
		}

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
	for (const auto &child:grid_elements(IN_THREAD)->all_elements)
		callback(child.child_element);
}

bool gridlayoutmanagerObj::implObj::elementsObj
::recalculate_metrics(IN_THREAD_ONLY,
		      bool flag,
		      const metrics::horizvert &my_metrics)
{
	auto new_horiz_metrics=
		metrics::calculate_grid_horiz_metrics(all_elements);

	auto new_vert_metrics=
		metrics::calculate_grid_vert_metrics(all_elements);

	if (!flag && (horiz_metrics != new_horiz_metrics ||
		      vert_metrics != new_vert_metrics))
	{
		flag=true;
	}

	horiz_metrics=new_horiz_metrics;
	vert_metrics=new_vert_metrics;

	my_metrics->set_element_metrics(IN_THREAD,
					total_metrics(horiz_metrics),
					total_metrics(vert_metrics));

	return flag;
}

metrics::axis gridlayoutmanagerObj::implObj::elementsObj
::total_metrics(const metrics::grid_metrics_t &metrics)
{
	metrics::axis total;

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
::recalculate_sizes(dim_t target_width,
		    dim_t target_height)
{
	bool flag=false;

	if (metrics::calculate_grid_size(horiz_metrics,
					 horiz_sizes,
					 target_width))
		flag=true;

	if (metrics::calculate_grid_size(vert_metrics,
					 vert_sizes,
					 target_height))
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
	elements.recalculate_sizes(position.width, position.height);

	for (const auto &child:elements.all_elements)
	{
		const auto &element=child.child_element;

		const metrics::pos_axis_padding &padding=child.padding;

		auto hv=element->impl->get_horizvert(IN_THREAD);

		auto element_position=
			elements.compute_element_position(child);

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
				(element_position.y+padding.left_padding);
		}

		// If the total size of the element's rows and columns
		// exceeds the elements maximum metric, position the
		// element accordingly. width and height,
		// so the element does not get resized.

		if (hv->horiz.maximum() != dim_t::infinite())
		{
			dim_t max_width=hv->horiz.maximum();

			if (max_width < element_position.width)
			{
				dim_t padding=0;

				switch (hv->horizontal_alignment) {
				case metrics::halign::left:
					max_width=element_position.width;
					break;
				case metrics::halign::center:
					padding=(element_position.width - max_width) / 2;
					break;
				case metrics::halign::right:
					padding=(element_position.width - max_width);
					break;
				case metrics::halign::fill:
					max_width=element_position.width;
					break;
				}
				element_position.width=max_width;

				element_position.x=
					element_position.x.truncate
					(element_position.x+padding);
			}
		}

		if (hv->vert.maximum() != dim_t::infinite())
		{
			dim_t max_height=hv->vert.maximum();

			if (max_height < element_position.height)
			{
				dim_t padding=0;

				switch (hv->vertical_alignment) {
				case metrics::valign::top:
					max_height=element_position.height;
					break;
				case metrics::valign::middle:
					padding=(element_position.height - max_height) / 2;
					break;
				case metrics::valign::bottom:
					padding=(element_position.height - max_height);
					break;
				case metrics::valign::fill:
					max_height=element_position.height;
					break;
				}

				element_position.height=max_height;
				element_position.y=
					element_position.y.truncate
					(element_position.y+padding);
			}

		}

		element->impl->update_current_position(IN_THREAD,
						       element_position);
	}
}

rectangle gridlayoutmanagerObj::implObj
::padded_position(IN_THREAD_ONLY,
		  const ref<elementObj::implObj> &e_impl)
{
	rectangle ret=layoutmanagerObj::implObj::padded_position(IN_THREAD,
								 e_impl);

	grid_map_t::lock lock(grid_map);

	auto &lookup=lock->get_lookup_table();

	auto iter=lookup.find(e_impl);

	if (iter == lookup.end())
		return ret; // This is probably a border element. No padding.

	auto ge=lock->elements.at(iter->second->row).at(iter->second->col);

	ret.x=coord_t::truncate(ret.x-ge->left_padding);
	ret.y=coord_t::truncate(ret.y-ge->top_padding);

	ret.width=dim_t::truncate(ret.width+ge->total_horiz_padding);
	ret.height=dim_t::truncate(ret.height+ge->total_vert_padding);

	return ret;
}

rectangle gridlayoutmanagerObj::implObj::elementsObj
::compute_element_position(const pos_axis &child)
{
	// Find the element's first and last row and column.

	const auto &horiz_pos=child.pos->horiz_pos;
	const auto &vert_pos=child.pos->vert_pos;

	auto h_start=horiz_sizes.find(horiz_pos.start);
	auto h_end=horiz_sizes.find(horiz_pos.end);

	auto v_start=horiz_sizes.find(vert_pos.start);
	auto v_end=horiz_sizes.find(vert_pos.end);

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
