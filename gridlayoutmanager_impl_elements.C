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
	auto iter=lookup.find( (*ptr)->grid_element );

	if (iter == lookup.end())
		throw EXCEPTION("Internal element, unable to find grid element in the lookup table.");

	iter->second->border_elements.push_back(border_element);
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
		corner_borderObj::implObj::surrounding_elements_info new_info;

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
				 col->grid_element);
		}

	// Now, add all the borders to the mix.

	for (const auto &b:ge->straight_borders)
	{
		b.second.pos->validate();
		all_elements.emplace_back(b.second.pos,
					  metrics::horizvert(b.second.border
							     ->impl
							     ->get_horizvert
							     (IN_THREAD)),
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
		std::cout << "   TRUE" << std::endl;
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

		auto hv=element->impl->get_horizvert(IN_THREAD);

		// Find the element's first and last row and column.

		const auto &horiz_pos=child.pos->horiz_pos;
		const auto &vert_pos=child.pos->vert_pos;

		auto h_start=elements.horiz_sizes.find(horiz_pos.start);
		auto h_end=elements.horiz_sizes.find(horiz_pos.end);

		auto v_start=elements.horiz_sizes.find(vert_pos.start);
		auto v_end=elements.horiz_sizes.find(vert_pos.end);

		if (h_start == elements.horiz_sizes.end() ||
		    h_end == elements.horiz_sizes.end() ||
		    v_start == elements.vert_sizes.end() ||
		    v_end == elements.vert_sizes.end())
		{
			LOG_FATAL("Internal: cannot find grid rows or columns for an element, horiz_pos=" << horiz_pos.start << "-" << horiz_pos.end
				  << ", vert_pos=" << vert_pos.start << "-" <<vert_pos.end);
			continue;
		}

		coord_t x=std::get<coord_t>(h_start->second);
		coord_t y=std::get<coord_t>(v_start->second);

		// Compute the sum total size of the element's rows and columns

		auto new_width=
			(std::get<coord_t>(h_end->second)
			 + std::get<dim_t>(h_end->second)
			 - x);

		auto new_height=
			(std::get<coord_t>(v_end->second)
			 + std::get<dim_t>(v_end->second)
			 - y);

		if (dim_t::overflows(new_width))
			new_width=std::numeric_limits<dim_t::value_type>::max();

		if (dim_t::overflows(new_height))
			new_height=std::numeric_limits<dim_t::value_type>::max();

		// If the total size of the element's rows and columns
		// exceeds the elements maximum metric, position the
		// element accordingly. Adjust new_width and new_height,
		// so the element does not get resized.

		if (hv->horiz.maximum() != dim_t::infinite())
		{
			auto max_width=hv->horiz.maximum()+dim_t(0);

			if (max_width < new_width)
			{
				dim_squared_t padding=0;

				switch (hv->horizontal_alignment) {
				case metrics::halign::left:
					max_width=new_width;
					break;
				case metrics::halign::center:
					padding=(new_width - max_width) / 2;
					break;
				case metrics::halign::right:
					padding=(new_width - max_width);
					break;
				case metrics::halign::fill:
					max_width=new_width;
					break;
				}
				new_width=max_width;

				x=x.truncate(x+padding);
			}
		}

		if (hv->vert.maximum() != dim_t::infinite())
		{
			auto max_height=hv->vert.maximum()+dim_t(0);

			if (max_height < new_height)
			{
				dim_squared_t padding=0;


				switch (hv->vertical_alignment) {
				case metrics::valign::top:
					max_height=new_height;
					break;
				case metrics::valign::middle:
					padding=(new_height - max_height) / 2;
					break;
				case metrics::valign::bottom:
					padding=(new_height - max_height);
					break;
				case metrics::valign::fill:
					max_height=new_height;
					break;
				}

				new_height=max_height;
				y=y.truncate(y+padding);
			}

		}

		element->impl->update_current_position
			(IN_THREAD,
			 {
				 x, y,
					 dim_t::truncate(new_width),
					 dim_t::truncate(new_height),
			 });
	}
}

LIBCXXW_NAMESPACE_END
